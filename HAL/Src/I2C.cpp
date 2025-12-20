#include "I2C.h"

// registry of instances, index 1..3 (0 unused)
I2C *I2C::instances[4] = {nullptr, nullptr, nullptr, nullptr};

void I2C::init()
{
    mutex.lock();
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    HAL_StatusTypeDef status;

    // peripheral / I2C instance dependant configurations
    if (_instance == I2C_1) {
        GPIO_InitStruct.Alternate = GPIO_AF4_I2C1;
    }
    else if (_instance == I2C_2) {
        GPIO_InitStruct.Alternate = GPIO_AF4_I2C2;
    }
    else {
        GPIO_InitStruct.Alternate = GPIO_AF4_I2C3;
    }

    // enable SCL and SDA pins
    GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;

    GPIO_InitStruct.Pin = gpio_get_pin(_scl_pin);
    GPIO_TypeDef *port = gpio_enable_clock(_scl_pin);
    HAL_GPIO_Init(port, &GPIO_InitStruct);
    
    GPIO_InitStruct.Pin = gpio_get_pin(_sda_pin);
    port = gpio_enable_clock(_sda_pin);
    HAL_GPIO_Init(port, &GPIO_InitStruct);

    /* I2C3 interrupt Init */
    // HAL_NVIC_SetPriority(I2C3_EV_IRQn, 0, 0);
    // HAL_NVIC_EnableIRQ(I2C3_EV_IRQn);

    switch (_instance)
    {
    case I2C_1:
        __HAL_RCC_I2C1_CLK_ENABLE(); /* Peripheral clock enable */
        _hi2c.Instance = I2C1;
        break;
    case I2C_2:
        __HAL_RCC_I2C2_CLK_ENABLE();
        _hi2c.Instance = I2C2;
        break;
    case I2C_3:
        __HAL_RCC_I2C3_CLK_ENABLE();
        _hi2c.Instance = I2C3;
        break;
    }

    // I2C Init
    _hi2c.Init.ClockSpeed = 400000;
    _hi2c.Init.DutyCycle = I2C_DUTYCYCLE_2;
    _hi2c.Init.OwnAddress1 = 0;
    _hi2c.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
    _hi2c.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
    _hi2c.Init.OwnAddress2 = 0;
    _hi2c.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
    _hi2c.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
    status = HAL_I2C_Init(&_hi2c);
    if (status != HAL_OK) {
        logger_log_err("I2C->init", status);
    }

    if (_mode == NonBlocking) {
        switch (_instance)
        {
        case I2C_1:
            HAL_NVIC_SetPriority(I2C1_EV_IRQn, RTOS_ISR_DEFAULT_PRIORITY, 0);
            HAL_NVIC_EnableIRQ(I2C1_EV_IRQn);
            HAL_NVIC_SetPriority(I2C1_ER_IRQn, RTOS_ISR_DEFAULT_PRIORITY, 0);
            HAL_NVIC_EnableIRQ(I2C1_ER_IRQn);
            break;
        case I2C_2:
            HAL_NVIC_SetPriority(I2C2_EV_IRQn, RTOS_ISR_DEFAULT_PRIORITY, 0);
            HAL_NVIC_EnableIRQ(I2C2_EV_IRQn);
            HAL_NVIC_SetPriority(I2C2_ER_IRQn, RTOS_ISR_DEFAULT_PRIORITY, 0);
            HAL_NVIC_EnableIRQ(I2C2_ER_IRQn);
            break;
        case I2C_3:
            HAL_NVIC_SetPriority(I2C3_EV_IRQn, RTOS_ISR_DEFAULT_PRIORITY, 0);
            HAL_NVIC_EnableIRQ(I2C3_EV_IRQn);
            HAL_NVIC_SetPriority(I2C3_ER_IRQn, RTOS_ISR_DEFAULT_PRIORITY, 0);
            HAL_NVIC_EnableIRQ(I2C3_ER_IRQn);
            break;
        default:
            break;
        }
    }

    mutex.unlock();
}

HAL_StatusTypeDef I2C::write(int address, uint8_t *data, int length, bool repeated /*=false*/)
{
    // if (_mode == NonBlocking)
    // {
    //     return OK_I2C_NON_BLOCKING_TRANSMIT(&_hi2c, (uint16_t)address, data, (uint16_t)length, 50);
    // }

    mutex.lock();
    HAL_StatusTypeDef status;
    while (HAL_I2C_GetState(&_hi2c) != HAL_I2C_STATE_READY)
    {
        vTaskDelay(pdMS_TO_TICKS(3));
    };

    status = HAL_I2C_Master_Transmit(&_hi2c, address, data, length, HAL_MAX_DELAY);
    if (status != HAL_OK)
    {
        logger_log_err("I2C->write", status);
    }
    mutex.unlock();
    return status;
}

