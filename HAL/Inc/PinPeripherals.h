#pragma once

#include "PinNames.h"

typedef enum
{
    SPI_1 = (int)SPI1_BASE,
    SPI_2 = (int)SPI2_BASE,
    SPI_3 = (int)SPI3_BASE,
} SPIName;

typedef enum
{
    TIM_1 = (int)TIM1_BASE,
    TIM_2 = (int)TIM2_BASE,
    TIM_3 = (int)TIM3_BASE,
    TIM_4 = (int)TIM4_BASE,
    TIM_5 = (int)TIM5_BASE,
    TIM_6 = (int)TIM6_BASE,
    TIM_7 = (int)TIM7_BASE,
    TIM_8 = (int)TIM8_BASE,
    TIM_9 = (int)TIM9_BASE,
    TIM_10 = (int)TIM10_BASE,
    TIM_11 = (int)TIM11_BASE,
    TIM_12 = (int)TIM12_BASE,
    TIM_13 = (int)TIM13_BASE,
    TIM_14 = (int)TIM14_BASE,
} TIMName;

typedef struct
{
    PinName pin;
    int peripheral;
    int function;
} PinMap;

extern const PinMap PinMap_SPI_MOSI[];
extern const PinMap PinMap_SPI_MISO[];
extern const PinMap PinMap_SPI_SCLK[];
extern const PinMap PinMap_SPI_SSEL[];
extern const PinMap PinMap_TIM[];