#pragma once

#include "common.h"
#include "Metronome.h"
#include "HardwareTimer.h"

class Metronome; // forward declaration
class HardwareTimer; // forward declaration

uint32_t tim_get_APBx_freq(TIM_HandleTypeDef *htim);

uint32_t tim_get_overflow_freq(TIM_HandleTypeDef *htim);

void tim_set_overflow_freq(TIM_HandleTypeDef *htim, uint32_t target_freq);

void tim_enable(TIM_TypeDef *instance);

void OK_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim);
void OK_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim);