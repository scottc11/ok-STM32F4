#include "DigitalOut.h"

DigitalOut *DigitalOut::_instances[NUM_GPIO_INSTANCES] = {0};

/** A shorthand for write()
     * \sa DigitalOut::write()
     * @code
     *      DigitalIn  button(BUTTON1);
     *      DigitalOut led(LED1);
     *      led = button;   // Equivalent to led.write(button.read())
     * @endcode
     */
DigitalOut &DigitalOut::operator= (int value)
{
    write(value);
    return *this;
}

void DigitalOut::initialize() {
    for (int i = 0; i < NUM_GPIO_INSTANCES; i++) {
        if (_instances[i] != NULL) {
            _instances[i]->gpio_init(_instances[i]->_pin, _instances[i]->initState);
        }
    }
}

void DigitalOut::gpio_init(PinName pin, int value)
{
    if (pin == PinName::NC)
    {
        return;
    }
    
    // enable gpio clock
    _port = gpio_enable_clock(pin);

    _pin_num = gpio_get_pin(pin);

    GPIO_InitTypeDef GPIO_InitStruct = {0};

    /*Configure GPIO pin Output Level */
    HAL_GPIO_WritePin(_port, _pin_num, (GPIO_PinState)value);

    /*Configure GPIO pin : PA1 */
    GPIO_InitStruct.Pin = _pin_num;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(_port, &GPIO_InitStruct);
}

void DigitalOut::write(int value) {
    if (_pin != NC) {
        if (value) {
            HAL_GPIO_WritePin(_port, _pin_num, GPIO_PIN_SET);
        }
        else {
            HAL_GPIO_WritePin(_port, _pin_num, GPIO_PIN_RESET);
        }
    }
}

int DigitalOut::read() {
    return HAL_GPIO_ReadPin(_port, _pin_num);
}

void DigitalOut::toggle() {
    this->write(!this->read());
}