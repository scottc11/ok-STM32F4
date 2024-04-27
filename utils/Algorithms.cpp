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

/**
 * @brief function to find the closest value in an array of floats to a target float
 * 
 * @param array the array of floats
 * @param size the size of the array
 * @param target the target float
 * @return the closest float in the array to the target
*/
float findClosestValue(float array[], int size, float target)
{
    // Initialize variables to keep track of closest value and its difference
    float closestValue = array[0];
    float minDifference = abs(target - array[0]);

    // Iterate through the array to find the closest value
    for (int i = 1; i < size; ++i)
    {
        // Calculate the absolute difference between the target value and the current array element
        float difference = abs(target - array[i]);

        // Update closestValue and minDifference if the current element is closer to the target
        if (difference < minDifference)
        {
            closestValue = array[i];
            minDifference = difference;
        }
    }

    return closestValue;
}