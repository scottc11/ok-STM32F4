#include "ArrayMethods.h"

int arr_max(int arr[], int n)
{
    int i;

    // Initialize maximum element
    int max = arr[0];

    // Traverse array elements from second and
    // compare every element with current max
    for (i = 1; i < n; i++)
        if (arr[i] > max)
            max = arr[i];

    return max;
}

int arr_min(int arr[], int n)
{
    int i;

    // Initialize min element
    int min = arr[0];

    // Traverse array elements from second and
    // compare every element with current min
    for (i = 1; i < n; i++)
        if (arr[i] < min)
            min = arr[i];

    return min;
}

int arr_average(int arr[], int n)
{
    // Find sum of array element
    int sum = 0;
    for (int i = 0; i < n; i++)
        sum += arr[i];

    return sum / n;
}


// Returns element closest to target in arr[] 
int arr_find_closest_int(int arr[], int n, int target) 
{ 
    // Corner cases 
    if (target <= arr[0]) {
        return arr[0];
    }
    if (target >= arr[n - 1]) {
        return arr[n - 1];
    }

    // Doing binary search 
    int i = 0, j = n, mid = 0; 
    while (i < j) { 
        mid = (i + j) / 2; 
  
        if (arr[mid] == target) 
            return arr[mid]; 
  
        /* If target is less than array element, 
            then search in left */
        if (target < arr[mid]) { 
  
            // If target is greater than previous 
            // to mid, return closest of two 
            if (mid > 0 && target > arr[mid - 1]) 
                return getClosestInt(arr[mid - 1], 
                                  arr[mid], target); 
  
            /* Repeat for left half */
            j = mid; 
        } 
  
        // If target is greater than mid 
        else { 
            if (mid < n - 1 && target < arr[mid + 1]) 
                return getClosestInt(arr[mid], 
                                  arr[mid + 1], target); 
            // update i 
            i = mid + 1;  
        } 
    } 
  
    // Only single element left after search 
    return arr[mid]; 
}

// Returns the index of the element closest to target in arr[]
int arr_find_closest_float(float arr[], int n, float target)
{
    // Corner cases
    if (target <= arr[0]) {
        return 0;
    }
        
    if (target >= arr[n - 1]) {
        return n - 1;
    }

    // Doing binary search
    int i = 0, j = n, mid = 0;
    while (i < j)
    {
        mid = (i + j) / 2;

        if (arr[mid] == target)
            return mid;

        /* If target is less than array element, 
            then search in left */
        if (target < arr[mid])
        {

            // If target is greater than previous
            // to mid, return closest of two
            if (mid > 0 && target > arr[mid - 1]) {
                if (getClosestFloat(arr[mid - 1], arr[mid], target) == arr[mid - 1]) {
                    return mid - 1;
                } else {
                    return mid;
                }
            }

            /* Repeat for left half */
            j = mid;
        }

        // If target is greater than mid
        else
        {
            if (mid < n - 1 && target < arr[mid + 1]) {
                if (getClosestFloat(arr[mid], arr[mid + 1], target) == arr[mid]) {
                    return mid;
                } else {
                    return mid + 1;
                }
                
            }
                
            // update i
            i = mid + 1;
        }
    }

    // Only single element left after search
    return arr[mid];
}