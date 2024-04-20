#pragma once

#include "common.h"

/**
 * @brief One Pole Filter: If alpha is close to 0, the output will change very slowly and smooth out high-frequency variations in the input.
 *
 * @param currInput
 * @param prevInput
 * @param alpha must be between 0.0..1.0.
 * @return T
 */
template <class T>
T filter_one_pole(T currInput, T prevInput, float alpha)
{
    alpha = alpha > 1.0 ? 1.0 : alpha;
    return prevInput + (alpha * (currInput - prevInput));
}