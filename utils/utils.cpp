#include "utils.h"

uint16_t ok_float_to_u16(float f, float min, float max)
{
    // Scale the floating-point value to fit within the range of uint16_t
    float scaledValue = (f - min) / (max - min) * 65535.0f;

    // Clamp the scaled value to the range of uint16_t
    scaledValue = ok_clamp<float>(scaledValue, 0.0f, 65535.0f);

    // Convert the scaled value to uint16_t
    return (uint16_t)scaledValue;
}