int I2C::read(int address, uint8_t *data, int length, bool repeated /*=false*/)
{

    // if (_mode == NonBlocking)
    // {
    //     return OK_I2C_NON_BLOCKING_RECEIVE(&_hi2c, (uint16_t)address, data, (uint16_t)length, 50);
    // }

    mutex.lock();
    HAL_StatusTypeDef status;
    
    while (HAL_I2C_GetState(&_hi2c) != HAL_I2C_STATE_READY)
    {
        vTaskDelay(pdMS_TO_TICKS(3));
    };

    status = HAL_I2C_Master_Receive(&_hi2c, address, data, length, HAL_MAX_DELAY);
    if (status != HAL_OK) {
        logger_log_err("I2C->read", status);
    }
    mutex.unlock();
    return status;
}

I2C_TypeDef *I2C::get_i2c_instance(Instance instance)
{
    switch (instance)
    {
        case Instance::I2C_1:
            return I2C1;
        case Instance::I2C_2:
            return I2C2;
        case Instance::I2C_3:
            return I2C3;
    }
}

/**
 * @brief Handle non-blocking I2C transmit
 * @note This function should not be modified, when the callback is needed, OK_I2C_NON_BLOCKING_TRANSMIT should be implemented in the user file
 *
 * @param hi2c I2C handle
 * @param address 8-bit device address
 * @param buf Pointer to the data to transmit
 * @param len Length of the data to transmit
 * @param timeout_ms how long to wait for transaction to complete
 * @return HAL_StatusTypeDef
 */
extern "C" __weak HAL_StatusTypeDef OK_I2C_NON_BLOCKING_TRANSMIT(I2C *instance, uint16_t address, uint8_t *buf, uint16_t len, uint32_t timeout_ms)
{
    UNUSED(instance);
    UNUSED(address);
    UNUSED(buf);
    UNUSED(len);
    UNUSED(timeout_ms);
    return HAL_ERROR;
}

/**
 * @brief Handle non-blocking I2C receive
 * @note This function should not be modified, when the callback is needed, OK_I2C_NON_BLOCKING_RECEIVE should be implemented in the user file
 *
 * @param hi2c I2C handle
 * @param address 8-bit device address
 * @param buf Pointer to the data to receive
 * @param len Length of the data to receive
 * @param timeout_ms how long to wait for transaction to complete
 * @return HAL_StatusTypeDef
 */
extern "C" __weak HAL_StatusTypeDef OK_I2C_NON_BLOCKING_RECEIVE(I2C *instance, uint16_t address, uint8_t *buf, uint16_t len, uint32_t timeout_ms)
{
    UNUSED(instance);
    UNUSED(address);
    UNUSED(buf);
    UNUSED(len);
    UNUSED(timeout_ms);
    return HAL_ERROR;
}

extern "C" void I2C1_EV_IRQHandler(void)
{
    I2C *i2c = I2C::instances[I2C::Instance::I2C_1];
    if (i2c) {
        HAL_I2C_EV_IRQHandler(&i2c->_hi2c);
    }
}

extern "C" void I2C1_ER_IRQHandler(void)
{
    I2C *i2c = I2C::instances[I2C::Instance::I2C_1];
    if (i2c) {
        HAL_I2C_ER_IRQHandler(&i2c->_hi2c);
    }
}

extern "C" void I2C2_EV_IRQHandler(void)
{
    I2C *i2c = I2C::instances[I2C::Instance::I2C_2];
    if (i2c) {
        HAL_I2C_EV_IRQHandler(&i2c->_hi2c);
    }
}

extern "C" void I2C2_ER_IRQHandler(void)
{
    I2C *i2c = I2C::instances[I2C::Instance::I2C_2];
    if (i2c) {
        HAL_I2C_ER_IRQHandler(&i2c->_hi2c);
    }
}

extern "C" void I2C3_EV_IRQHandler(void)
{
    I2C *i2c = I2C::instances[I2C::Instance::I2C_3];
    if (i2c) {
        HAL_I2C_EV_IRQHandler(&i2c->_hi2c);
    }
}

extern "C" void I2C3_ER_IRQHandler(void)
{
    I2C *i2c = I2C::instances[I2C::Instance::I2C_3];
    if (i2c) {
        HAL_I2C_ER_IRQHandler(&i2c->_hi2c);
    }
}