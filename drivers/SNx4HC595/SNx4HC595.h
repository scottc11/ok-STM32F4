#pragma once

#include "main.h"
#include "SPI.h"
#include "DigitalOut.h"

/**
 * @brief 8-bit shift register
 * 
 * RCLK: refreshes output of the IC
 * 
 */
class SNx4HC595 {
public:
    SNx4HC595(SPI *spi_ptr, PinName latch_pin, PinName enable_pin) : latchPin(latch_pin), enablePin(enable_pin)
    {
        spi = spi_ptr;
    }

    SPI *spi;
    DigitalOut latchPin;
    DigitalOut enablePin;

    void enable() { enablePin.write(0); }
    void disable() { enablePin.write(1); }
    void write(uint8_t data);

};