#include "error_handler.h"

/**
 * @brief Error Handler
 * You need to define this function in your application if you want to handle errors
*/ 
__weak HAL_StatusTypeDef error_handler(HAL_StatusTypeDef error)
{
    UNUSED(error);
}