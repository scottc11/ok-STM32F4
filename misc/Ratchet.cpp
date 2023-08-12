#include "Ratchet.h"

/**
 * @brief Set the median, minimum, and maximum input values
 * 
 * @param idle
 * @param min 
 * @param max 
 */
void Ratchet::init(uint16_t idle, uint16_t min, uint16_t max)
{
    setRatchetThresholds(idle, min, max);
}

/**
 * @brief Take a 16-bit value and map it to one of the preset Ratchet Rates
 */
uint8_t Ratchet::calculateRatchet(uint16_t input)
{
    if (input > input_idle)
    {
        if (input < ratchetThresholds[3])
        {
            return RATCHET_DIV_1;
        }
        else if (input < ratchetThresholds[2])
        {
            return RATCHET_DIV_2;
        }
        else if (input < ratchetThresholds[1])
        {
            return RATCHET_DIV_4;
        }
        else if (input < ratchetThresholds[0])
        {
            return RATCHET_DIV_8;
        }
    }
    else if (input < input_idle) // input DOWN == Triplets
    {
        if (input > ratchetThresholds[7])
        {
            return RATCHET_DIV_3;
        }
        else if (input > ratchetThresholds[6])
        {
            return RATCHET_DIV_6;
        }
        else if (input > ratchetThresholds[5])
        {
            return RATCHET_DIV_12;
        }
        else if (input > ratchetThresholds[4])
        {
            return RATCHET_DIV_16;
        }
    }
    else
    {
        return 0;
    }
}

/**
 * @brief based on the current position of the sequencer clock, toggle gate on / off
 * @param position sequence position
 * @param input via ADC or whatever
 */
bool Ratchet::handleRatchet(int position, uint16_t input)
{
    prevRatchetRate = currRatchetRate;
    currRatchetRate = calculateRatchet(input);;
    if (currRatchetRate != 0 && position % currRatchetRate == 0)
    {
        if (ratchetCallback)
            ratchetCallback(true); // trigger a ratchet event
    }
    else
    {
        if (prevRatchetRate != 0)
        {
            if (ratchetCallback)
                ratchetCallback(false);
            return false;
        } else {
            return true;
        }
    }
}

void Ratchet::setRatchetThresholds(uint16_t idle, uint16_t min, uint16_t max)
{
    input_idle = idle;
    input_min = min;
    input_max = max;
    ratchetThresholds[3] = idle + ((max - idle) / 4);
    ratchetThresholds[2] = idle + ((max - idle) / 3);
    ratchetThresholds[1] = idle + ((max - idle) / 2);
    ratchetThresholds[0] = idle + (max - idle);
    ratchetThresholds[7] = idle - ((idle - min) / 4);
    ratchetThresholds[6] = idle - ((idle - min) / 3);
    ratchetThresholds[5] = idle - ((idle - min) / 2);
    ratchetThresholds[4] = idle - (idle - min);
}

/**
 * @brief attach a callback function to trigger when a ratchet event occurs
 * 
 * @param func 
 */
void Ratchet::attachCallback(Callback<void(bool state)> func)
{
    ratchetCallback = func;
}