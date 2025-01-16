#pragma once

#include "common.h"

class DigitalOut {
public:

    GPIO_TypeDef *_port;
    PinName _pin;
    uint32_t _pin_num;
    uint8_t initState;
    
    DigitalOut(PinName pin, int value = 0)
    {
        _pin = pin;
        initState = value;

        if (_pin != NC)
        {
            for (int i = 0; i < NUM_GPIO_IRQ_INSTANCES; i++)
            {
                if (_instances[i] == NULL)
                {
                    _instances[i] = this;
                    break;
                }
            }
        }
    }

    DigitalOut &operator=(int value);

    void write(int value);
    int read();
    void toggle();

    static void initialize();
    void gpio_init(PinName pin, int value);

private:
    static DigitalOut *_instances[NUM_GPIO_INSTANCES];
};