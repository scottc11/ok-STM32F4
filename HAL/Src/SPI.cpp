#include "SPI.h"

Mutex SPI::_mutex;

void SPI::init()
{
    _mutex.lock();
    
    /* Peripheral clock enable */
    if (this->instance == SPI1) {
        __HAL_RCC_SPI1_CLK_ENABLE();    
    }
    else if (this->instance == SPI2) {
        __HAL_RCC_SPI2_CLK_ENABLE();
    }
    else {
        __HAL_RCC_SPI3_CLK_ENABLE();
    }

    /** SPI2 GPIO Configuration */
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    gpio_enable_clock(_mosi);
    GPIO_InitStruct.Pin = gpio_get_pin(_mosi);
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    
    // handle alternate pin func per instance
    if (this->instance == SPI1) {
        GPIO_InitStruct.Alternate = GPIO_AF5_SPI1;
    } else if (this->instance == SPI2) {
        GPIO_InitStruct.Alternate = GPIO_AF5_SPI2;
    } else {
        GPIO_InitStruct.Alternate = GPIO_AF5_SPI3;
    }
    HAL_GPIO_Init(gpio_get_port(_mosi), &GPIO_InitStruct);

    gpio_enable_clock(_sclk);
    GPIO_InitStruct.Pin = gpio_get_pin(_sclk);
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    HAL_GPIO_Init(gpio_get_port(_sclk), &GPIO_InitStruct);

    /* SPI2 parameter configuration*/
    _hspi.Instance = SPI2;
    _hspi.Init.Mode = SPI_MODE_MASTER;
    _hspi.Init.Direction = SPI_DIRECTION_2LINES;
    _hspi.Init.DataSize = SPI_DATASIZE_8BIT;
    _hspi.Init.NSS = SPI_NSS_SOFT;
    _hspi.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_32;
    _hspi.Init.FirstBit = SPI_FIRSTBIT_MSB;
    _hspi.Init.TIMode = SPI_TIMODE_DISABLE;
    _hspi.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
    _hspi.Init.CRCPolynomial = 10;
    
    this->mode(_mode);
    
    HAL_StatusTypeDef status;
    status = HAL_SPI_Init(&_hspi);
    error_handler(status);
    _mutex.unlock();
}

/**
 * @brief master and slaves must agree on the clock polarity and phase with respect to the data exchanged over MOSI and MISO lines
 * The combinations of polarity and phase are often referred to as SPI bus modes
 * The most common modes are mode 0 and mode 3, but the majority of slave devices support at least a couple of bus modes.
 * @param mode
 */
void SPI::mode(int mode)
{
    switch (mode)
    {
    case 0:
        _hspi.Init.CLKPolarity = SPI_POLARITY_LOW;
        _hspi.Init.CLKPhase = SPI_PHASE_1EDGE;
        break;
    case 1:
        _hspi.Init.CLKPolarity = SPI_POLARITY_LOW;
        _hspi.Init.CLKPhase = SPI_PHASE_2EDGE;
        break;
    case 2:
        _hspi.Init.CLKPolarity = SPI_POLARITY_HIGH;
        _hspi.Init.CLKPhase = SPI_PHASE_1EDGE;
        break;
    default:
        _hspi.Init.CLKPolarity = SPI_POLARITY_HIGH;
        _hspi.Init.CLKPhase = SPI_PHASE_2EDGE;
        break;
    }
}

void SPI::write(uint8_t *data, int length)
{
    _mutex.lock();
    HAL_StatusTypeDef status;
    _slaveSelect.write(0);
    status = HAL_SPI_Transmit(&_hspi, (uint8_t *)data, length, HAL_MAX_DELAY);
    if (status != HAL_OK) {
        error_handler(status);
    }
    _slaveSelect.write(1);
    _mutex.unlock();
}