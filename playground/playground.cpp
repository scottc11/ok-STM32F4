#include <stdio.h>
#include <stdint.h>
#include <math.h>

uint16_t floatToUint16(float f, float min, float max)
{
    // Scale the floating-point value to fit within the range of uint16_t
    float scaledValue = (f - min) / (max - min) * 65535.0f;

    // Clamp the scaled value to the range of uint16_t
    if (scaledValue < 0.0f)
    {
        scaledValue = 0.0f;
    }
    else if (scaledValue > 65535.0f)
    {
        scaledValue = 65535.0f;
    }

    // Convert the scaled value to uint16_t
    return (uint16_t)scaledValue;
}

int main()
{
    float sine_wave_f = 0.0f;
    uint16_t sine_wave_u16 = 0;
    float time = 0.0f;
    for (int i = 0; i < 10000; i++)
    {
        sine_wave_f = sin(2 * M_PI * 2.0f * time);
        sine_wave_u16 = floatToUint16(sine_wave_f, -1.0f, 1.0f);
        printf("F: %.10f  u16: %u\n", sine_wave_f, sine_wave_u16); // Print the result
        time += 0.0001f;
    }

    return 0;
}

/*


Clipping and Scaling: If your float values can be outside the range of uint16, you might want to clip them to ensure they fall within the valid range before scaling and converting to uint16.

// Here's an example of how you can clip and scale a float value to fit within the range of uint16:
float floatValue = 123.45f;
float minFloatValue = 0.0f;
float maxFloatValue = 65535.0f; // Maximum value for uint16
float scaleFactor = 1000.0f;    // Adjust based on your precision requirements
uint16_t uintValue;

// Clip the float value to ensure it falls within the valid range
floatValue = fminf(fmaxf(floatValue, minFloatValue), maxFloatValue);

// Scale and convert to uint16
uintValue = (uint16_t)(floatValue * scaleFactor);

*/