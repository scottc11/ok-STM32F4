#pragma once

#include "common.h"
#include "CAN.h"

/**
 * @brief A single CAN frame passed between the application and the CAN manager task.
 */
struct CANMessage
{
    uint32_t id;         // 11-bit standard or 29-bit extended identifier
    uint8_t  data[8];    // payload (max 8 bytes)
    uint8_t  len;        // number of valid payload bytes (0..8)
    bool     extended;   // true => 29-bit extended id, false => 11-bit standard id
};

// Depth of the RX/TX queues (in frames). Sized generously for bursty MIDI traffic.
#ifndef CAN_RX_QUEUE_DEPTH
#define CAN_RX_QUEUE_DEPTH 64
#endif

#ifndef CAN_TX_QUEUE_DEPTH
#define CAN_TX_QUEUE_DEPTH 64
#endif

// Task handle + queues exposed to application code.
extern TaskHandle_t th_can_manager;
extern QueueHandle_t can_rx_queue;
extern QueueHandle_t can_tx_queue;

/**
 * @brief Interrupt-driven CAN manager task.
 *
 * RX is fully interrupt-driven: the RX FIFO 0 "message pending" ISR drains every
 * pending frame into can_rx_queue, so nothing is lost to polling gaps. TX frames
 * queued by the application (can_tx_queue) are drained and transmitted here.
 *
 * @param pvParameters pointer to an already-constructed CAN instance (ex. &can_bus).
 *                     The application must call CAN::init() on that instance; the task
 *                     waits until the peripheral is ready before enabling interrupts.
 */
void task_CAN_manager(void *pvParameters);

/**
 * @brief Queue a frame for transmission (asynchronous, returns immediately).
 * @param id message identifier
 * @param data pointer to payload (copied into the queued message)
 * @param len number of payload bytes (clamped to 8)
 * @param extended true for a 29-bit extended id, false for an 11-bit standard id
 * @param timeout how long to wait for space in the TX queue
 * @return pdPASS if queued, pdFAIL / errQUEUE_FULL otherwise
 */
BaseType_t can_manager_send(uint32_t id, uint8_t *data, uint8_t len, bool extended = false, TickType_t timeout = pdMS_TO_TICKS(10));

/**
 * @brief Queue a pre-built frame for transmission (asynchronous, returns immediately).
 */
BaseType_t can_manager_send(const CANMessage &msg, TickType_t timeout = pdMS_TO_TICKS(10));

/**
 * @brief Block until a frame is received from the bus (or the timeout elapses).
 * @param msg [out] destination for the received frame
 * @param timeout how long to wait for a frame (default: block forever)
 * @return pdPASS if a frame was dequeued, pdFALSE on timeout
 */
BaseType_t can_manager_receive(CANMessage *msg, TickType_t timeout = portMAX_DELAY);
