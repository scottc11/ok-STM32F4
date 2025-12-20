#pragma once

#include "stm32f4xx_hal.h"

#ifdef __cplusplus
extern "C"
{
#endif

HAL_StatusTypeDef OK_ERROR_HANDLER(HAL_StatusTypeDef error, const char *msg);

#ifdef __cplusplus
}
#endif