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

uint16_t ok_float_to_u16(float f, float min, float max);