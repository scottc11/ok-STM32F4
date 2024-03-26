#pragma once

#include "common.h"
#include "DigitalOut.h"

class CD4051 {
public:
    CD4051(PinName pin_a, PinName pin_b, PinName pin_c, PinName pin_inhibit = PinName::NC) : pinA(pin_a), pinB(pin_b), pinC(pin_c), inhibitPin(pin_inhibit, 0)
    {
        // constructor
    }
    
    uint8_t activeChannel = 0;

    void inhibit(bool state);
    void activateChannel(int channel);
    
private:
    DigitalOut pinA;
    DigitalOut pinB;
    DigitalOut pinC;
    DigitalOut inhibitPin;
};