#include "Algorithms.h"

/**
 * "Reverses" a number based on a set range of numbers
 * 
 * @param min the lowest possible value num can be
 * @param max the highest possible value num can be
 * @param num number to be reversed
 * @return reversed number
*/
int invertIntInRange(int min, int max, int num)
{
    return (max + min) - num;
}

/**
 * Compares two integers to a target integer and returns the closest. 
 * We find the closest by taking the difference between the target and both values. 
 * NOTE: It assumes that val2 is greater than val1 and target lies between these two.
*/
int getClosestInt(int val1, int val2, int target)
{
    if (target - val1 >= val2 - target)
        return val2;
    else
        return val1;
}

/**
 * Compares two floats to a target float and returns the closest. 
 * We find the closest by taking the difference between the target and both values. 
 * NOTE: It assumes that val2 is greater than val1 and target lies between these two.
*/
float getClosestFloat(float val1, float val2, float target)
{
    if (target - val1 >= val2 - target)
        return val2;
    else
        return val1;
}