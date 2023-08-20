#include "DAC8554.h"

void DAC8554::init()
{
    spi.init();
    setInternalVoltageReference(true);
};

// 485 and 64741
void DAC8554::write(DAC8554::Channel chan, uint16_t value, uint8_t mode /*DAC8554_SINGLE_WRITE*/ )
{
    uint8_t config = mode | (chan << 1);
    this->writeRegister(config, value);
};

void DAC8554::setInternalVoltageReference(bool enabled) {
    uint16_t data = enabled ? DAC85X4_INTERNAL_REFERENCE_ENABLED : DAC85X4_INTERNAL_REFERENCE_DISABLED;
    this->writeRegister(DAC85X4_SET_INTERNAL_REFERENCE, data);
}