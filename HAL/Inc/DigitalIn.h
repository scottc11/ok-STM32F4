#pragma once

#include "common.h"
#include "gpio_api.h"

class DigitalIn {
public:
    GPIO_TypeDef *_port;
    uint32_t _pin;

    DigitalIn(PinName pin, PinMode mode = PullNone)
    {
        gpio_input_init(pin, mode);
    }

    DigitalIn &operator=(int value);
    
    int read();

    void gpio_input_init(PinName pin, PinMode mode);
};