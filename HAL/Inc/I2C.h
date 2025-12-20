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
        instances[inst] = this;
        _sda_pin = sda;
        _scl_pin = scl;
        _instance = inst;
        _mode = mode;
    };

    void init();
    HAL_StatusTypeDef write(int address, uint8_t *data, int length, bool repeated = false);
    int read(int address, uint8_t *data, int length, bool repeated = false);

    // registry of instances, index 1..3 (0 unused)
    static I2C *instances[4];

private:
    Mutex mutex;
    static I2C_TypeDef *get_i2c_instance(Instance instance);
};

// global i2c instances (to be used in application code)
extern I2C i2c1;
extern I2C i2c2;
extern I2C i2c3;

// weak queue hooks (C linkage so they can be defined in C or C++ and don't get name mangled by compiler)
#ifdef __cplusplus
extern "C"
{
#endif

// Start a queued transmit on the given HAL I2C handle; return when complete or timed out.
HAL_StatusTypeDef OK_I2C_NON_BLOCKING_TRANSMIT(I2C *instance, uint16_t address, uint8_t *buf, uint16_t len, uint32_t timeout_ms);

// Start a queued receive on the given HAL I2C handle; return when complete or timed out.
HAL_StatusTypeDef OK_I2C_NON_BLOCKING_RECEIVE(I2C *instance, uint16_t address, uint8_t *buf, uint16_t len, uint32_t timeout_ms);

#ifdef __cplusplus
}
#endif