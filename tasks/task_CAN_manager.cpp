#include "task_CAN_manager.h"

TaskHandle_t th_can_manager = nullptr;
QueueHandle_t can_rx_queue = nullptr;
QueueHandle_t can_tx_queue = nullptr;

// The CAN instance this manager owns (set from pvParameters at task start).
static CAN *g_can = nullptr;

/**
 * @brief Resolve the RX FIFO 0 interrupt line for the given bxCAN peripheral.
 */
static IRQn_Type can_rx0_irqn(CAN_TypeDef *instance)
{
    if (instance == CAN2) {
        return CAN2_RX0_IRQn;
    }
    return CAN1_RX0_IRQn;
}

void can_manager_init(void)
{
    if (!can_rx_queue) {
        can_rx_queue = xQueueCreate(CAN_RX_QUEUE_DEPTH, sizeof(CANMessage));
    }
    if (!can_tx_queue) {
        can_tx_queue = xQueueCreate(CAN_TX_QUEUE_DEPTH, sizeof(CANMessage));
    }
}

void task_CAN_manager(void *pvParameters)
{
    g_can = (CAN *)pvParameters;

    th_can_manager = xTaskGetCurrentTaskHandle();

    // Safety net in case can_manager_init() was not called before the scheduler started.
    can_manager_init();

    // Wait until the application has initialized + started the peripheral before we
    // enable notifications (CAN::init() moves the handle out of the RESET state).
    while (g_can == nullptr || HAL_CAN_GetState(&g_can->_hcan) == HAL_CAN_STATE_RESET) {
        vTaskDelay(pdMS_TO_TICKS(5));
    }

    // Enable interrupt-driven reception on RX FIFO 0.
    HAL_StatusTypeDef status = HAL_CAN_ActivateNotification(&g_can->_hcan, CAN_IT_RX_FIFO0_MSG_PENDING);
    OK_ERROR_HANDLER(status, "HAL_CAN_ActivateNotification");

    IRQn_Type irqn = can_rx0_irqn(g_can->_hcan.Instance);
    HAL_NVIC_SetPriority(irqn, RTOS_ISR_DEFAULT_PRIORITY, 0);
    HAL_NVIC_EnableIRQ(irqn);

    CANMessage tx;
    while (1)
    {
        // Block until the application queues a frame to transmit. RX needs no polling
        // here because it is serviced entirely in the ISR below.
        if (xQueueReceive(can_tx_queue, &tx, portMAX_DELAY) == pdPASS)
        {
            g_can->transmit(tx.id, tx.data, tx.len, tx.extended, 100);
        }
    }
}

/* *******************************************************
 * ISR bridge: the RX FIFO 0 "message pending" interrupt drains every pending frame
 * into can_rx_queue. Running in ISR context keeps latency to microseconds and prevents
 * the 3-deep hardware FIFO from overflowing under bursty traffic.
 * ******************************************************* */

extern "C" void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
    if (!g_can || hcan != &g_can->_hcan) {
        return;
    }

    BaseType_t higher_priority_woken = pdFALSE;
    CAN_RxHeaderTypeDef header;
    CANMessage msg;

    // Drain all frames currently in the FIFO (may be more than one).
    while (HAL_CAN_GetRxFifoFillLevel(hcan, CAN_RX_FIFO0) > 0)
    {
        if (HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &header, msg.data) != HAL_OK) {
            break;
        }

        msg.extended = (header.IDE == CAN_ID_EXT);
        msg.id = msg.extended ? header.ExtId : header.StdId;
        msg.len = (uint8_t)header.DLC;

        // If the queue is full the frame is dropped (oldest data is preserved).
        (void)xQueueSendToBackFromISR(can_rx_queue, &msg, &higher_priority_woken);
    }

    portYIELD_FROM_ISR(higher_priority_woken);
}

extern "C" void CAN1_RX0_IRQHandler(void)
{
    if (g_can && g_can->_hcan.Instance == CAN1) {
        HAL_CAN_IRQHandler(&g_can->_hcan);
    }
}

extern "C" void CAN2_RX0_IRQHandler(void)
{
    if (g_can && g_can->_hcan.Instance == CAN2) {
        HAL_CAN_IRQHandler(&g_can->_hcan);
    }
}

/* *******************************************************
 * Application interface
 * ******************************************************* */

BaseType_t can_manager_send(uint32_t id, uint8_t *data, uint8_t len, bool extended /*=false*/, TickType_t timeout /*=pdMS_TO_TICKS(10)*/)
{
    if (can_tx_queue == nullptr) {
        return pdFAIL;
    }

    if (len > 8) {
        len = 8;
    }

    CANMessage msg;
    msg.id = id;
    msg.len = len;
    msg.extended = extended;
    memcpy(msg.data, data, len);

    return xQueueSend(can_tx_queue, &msg, timeout);
}

BaseType_t can_manager_send(const CANMessage &msg, TickType_t timeout /*=pdMS_TO_TICKS(10)*/)
{
    if (can_tx_queue == nullptr) {
        return pdFAIL;
    }
    return xQueueSend(can_tx_queue, &msg, timeout);
}

BaseType_t can_manager_receive(CANMessage *msg, TickType_t timeout /*=portMAX_DELAY*/)
{
    if (can_rx_queue == nullptr) {
        return pdFAIL;
    }
    return xQueueReceive(can_rx_queue, msg, timeout);
}

/* *******************************************************
 * How to receive CAN messages from an application
 * *******************************************************
 *
 * Received frames are pushed into can_rx_queue by the RX ISR, so the correct way
 * to consume them is a dedicated task that blocks on can_manager_receive(). Do NOT
 * call it from a latency-sensitive task (ex. the main clock task): a blocked task
 * costs zero CPU (the scheduler runs everyone else while it sleeps), but you don't
 * want your clock task to be the one parked waiting on a frame.
 *
 *   void task_can_rx(void *pv)
 *   {
 *       can_manager_init();
 *       CANMessage msg;
 *       for (;;) {
 *           // Blocks (sleeps) until a frame arrives; wakes only when one is ready.
 *           if (can_manager_receive(&msg, portMAX_DELAY) == pdPASS) {
 *               // Act on the frame here: msg.id, msg.data[0..msg.len), msg.extended
 *               // e.g. forward to USB, update the metronome, dispatch an event, etc.
 *           }
 *       }
 *   }
 *
 *   // create it alongside the other tasks:
 *   xTaskCreate(task_can_rx, "CAN rx", 256, NULL, 3, NULL);
 *
 * To poll without blocking (only useful if you already have a periodic tick to call
 * it from), pass a zero timeout and drain whatever is currently queued:
 *
 *   CANMessage msg;
 *   while (can_manager_receive(&msg, 0) == pdPASS) {
 *       // handle msg
 *   }
 *
 * If the data must reach an event-driven task (ex. one blocked on queue_main), have
 * the RX task stash the payload and dispatch an event to wake that task, rather than
 * having the event-driven task block on the CAN queue directly.
 */
