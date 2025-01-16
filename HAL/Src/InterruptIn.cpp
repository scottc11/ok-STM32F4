#include "InterruptIn.h"

InterruptIn *InterruptIn::_instances[NUM_GPIO_IRQ_INSTANCES] = {0};

void InterruptIn::initialize() {
    for (int i = 0; i < NUM_GPIO_IRQ_INSTANCES; i++) {
        if (_instances[i] != NULL) {
            _instances[i]->gpio_irq_init(_instances[i]->_pin);
        }
    }
}

int InterruptIn::read() {
    return (int)gpio_read_pin(_pin);
};

/**
 * @brief set pin as PullUp, PullDown, or PullNone
*/ 
void InterruptIn::mode(PinMode mode) {
    _pull = mode;
    gpio_irq_init(_pin);
}

void InterruptIn::rise(Callback<void()> func)
{
    if (func) {
        riseCallback = func;
    }
    gpio_irq_set(_pin, IRQ_EVENT_RISE, true);
}
void InterruptIn::fall(Callback<void()> func)
{
    if (func)
    {
        fallCallback = func;
    }
    gpio_irq_set(_pin, IRQ_EVENT_FALL, true);
}

void InterruptIn::handleInterupt() {
    int pin_state = this->read();
    if (pin_state) {
        if (riseCallback)
        {
            riseCallback();
        }
    } else {
        if (fallCallback)
        {
            fallCallback();
        }
    }
}

void InterruptIn::gpio_irq_init(PinName pin)
{
    _port = gpio_enable_clock(pin);
    _pin_num = gpio_get_pin(pin);

    // configure gpio for interupt
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = _pin_num;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    set_pin_pull(&GPIO_InitStruct, _pull);
    GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;

    HAL_GPIO_Init(_port, &GPIO_InitStruct);

    /* EXTI interrupt init*/
    _irq = gpio_get_irq_line(_pin);
    HAL_NVIC_SetPriority(_irq, RTOS_ISR_DEFAULT_PRIORITY, 0);
    HAL_NVIC_EnableIRQ(_irq);
}

// note: potentially add a break; once gpio matched
void InterruptIn::RouteCallback(uint16_t GPIO_Pin)
{
    for (auto ins : _instances)
    {
        if (ins && ins->_pin_num == GPIO_Pin)
        {
            ins->handleInterupt();
        }
    }
}

extern "C" void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    InterruptIn::RouteCallback(GPIO_Pin);
}