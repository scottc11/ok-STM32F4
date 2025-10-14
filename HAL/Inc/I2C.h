#pragma once

#include "common.h"
#include "uart_logger.h"
#include "Mutex.h"
#include "gpio_api.h"

class I2C {
public:
    enum Instance
    {
        I2C_1 = 1,
        I2C_2 = 2,
        I2C_3 = 3
    };

    enum Mode
    {
        Blocking = 0,
        NonBlocking = 1,
        NonBlockingDMA = 2
    };

    enum Frequency
    {
        NormalMode = 100000,
        FastMode = 4000000
    };

    PinName _sda_pin;
    PinName _scl_pin;
    I2C_HandleTypeDef _hi2c;
    Instance _instance;
    Mode _mode;

    I2C(PinName sda, PinName scl, Instance inst, Mode mode = Blocking) {
        _sda_pin = sda;
        _scl_pin = scl;
        _instance = inst;
        _mode = mode;
    };

    void init();
    HAL_StatusTypeDef write(int address, uint8_t *data, int length, bool repeated = false);
    int read(int address, uint8_t *data, int length, bool repeated = false);

private:

    Mutex mutex;

    static I2C_TypeDef *get_i2c_instance(Instance instance);
};

// weak queue hooks (C linkage so they can be defined in C or C++ and don't get name mangled by compiler)
#ifdef __cplusplus
extern "C"
{
#endif

// Start a queued transmit on the given HAL I2C handle; return when complete or timed out.
HAL_StatusTypeDef OK_I2C_NON_BLOCKING_TRANSMIT(I2C_HandleTypeDef *hi2c, uint16_t address, uint8_t *buf, uint16_t len, uint32_t timeout_ms);

// Start a queued receive on the given HAL I2C handle; return when complete or timed out.
HAL_StatusTypeDef OK_I2C_NON_BLOCKING_RECEIVE(I2C_HandleTypeDef *hi2c, uint16_t address, uint8_t *buf, uint16_t len, uint32_t timeout_ms);

#ifdef __cplusplus
}
#endif