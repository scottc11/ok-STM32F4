#ifndef __OK_ALGORITHMS_H
#define __OK_ALGORITHMS_H

#include <stdint.h>
#include <stdlib.h>

int invertIntInRange(int min, int max, int num);

int getClosestInt(int val1, int val2, int target);

float getClosestFloat(float val1, float val2, float target);

uint32_t linear_interpolation(uint32_t targetX, uint32_t x[], uint32_t y[], int num_plots);
float linear_interpolation_f(float targetX, float x[], float y[], int num_plots);

// float findClosestValue(float array[], int size, float target);

template <class T>
T map_num_in_range(T input, T minInput, T maxInput, T minOutput, T maxOutput)
{
    return ((maxOutput - minOutput) * (input - minInput)) / (maxInput - minInput) + minOutput;
}

#endif