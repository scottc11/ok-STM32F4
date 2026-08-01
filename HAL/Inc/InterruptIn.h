#pragma once

#include "common.h"
#include "gpio_api.h"
#include "Callback.h"

class InterruptIn {
public:

    PinName _pin;
    PinMode _pull;
    GPIO_TypeDef *_port;
    uint32_t _pin_num;
    PinEvent _event;
    IRQn_Type _irq;
    Callback<void()> riseCallback;
    Callback<void()> fallCallback;

    InterruptIn(PinName pin, PinMode mode = PullNone, PinEvent event = Rising)
    {
        _pin = pin;
        _pull = mode;
        _event = event;
        
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

    static void initialize();
    int read();
    void handleInterrupt();
    void mode(PinMode mode);
    void rise(Callback<void()> func);
    void fall(Callback<void()> func);
    void enable();
    void disable();
    void gpio_irq_init(PinName pin, PinEvent event = RisingFalling);
    void gpio_irq_deinit();
    static void RouteCallback(uint16_t GPIO_Pin);

private:
    static InterruptIn *_instances[NUM_GPIO_IRQ_INSTANCES];
};