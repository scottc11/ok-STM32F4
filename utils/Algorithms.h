#ifndef __OK_ALGORITHMS_H
#define __OK_ALGORITHMS_H

int invertIntInRange(int min, int max, int num);

int getClosestInt(int val1, int val2, int target);
float getClosestFloat(float val1, float val2, float target);

template <class T>
T map_num_in_range(T input, T minInput, T maxInput, T minOutput, T maxOutput)
{
    return ((maxOutput - minOutput) * (input - minInput)) / (maxInput - minInput) + minOutput;
}

#endif