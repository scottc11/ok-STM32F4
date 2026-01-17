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
    setThresholds(idle, min, max);
}

/**
 * @brief Take a 16-bit value and map it to one of the preset Ratchet Rates
 */
uint8_t Ratchet::calculateRatchet(uint16_t input)
{
    if (input == input_idle) {
        return 0;
    } else {
        for (int i = 0; i < num_thresholds; i++)
        {
            if (input < thresholds[i].threshold)
            {
                return thresholds[i].rate;
            }
        }
    }
    return 0;
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
    return false;
}

/**
 * @brief this calculation is off. The thresholds should be evenly spaced.
 * to achive that, determine how many thresholds you want between idle position and max position
 * then divide the range by that number. Then add that number to the idle position to get the next threshold
 * ie. (65535 / 2) / 4 = 8192.5, so 8192.5 + 8192.5 = 16384. Then 16384 + 8192.5 = 24576, etc.
 * 
 * also, shouldn't there be 9 thresholds? 4 above idle, 4 below idle, and idle itself?
 * 
 * @param idle
 * @param min
 * @param max
 */
void Ratchet::setThresholds(uint16_t idle, uint16_t min, uint16_t max)
{
    input_idle = idle;
    input_min = min;
    input_max = max;
    num_thresholds = 9;

    uint16_t threshold = max / (num_thresholds - 1);

    int buffer = 2000;
    thresholds[0].threshold = (threshold * 0) + buffer;
    thresholds[0].rate = RATCHET_DIV_16;
    thresholds[1].threshold = (threshold * 1);
    thresholds[1].rate = RATCHET_DIV_8;
    thresholds[2].threshold = (threshold * 2);
    thresholds[2].rate = RATCHET_DIV_4;
    thresholds[3].threshold = (threshold * 3);
    thresholds[3].rate = RATCHET_DIV_2;
    thresholds[4].threshold = idle;
    thresholds[4].rate = RATCHET_DIV_1;
    thresholds[5].threshold = (threshold * 5);
    thresholds[5].rate = RATCHET_DIV_3;
    thresholds[6].threshold = (threshold * 6);
    thresholds[6].rate = RATCHET_DIV_6;
    thresholds[7].threshold = (threshold * 7);
    thresholds[7].rate = RATCHET_DIV_12;
    thresholds[8].threshold = (threshold * 8) - buffer;
    thresholds[8].rate = RATCHET_DIV_24;
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