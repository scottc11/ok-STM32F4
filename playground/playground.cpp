#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <iostream>
#include <fstream>

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
    std::ofstream outFile("output.csv", std::ios::out | std::ios::trunc); // Open file for writing, truncate if it exists
    if (!outFile)
    {
        std::cerr << "Error opening file for writing" << std::endl;
        return 1;
    }

    uint16_t adcValue = 1000;
    uint16_t adcMax = 4095;
    uint16_t adcMin = 0;

    uint16_t maxSteps = 2000; // Maximum number of dac updates it can take to reach the target value
    // how many dac updates it takes to reach the target value
    uint16_t steps = (static_cast<float>(adcValue) / adcMax) * maxSteps; // this should be exponential!

    const int numPoints = 4096; // Number of points in the curve
    const float exponentBase = 10.0f; // Base of the exponential function
    const float maxValue = 5000.0f; // Maximum value for scaling

    for (int i = 0; i < numPoints; ++i) {
        float normalizedValue = static_cast<float>(i) / (numPoints - 1); // Normalize i to [0, 1]
        float exponentialValue = pow(exponentBase, normalizedValue) - 1; // Exponential function
        uint16_t scaledValue = static_cast<uint16_t>((exponentialValue / (exponentBase - 1)) * maxValue); // Scale to maxValue

        outFile << i << "," << scaledValue << "\n"; // Write exponential value to file
    }


    // float currentValue = 2000.0f;   // Starting value
    // float targetValue = 28000.0f; // Target value

    // // Calculate the growth factor (for exponential interpolation)
    // float growthFactor = pow(targetValue / currentValue, 1.0f / steps);
    
    // // Calculate the increment for each step (for linear interpolation)
    // float increment = (targetValue - currentValue) / steps;

    // printf("Steps: %u\n", steps); // Print the result
    // printf("Growth Factor: %f\n", growthFactor); // Print the result
    // printf("Increment: %f\n", increment); // Print the result

    // for (int i = 0; i < steps; i++)
    // {
    //     currentValue += increment;
    //     outFile << i << "," << static_cast<uint16_t>(currentValue) << "\n"; // Write exponential value to file
    // }

    outFile.close(); // Close the file
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

// float sine_wave_f = 0.0f;
// uint16_t sine_wave_u16 = 0;
// float time = 0.0f;
// for (int i = 0; i < 10000; i++)
// {
//     sine_wave_f = sin(2 * M_PI * 2.0f * time);
//     sine_wave_u16 = floatToUint16(sine_wave_f, -1.0f, 1.0f);
//     outFile << sine_wave_f << "," << sine_wave_u16 << "\n"; // Write the result to the file
//     time += 0.0001f;
// }