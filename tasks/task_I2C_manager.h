#pragma once

#include "common.h"
#include "I2C.h"

enum class RequestType : uint8_t
{
    Transmit,
    Receive
};

struct I2CRequest
{
    RequestType type;
    I2C *instance; // wrapper instance (e.g., &i2c1, &i2c2, &i2c3)
    uint16_t addr; // 8-bit address device address
    uint8_t *buf;
    uint16_t len;
    TaskHandle_t waiter; // task to notify on completion; nullptr => fire-and-forget
    BaseType_t result;   // pdPASS/pdFAIL filled by manager
};

struct I2CBusState
{
    I2C *instance;          // &i2c1, &i2c2, &i2c3
    QueueHandle_t queue;    // one queue per bus
    I2CRequest current_req; // in‑flight request on this bus
    uint8_t busy;
    uint8_t err;
};

static I2CBusState buses[] __attribute__((unused)) = {
    {&i2c1, nullptr, {}, 0, 0},
    {&i2c2, nullptr, {}, 0, 0},
    {&i2c3, nullptr, {}, 0, 0},
};

// expose the task handle to the application code
extern TaskHandle_t th_i2c_manager;

// I2C manager task
void task_I2C_manager(void *pvParameters);

// Synchronous helpers (enqueue + wait for completion)
HAL_StatusTypeDef i2c_submit_sync_tx(I2C *instance, uint16_t addr, uint8_t *buf, uint16_t len, TickType_t timeout);
HAL_StatusTypeDef i2c_submit_sync_rx(I2C *instance, uint16_t addr, uint8_t *buf, uint16_t len, TickType_t timeout);

// Asynchronous helper (enqueue only)
BaseType_t i2c_submit_async(const I2CRequest &req);

// Weak hooks (C linkage so they can be defined in C or C++ and don't get name mangled by compiler)
#ifdef __cplusplus
extern "C"
{
#endif

void OK_I2C_MANAGER_WHILE_LOOP_START();
void OK_I2C_MANAGER_WHILE_LOOP_END();

#ifdef __cplusplus
}
#endif