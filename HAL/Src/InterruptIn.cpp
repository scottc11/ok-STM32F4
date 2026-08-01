#include "InterruptIn.h"

InterruptIn *InterruptIn::_instances[NUM_GPIO_IRQ_INSTANCES] = {0};

void InterruptIn::initialize() {
    for (int i = 0; i < NUM_GPIO_IRQ_INSTANCES; i++) {
        if (_instances[i] != NULL) {
            _instances[i]->gpio_irq_init(_instances[i]->_pin, _instances[i]->_event);
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
    gpio_irq_init(_pin, _event);
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

// NOTE: dispatches by reading the pin level, not by which edge fired. 
void InterruptIn::handleInterrupt()
{
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
// TODO: you need an option select Rising, Falling, or Both. No need to tax the interrupt system with an un-needed interrupt.
void InterruptIn::gpio_irq_init(PinName pin, PinEvent event)
{
    _port = gpio_enable_clock(pin);
    _pin_num = gpio_get_pin(pin);

    // configure gpio for interupt
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = _pin_num;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    set_pin_pull(&GPIO_InitStruct, _pull);
    GPIO_InitStruct.Mode = event;

    HAL_GPIO_Init(_port, &GPIO_InitStruct);

    /* EXTI interrupt init*/
    _irq = gpio_get_irq_line(_pin);
    HAL_NVIC_SetPriority(_irq, RTOS_ISR_DEFAULT_PRIORITY, 0);
    HAL_NVIC_EnableIRQ(_irq);
}

/**
 * @brief de-initialize the gpio interrupt, releasing the EXTI line and
 *        resetting the pin config to its default (analog) state.
 *
 * The NVIC IRQ is intentionally left enabled: EXTI lines 5-9 and 10-15 share
 * IRQ vectors, so disabling the vector could break other pins in the group.
 * HAL_GPIO_DeInit unmasks this pin's EXTI line, which is sufficient.
 */
void InterruptIn::gpio_irq_deinit()
{
    HAL_GPIO_DeInit(_port, _pin_num);
    __HAL_GPIO_EXTI_CLEAR_IT(_pin_num);
}

/**
 * @brief (re)enable the interrupt by unmasking this pin's EXTI line.
 *
 * Masking is done per-line (EXTI->IMR) rather than at the NVIC, since EXTI
 * lines 5-9 and 10-15 share IRQ vectors and disabling the vector would
 * affect other pins. Any edge latched while disabled is cleared first so
 * the callback doesn't fire spuriously on enable.
 */
void InterruptIn::enable()
{
    __HAL_GPIO_EXTI_CLEAR_IT(_pin_num);
    EXTI->IMR |= _pin_num;
}

/**
 * @brief disable the interrupt by masking this pin's EXTI line, keeping the
 *        gpio configured and other pins on the shared IRQ vector unaffected.
 */
void InterruptIn::disable()
{
    EXTI->IMR &= ~_pin_num;
}

// note: potentially add a break; once gpio matched
void InterruptIn::RouteCallback(uint16_t GPIO_Pin)
{
    for (auto ins : _instances)
    {
        if (ins && ins->_pin_num == GPIO_Pin)
        {
            ins->handleInterrupt();
        }
    }
}

extern "C" void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    InterruptIn::RouteCallback(GPIO_Pin);
}