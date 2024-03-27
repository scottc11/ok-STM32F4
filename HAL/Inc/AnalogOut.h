#pragma once

#include "main.h"

class AnalogOut {
public:

    /**
     * @brief Construct a new Analog Out object
     *
     * @param _channel one of DAC_CHANNEL_1 or DAC_CHANNEL_2
     */
    AnalogOut(int _channel) {
        channel = _channel;
    }
    
    int channel;
    DAC_HandleTypeDef hdac;
    
    void init();
    void write(uint16_t value);
};