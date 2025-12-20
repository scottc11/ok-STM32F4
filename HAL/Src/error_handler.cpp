#include "error_handler.h"

/**
 * @brief Generic error handler for all HAL functions
 * You need to define this function in your application if you want to handle errors
*/
extern "C" __weak HAL_StatusTypeDef OK_ERROR_HANDLER(HAL_StatusTypeDef error, const char *msg)
{
    UNUSED(error);
    UNUSED(msg);
    return error;
}