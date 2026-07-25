#include "CAN.h"

Mutex CAN::_mutex;

/**
 * @brief Initialize the CAN peripheral based on the selected instance.
 * @param prescaler baudrate prescaler for setting the bus bit rate (defaults to 6)
 * @param mode operating mode (CAN_MODE_NORMAL, CAN_MODE_LOOPBACK, ...)
 *
 * @note Bit timing is derived from APB1. With APB1 @ 45 MHz the default
 * prescaler of 6 and (BS1 = 12TQ, BS2 = 2TQ, SJW = 1TQ) gives:
 * 45 MHz / (6 * (1 + 12 + 2)) = 500 kbit/s at an ~86% sample point.
 */
void CAN::init(uint32_t prescaler /*=6*/, uint32_t mode /*=CAN_MODE_NORMAL*/)
{
    _mutex.lock();

    HAL_StatusTypeDef status;

    /* Peripheral clock enable */
    uint8_t alternate;
    if (this->instance == CAN1) {
        __HAL_RCC_CAN1_CLK_ENABLE();
        alternate = GPIO_AF9_CAN1;
    } else {
        /* CAN2 is a slave of CAN1 and shares its filter block, so CAN1 must be clocked too */
        __HAL_RCC_CAN1_CLK_ENABLE();
        __HAL_RCC_CAN2_CLK_ENABLE();
        alternate = GPIO_AF9_CAN2;
    }

    /* CAN GPIO configuration (RX + TX as alternate function push-pull) */
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = alternate;

    gpio_enable_clock(_rx);
    GPIO_InitStruct.Pin = gpio_get_pin(_rx);
    HAL_GPIO_Init(gpio_get_port(_rx), &GPIO_InitStruct);

    gpio_enable_clock(_tx);
    GPIO_InitStruct.Pin = gpio_get_pin(_tx);
    HAL_GPIO_Init(gpio_get_port(_tx), &GPIO_InitStruct);

    /* CAN peripheral configuration */
    _hcan.Instance = this->instance;
    _hcan.Init.Prescaler = prescaler;
    _hcan.Init.Mode = mode;
    _hcan.Init.SyncJumpWidth = CAN_SJW_1TQ;
    _hcan.Init.TimeSeg1 = CAN_BS1_12TQ;
    _hcan.Init.TimeSeg2 = CAN_BS2_2TQ;
    _hcan.Init.TimeTriggeredMode = DISABLE;
    _hcan.Init.AutoBusOff = ENABLE;
    _hcan.Init.AutoWakeUp = DISABLE;
    _hcan.Init.AutoRetransmission = ENABLE;
    _hcan.Init.ReceiveFifoLocked = DISABLE;
    _hcan.Init.TransmitFifoPriority = DISABLE;

    status = HAL_CAN_Init(&_hcan);
    OK_ERROR_HANDLER(status, "HAL_CAN_Init");

    /* Accept-all filter routed to RX FIFO 0.
     * CAN1 and CAN2 share one bank of 28 filters, split at SlaveStartFilterBank:
     * banks 0..13 belong to CAN1, banks 14..27 belong to CAN2. A CAN2 filter
     * MUST live in its own range or hardware drops every frame (FIFO never fills). */
    CAN_FilterTypeDef filter = {0};
    filter.FilterBank = (this->instance == CAN1) ? 0 : 14;
    filter.FilterMode = CAN_FILTERMODE_IDMASK;
    filter.FilterScale = CAN_FILTERSCALE_32BIT;
    filter.FilterIdHigh = 0x0000;
    filter.FilterIdLow = 0x0000;
    filter.FilterMaskIdHigh = 0x0000;
    filter.FilterMaskIdLow = 0x0000;
    filter.FilterFIFOAssignment = CAN_RX_FIFO0;
    filter.FilterActivation = CAN_FILTER_ENABLE;
    filter.SlaveStartFilterBank = 14;

    status = HAL_CAN_ConfigFilter(&_hcan, &filter);
    OK_ERROR_HANDLER(status, "HAL_CAN_ConfigFilter");

    status = HAL_CAN_Start(&_hcan);
    OK_ERROR_HANDLER(status, "HAL_CAN_Start");

    _mutex.unlock();
}

