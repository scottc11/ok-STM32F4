#pragma once

#include "common.h"

/*
Advanced control:
TIM1, TIM8 :: 16-bit

General purpose:
TIM2, TIM5 :: 32-bit
TIM3, TIM4, TIM9, TIM10, TIM11 TIM12, TIM13, TIM14 :: 16-bit

Basic:
TIM6, TIM7 :: 16-bit
*/

uint32_t tim_get_APBx_freq(TIM_HandleTypeDef *htim);

uint32_t tim_get_overflow_freq(TIM_HandleTypeDef *htim);

void tim_set_overflow_freq(TIM_HandleTypeDef *htim, uint32_t targetFrequency, uint32_t maxTimerResolution);

uint8_t tim_get_capture_prescaler(TIM_HandleTypeDef *htim, uint32_t channel);

float tim_calculate_capture_frequency(TIM_HandleTypeDef *htim, uint32_t channel);

uint32_t tim_get_capture_period(TIM_HandleTypeDef *htim, uint32_t current, uint32_t previous);

void tim_enable(TIM_TypeDef *instance);

void OK_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim);
void OK_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim);