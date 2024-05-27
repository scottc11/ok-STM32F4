#include "Logger.h"

void Logger::init() {
    MX_USB_DEVICE_Init();
}

/**
 * @brief Logs a formatted string over USB COM Port
 *
 * @param string max 32 characters!
 *
 * %d or %i: Used to print signed decimal integers.
 * %u: Used to print unsigned decimal integers.
 * %f: Used to print floating-point numbers in decimal notation.
 * %c: Used to print single characters.
 * %s: Used to print strings (null-terminated character arrays).
 * %x: Used to print unsigned hexadecimal integers (lowercase).
 * %X: Used to print unsigned hexadecimal integers (uppercase).
 * %p: Used to print pointers (memory addresses).
 * %e or %E: Used to print floating-point numbers in scientific notation (exponential format).
 * %%: Used to print a literal percent sign (%).
 */
void Logger::log(const char *string, ...)
{
    char buffer[32]; // Buffer to store the formatted string

    va_list args;           // Declare a variable argument list
    va_start(args, string); // Initialize the argument list

    // Call vsnprintf with the provided format string and arguments
    vsnprintf(buffer, sizeof(buffer), string, args);

    va_end(args); // Cleanup the argument list

    CDC_Transmit_FS((uint8_t *)buffer, strlen(buffer));
}

void Logger::log(int number)
{
    this->log("%d\n", number);
}

void Logger::log(float number)
{
    this->log("%.3f\n", number);
}

void Logger::logSystemStatus() {
    this->log("SYSCLK: %d\n", HAL_RCC_GetSysClockFreq());
    HAL_Delay(10);
    this->log("HCLK: %d\n", HAL_RCC_GetHCLKFreq());
    HAL_Delay(10);
    this->log("PCLK1: %d\n", HAL_RCC_GetPCLK1Freq());
    HAL_Delay(10);
    this->log("PCLK2: %d\n", HAL_RCC_GetPCLK2Freq());
    HAL_Delay(10);
}