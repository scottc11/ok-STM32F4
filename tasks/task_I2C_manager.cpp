#include "task_I2C_manager.h"

TaskHandle_t th_i2c_manager;
QueueHandle_t i2c_queue = nullptr;

uint8_t i2c_tx_busy = 0;
uint8_t i2c_tx_err = 0;

uint8_t i2c_rx_busy = 0;
uint8_t i2c_rx_err = 0;

void task_I2C_manager(void *pvParameters)
{
    th_i2c_manager = xTaskGetCurrentTaskHandle();

    if (!i2c_queue)
    {
        i2c_queue = xQueueCreate(100, sizeof(I2CRequest));
    }

    static I2CRequest current_req;

    vTaskDelay(pdMS_TO_TICKS(100));

    while (1)
    {
        OK_I2C_MANAGER_WHILE_LOOP_START();

        // 1) Drain ALL queued requests this tick (max 25ms)
        while (xQueueReceive(i2c_queue, &current_req, pdMS_TO_TICKS(25)) == pdPASS)
        {
            i2c_tx_err = 0;
            i2c_tx_busy = 1;

            HAL_StatusTypeDef status;
            
            // Start the I2C transaction
            switch (current_req.type)
            {
                case RequestType::Transmit:
                    status = HAL_I2C_Master_Transmit_IT(&current_req.instance->_hi2c, current_req.addr, current_req.buf, current_req.len);
                    break;
                case RequestType::Receive:
                    status = HAL_I2C_Master_Receive_IT(&current_req.instance->_hi2c, current_req.addr, current_req.buf, current_req.len);
                    break;
                default:
                    status = HAL_ERROR;
                    break;
            }

            if (status == HAL_BUSY)
            {
                // peripheral still busy; re-queue and re-try on next tick
                (void)xQueueSendToFront(i2c_queue, &current_req, 0);
                break;
            }
            else if (status != HAL_OK)
            {
                // start failed
                i2c_tx_busy = 0;
                i2c_tx_err = 1;

                if (current_req.waiter)
                {
                    current_req.result = pdFAIL;
                    xTaskNotifyGive(current_req.waiter);
                }
                continue;
            }

            // wait for ISR completion (or timeout)
            if (ulTaskNotifyTake(pdTRUE, pdMS_TO_TICKS(50)) == 0)
            {
                i2c_tx_err = 1;
            }

            // notify waiting task (if any)
            if (current_req.waiter)
            {
                current_req.result = i2c_tx_err ? pdFAIL : pdPASS;
                xTaskNotifyGive(current_req.waiter);
            }
        }
        
        OK_I2C_MANAGER_WHILE_LOOP_END();
    }
}

/* *******************************************************
These HAL callbacks are the bridge between the I2C interrupt and the task_i2c_manager state machine.
They run in ISR context when a transfer finishes (or errors), and they wake task_i2c_manager so it
can stop waiting on that transfer, mark it done/failed, and move on to the next queued request.
*/
/* ******************************************************* */

// Notify the i2c manager task that the i2c transaction has completed
void HAL_I2C_MasterTxCpltCallback(I2C_HandleTypeDef *hi2c)
{
    if (hi2c->Instance == I2C1)
    {
        i2c_tx_busy = 0;
        BaseType_t hpw = pdFALSE;
        vTaskNotifyGiveFromISR(th_i2c_manager, &hpw);
        portYIELD_FROM_ISR(hpw);
    }
}

void HAL_I2C_MasterRxCpltCallback(I2C_HandleTypeDef *hi2c)
{
    if (hi2c->Instance == I2C1)
    {
        i2c_rx_busy = 0;
        BaseType_t hpw = pdFALSE;
        vTaskNotifyGiveFromISR(th_i2c_manager, &hpw);
        portYIELD_FROM_ISR(hpw);
    }
}

void HAL_I2C_ErrorCallback(I2C_HandleTypeDef *hi2c)
{
    if (hi2c->Instance == I2C1)
    {
        i2c_tx_busy = 0;
        i2c_tx_err = 1;
        BaseType_t hpw = pdFALSE;
        // Treat like completion; manager will advance/skip
        vTaskNotifyGiveFromISR(th_i2c_manager, &hpw);
        portYIELD_FROM_ISR(hpw);
    }
}

/**
 * @brief Submit a synchronous I2C transmit request.
 * @note This function will block the task which called it until the request is completed or timed out.
 *
 * @param instance I2C instance
 * @param addr 8-bit address
 * @param buf Pointer to the data to transmit
 * @param len Length of the data to transmit
 * @param timeout Timeout duration
 * @return HAL_StatusTypeDef
 */
HAL_StatusTypeDef i2c_submit_sync_tx(I2C *instance, uint16_t addr, uint8_t *buf, uint16_t len, TickType_t timeout)
{
    I2CRequest req{RequestType::Transmit, instance, addr, buf, len, xTaskGetCurrentTaskHandle(), pdFAIL};
    if (xQueueSend(i2c_queue, &req, pdMS_TO_TICKS(2)) != pdPASS)
    {
        return HAL_BUSY;
    }

    if (ulTaskNotifyTake(pdTRUE, timeout) == 0)
    {
        return HAL_TIMEOUT;
    }

    return (req.result == pdPASS) ? HAL_OK : HAL_ERROR;
}

/**
 * @brief Submit a synchronous I2C receive request.
 * @note This function will block the task which called it until the request is completed or timed out.
 *
 * @param instance I2C instance
 * @param addr 8-bit address
 * @param buf Pointer to the data to receive
 * @param len Length of the data to receive
 * @param timeout Timeout duration
 * @return HAL_StatusTypeDef
 */
HAL_StatusTypeDef i2c_submit_sync_rx(I2C *instance, uint16_t addr, uint8_t *buf, uint16_t len, TickType_t timeout)
{
    I2CRequest req{RequestType::Receive, instance, addr, buf, len, xTaskGetCurrentTaskHandle(), pdFAIL};
    if (xQueueSend(i2c_queue, &req, pdMS_TO_TICKS(50)) != pdPASS)
    {
        return HAL_BUSY;
    }

    if (ulTaskNotifyTake(pdTRUE, timeout) == 0)
    {
        return HAL_TIMEOUT;
    }

    return (req.result == pdPASS) ? HAL_OK : HAL_ERROR;
}

/**
 * @brief Submit an asynchronous I2C request.
 * @note This function will return immediately and the request will be processed in the background.
 *
 * @param req I2C request
 * @return BaseType_t
 */
BaseType_t i2c_submit_async(const I2CRequest &req)
{
    if (i2c_queue == nullptr)
    {
        return pdFAIL;
    }
    return xQueueSend(i2c_queue, &req, pdMS_TO_TICKS(50));
}

/**
 * @brief User code can implement this to do something at the start of the I2C manager while loop
*/
extern "C" __weak void OK_I2C_MANAGER_WHILE_LOOP_START()
{
    // default implementation does nothing
}

/**
 * @brief User code can implement this to do something at the end of the I2C manager while loop
*/
extern "C" __weak void OK_I2C_MANAGER_WHILE_LOOP_END()
{
    // default implementation does nothing
}