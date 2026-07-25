#pragma once

#include "common.h"
#include "gpio_api.h"
#include "Mutex.h"

class CAN {
public:
    /**
     * @brief Construct a CAN bus driver bound to a bxCAN peripheral instance.
     * @param _instance bxCAN peripheral (ex. CAN1, CAN2)
     * @param rx CAN RX pin
     * @param tx CAN TX pin
     */
    CAN(CAN_TypeDef *_instance, PinName rx, PinName tx)
    {
        instance = _instance;
        _rx = rx;
        _tx = tx;
    }

    /**
     * @brief Initialize the CAN peripheral, install an accept-all filter and start the bus.
     * @param prescaler baudrate prescaler. Defaults to 6 which yields 500 kbit/s with APB1 @ 45 MHz.
     * @param mode operating mode (CAN_MODE_NORMAL, CAN_MODE_LOOPBACK, CAN_MODE_SILENT, CAN_MODE_SILENT_LOOPBACK)
     */
    void init(uint32_t prescaler = 6, uint32_t mode = CAN_MODE_NORMAL);

    /**
     * @brief Transmit a data frame on the bus (blocks until a free mailbox is available).
     * @param id message identifier
     * @param data pointer to payload (up to 8 bytes)
     * @param length number of payload bytes (0..8)
     * @param extended true for a 29-bit extended identifier, false for an 11-bit standard identifier
     * @param timeout_ms how long to wait for a free transmit mailbox before giving up
     * @return HAL_OK on success, HAL_TIMEOUT if no mailbox freed in time
     */
    HAL_StatusTypeDef transmit(uint32_t id, uint8_t *data, uint8_t length, bool extended = false, uint32_t timeout_ms = 100);

    /**
     * @brief Receive a data frame from RX FIFO 0 (blocks until a frame arrives or timeout elapses).
     * @param id [out] received message identifier
     * @param data [out] buffer for payload (must hold at least 8 bytes)
     * @param length [out] number of payload bytes received
     * @param timeout_ms how long to wait for a frame
     * @return HAL_OK on success, HAL_TIMEOUT if no frame arrived in time
     */
    HAL_StatusTypeDef receive(uint32_t *id, uint8_t *data, uint8_t *length, uint32_t timeout_ms = HAL_MAX_DELAY);

    CAN_HandleTypeDef _hcan;

private:
    PinName _rx;
    PinName _tx;

    CAN_TypeDef *instance;
    static Mutex _mutex;
};
