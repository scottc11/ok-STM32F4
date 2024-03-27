#include "AnalogOut.h"

/**
 * @brief Initialize DAC peripheral
 */
void AnalogOut::init() {
    __HAL_RCC_DAC_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();

    hdac.Instance = DAC;
    HAL_DAC_Init(&hdac);

    GPIO_InitTypeDef GPIO_InitStruct = {0};
    
    if (channel == DAC_CHANNEL_1) {
        GPIO_InitStruct.Pin = GPIO_PIN_4;
    } else {
        GPIO_InitStruct.Pin = GPIO_PIN_5;
    }
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    DAC_ChannelConfTypeDef sConfig = {0};
    sConfig.DAC_Trigger = DAC_TRIGGER_NONE;
    sConfig.DAC_OutputBuffer = DAC_OUTPUTBUFFER_ENABLE;
    HAL_DAC_ConfigChannel(&hdac, &sConfig, channel);
    HAL_DAC_Start(&hdac, channel);
}

void AnalogOut::write(uint16_t value) {
    HAL_DAC_SetValue(&hdac, channel, DAC_ALIGN_12B_R, value);
}