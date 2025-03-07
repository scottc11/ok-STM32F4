#pragma once

#include <stdint.h>

template <typename T>
constexpr T ok_min(T in, T min)
{
    return in < min ? in : min;
}

template <typename T>
constexpr T ok_max(T in, T max)
{
    return in > max ? in : max;
}

template <typename T>
constexpr T ok_clamp(T x, T min, T max)
{
    return x < min ? min : (x > max ? max : x);
}

/**
 * @brief map a number from one range to another
 *
 * @param input
 * @param minInput
 * @param maxInput
 * @param minOutput
 * @param maxOutput
 * @return T
 */
template <class T>
T ok_map(T input, T minInput, T maxInput, T minOutput, T maxOutput)
{
    return ((maxOutput - minOutput) * (input - minInput)) / (maxInput - minInput) + minOutput;
}

/**
 * @brief invert a number
 *
 * @param input number to invert
 * @param min input range min
 * @param max input range max
 * @return T
 */
template <class T>
T ok_invert(T input, T min, T max)
{
    return max - (input - min);
}

template <typename T>
T ok_arr_average(T arr[], int n)
{
    // Find sum of array element
    T sum = 0;
    for (int i = 0; i < n; i++)
        sum += arr[i];

    return sum / n;
}

uint16_t ok_float_to_u16(float f, float min, float max);

uint16_t ok_voltage_to_data(float targetVoltage, float minVoltage, float maxVoltage, uint16_t resolution);