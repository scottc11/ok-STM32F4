#include "utils.h"

/**
 * @brief convert a floating-point value to a uint16_t value
 *
 * @note 👀 ok-STM32F4/CMSIS/DSP/Source/SupportFunctions/arm_float_to_q15.c
 *
 * @param f
 * @param min
 * @param max
 * @return uint16_t
 */
uint16_t ok_float_to_u16(float f, float min, float max)
{
    // Scale the floating-point value to fit within the range of uint16_t
    float scaledValue = (f - min) / (max - min) * 65535.0f;

    // Clamp the scaled value to the range of uint16_t
    scaledValue = ok_clamp<float>(scaledValue, 0.0f, 65535.0f);

    // Convert the scaled value to uint16_t
    return (uint16_t)scaledValue;
}

/**
 * @brief convert a target voltage to a DAC value
 *
 * @param targetVoltage target voltage to output on DAC
 * @param minVoltage minimum value the DAC can output
 * @param maxVoltage maximum value the DAC can output
 * @param resolution DAC resolution ex. 4096, 65536
 * @return uint16_t
 */
uint16_t ok_voltage_to_data(float targetVoltage, float minVoltage, float maxVoltage, uint16_t resolution)
{
    float totalVoltageRange = maxVoltage - minVoltage;
    float voltageStepPerDacValue = totalVoltageRange / (resolution - 1);
    uint16_t dacValue = (targetVoltage - minVoltage) / voltageStepPerDacValue;
    return dacValue;
}