#include "DAC8554.h"

void DAC8554::init()
{
    spi.init();
};

// 485 and 64741
void DAC8554::write(DAC8554::Channel chan, uint16_t value, uint8_t mode /*DAC8554_SINGLE_WRITE*/ )
{
    uint8_t config = mode | (chan << 1);
    this->writeRegister(config, value);
};