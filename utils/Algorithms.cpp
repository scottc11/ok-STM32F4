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
 * @brief Exponential mapping of an input value to an output value
 * @param input input value
 * @param maxInput maximum input value
 * @param maxOutput maximum output value
 * @param growthRate growth rate of the exponential function (adjust for steepness)
 * @return mapped output value
 */
uint16_t exponential_mapping(uint16_t input, uint16_t maxInput, uint16_t maxOutput, float growthRate /*5.0*/)
{
    const uint16_t x_max = maxInput;  // Maximum input value
    const uint16_t y_max = maxOutput; // Maximum output value
    const double k = growthRate;      // Growth rate (adjust for steepness)

    // Normalize x to [0, 1]
    double normalized_x = (double)input / x_max;

    // Apply the exponential formula
    double result = y_max * (1 - exp(-k * normalized_x));

    // Ensure result fits in uint16_t range
    return (uint16_t)result;
}


/**
 * Linearly interpolates the y value for a given x value based on the given data points.
 * 
 * @param targetX the x value for which we want to find the y value
 * @param x the array of x values
 * @param y the array of y values
 * @param num_plots the number of data points
 * @return the interpolated y value
*/
uint32_t linear_interpolation(uint32_t targetX, uint32_t x[], uint32_t y[], int num_plots)
{
    // Find the index of the left neighbor
    int leftIndex = 0;
    while (leftIndex < num_plots && x[leftIndex] < targetX)
    {
        leftIndex++;
    }

    // Check if targetX is out of range
    if (leftIndex == 0 || leftIndex == num_plots)
    {
        return 0; // or handle out of range error
    }

    // Calculate the interpolated y value using linear interpolation formula
    float x0 = x[leftIndex - 1];
    float x1 = x[leftIndex];
    float y0 = y[leftIndex - 1];
    float y1 = y[leftIndex];

    return (uint32_t)(y0 + ((float)targetX - x0) * (y1 - y0) / (x1 - x0));
}

/**
 * @brief Linearly interpolates the y value for a given x value based on the given data points.
 * 
 * @param targetX 
 * @param x 
 * @param y 
 * @param num_plots 
 * @return float 
 */
float linear_interpolation_f(float targetX, float x[], float y[], int num_plots)
{
    // Find the index of the left neighbor
    int leftIndex = 0;
    while (leftIndex < num_plots && x[leftIndex] < targetX)
    {
        leftIndex++;
    }

    // Check if targetX is out of range
    if (leftIndex == 0 || leftIndex == num_plots)
    {
        return 0; // or handle out of range error
    }

    // Calculate the interpolated y value using linear interpolation formula
    float x0 = x[leftIndex - 1];
    float x1 = x[leftIndex];
    float y0 = y[leftIndex - 1];
    float y1 = y[leftIndex];

    return (float)(y0 + ((float)targetX - x0) * (y1 - y0) / (x1 - x0));
}

// /**
//  * @brief function to find the closest value in an array of floats to a target float
//  * 
//  * @param array the array of floats
//  * @param size the size of the array
//  * @param target the target float
//  * @return the closest float in the array to the target
// */
// float findClosestValue(float array[], int size, float target)
// {
//     // Initialize variables to keep track of closest value and its difference
//     float closestValue = array[0];
//     float minDifference = abs(target - array[0]);

//     // Iterate through the array to find the closest value
//     for (int i = 1; i < size; ++i)
//     {
//         // Calculate the absolute difference between the target value and the current array element
//         float difference = abs(target - array[i]);

//         // Update closestValue and minDifference if the current element is closer to the target
//         if (difference < minDifference)
//         {
//             closestValue = array[i];
//             minDifference = difference;
//         }
//     }

//     return closestValue;
// }

/**
 * @brief Remove outliers from a list of data (based on the "Interquartile Range (IQR) Method")
 *
 * @param data
 * @param size
 * @param filteredData
 * @param filteredSize
 */
void ok_remove_outliers(float *data, size_t size, float *filteredData, size_t &filteredSize)
{
    // Allocate memory manually using malloc
    float *sortedData = (float *)malloc(size * sizeof(float));
    if (sortedData == nullptr)
    {
        // Handle memory allocation failure
        filteredSize = 0;
        return;
    }

    std::copy(data, data + size, sortedData);
    std::sort(sortedData, sortedData + size);

    // Calculate Q1 and Q3
    float Q1 = sortedData[size / 4];
    float Q3 = sortedData[3 * size / 4];
    float IQR = Q3 - Q1;

    // Calculate bounds for outliers
    float lowerBound = Q1 - 1.5f * IQR;
    float upperBound = Q3 + 1.5f * IQR;

    // Filter the data
    filteredSize = 0;
    for (size_t i = 0; i < size; ++i)
    {
        if (data[i] >= lowerBound && data[i] <= upperBound)
        {
            filteredData[filteredSize++] = data[i];
        }
    }

    // Free the allocated memory
    free(sortedData);
}