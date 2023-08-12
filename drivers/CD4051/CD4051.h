#pragma once

#include "common.h"
#include "DigitalOut.h"

class CD4051 {
public:
    CD4051(PinName pin_a, PinName pin_b, PinName pin_c) : pinA(pin_a), pinB(pin_b), pinC(pin_c)
    {

    }

    DigitalOut pinA;
    DigitalOut pinB;
    DigitalOut pinC;

    void activateChannel(int channel);
};