/**
 * @brief Transmit a data frame on the bus.
 *
 * @param id message identifier
 * @param data pointer to the payload to send (up to 8 bytes)
 * @param length number of payload bytes (clamped to 8)
 * @param extended true for a 29-bit extended id, false for an 11-bit standard id
 * @param timeout_ms how long to wait for a free transmit mailbox before giving up
 * @return HAL_StatusTypeDef HAL_OK on success, HAL_TIMEOUT if no mailbox freed in time
 */
HAL_StatusTypeDef CAN::transmit(uint32_t id, uint8_t *data, uint8_t length, bool extended /*=false*/, uint32_t timeout_ms /*=100*/)
{
    _mutex.lock();
    HAL_StatusTypeDef status;

    if (length > 8) {
        length = 8;
    }

    CAN_TxHeaderTypeDef header = {0};
    if (extended) {
        header.ExtId = id;
        header.IDE = CAN_ID_EXT;
    }
    else {
        header.StdId = id;
        header.IDE = CAN_ID_STD;
    }
    header.RTR = CAN_RTR_DATA;
    header.DLC = length;
    header.TransmitGlobalTime = DISABLE;

    /* Wait (bounded) for a free transmit mailbox. Without a bound the calling task
     * would block forever if frames are never acknowledged (ex. no other node on the
     * bus, or a mis-wired transceiver), since AutoRetransmission keeps the mailbox busy. */
    uint32_t elapsed = 0;
    while (HAL_CAN_GetTxMailboxesFreeLevel(&_hcan) == 0) {
        if (timeout_ms != HAL_MAX_DELAY && elapsed >= timeout_ms) {
            _mutex.unlock();
            return HAL_TIMEOUT;
        }
        vTaskDelay(pdMS_TO_TICKS(1));
        elapsed++;
    }

    uint32_t mailbox;
    status = HAL_CAN_AddTxMessage(&_hcan, &header, data, &mailbox);
    if (status != HAL_OK) {
        OK_ERROR_HANDLER(status, "HAL_CAN_AddTxMessage");
    }

    _mutex.unlock();
    return status;
}

/**
 * @brief Receive a data frame from RX FIFO 0.
 *
 * @param id [out] identifier of the received frame (standard or extended)
 * @param data [out] destination buffer (must hold at least 8 bytes)
 * @param length [out] number of payload bytes received
 * @param timeout_ms how long to wait for a frame to arrive
 * @return HAL_StatusTypeDef HAL_OK on success, HAL_TIMEOUT if nothing arrived
 */
HAL_StatusTypeDef CAN::receive(uint32_t *id, uint8_t *data, uint8_t *length, uint32_t timeout_ms /*=HAL_MAX_DELAY*/)
{
    _mutex.lock();
    HAL_StatusTypeDef status;

    /* Poll RX FIFO 0 until a message is pending or the timeout elapses */
    uint32_t elapsed = 0;
    while (HAL_CAN_GetRxFifoFillLevel(&_hcan, CAN_RX_FIFO0) == 0) {
        if (timeout_ms != HAL_MAX_DELAY && elapsed >= timeout_ms) {
            _mutex.unlock();
            return HAL_TIMEOUT;
        }
        vTaskDelay(pdMS_TO_TICKS(1));
        elapsed++;
    }

    CAN_RxHeaderTypeDef header = {0};
    status = HAL_CAN_GetRxMessage(&_hcan, CAN_RX_FIFO0, &header, data);
    if (status != HAL_OK) {
        OK_ERROR_HANDLER(status, "HAL_CAN_GetRxMessage");
        _mutex.unlock();
        return status;
    }

    if (id) {
        *id = (header.IDE == CAN_ID_EXT) ? header.ExtId : header.StdId;
    }
    if (length) {
        *length = (uint8_t)header.DLC;
    }

    _mutex.unlock();
    return status;
}
