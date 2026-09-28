#pragma once

#include "stm32f4xx_hal.h" // important that this is included first, mainly for arm_math.h
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <arm_math.h>
#include "system_clock_config.h"

#ifdef __cplusplus
extern "C"
{
#endif

void system_init(void);
void system_set_brownout_reset_level(uint8_t level);

#ifdef __cplusplus
}
#endif