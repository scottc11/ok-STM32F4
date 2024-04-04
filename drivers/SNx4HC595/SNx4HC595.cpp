#include "SNx4HC595.h"

void SNx4HC595::write(uint8_t data) {
    latchPin.write(0);
    uint8_t buffer[1] = {data};
    spi->write(buffer, 1);
    latchPin.write(1);
}