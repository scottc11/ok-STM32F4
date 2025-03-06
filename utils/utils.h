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