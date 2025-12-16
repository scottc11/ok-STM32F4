#include "DAC8554.h"

void DAC8554::init()
{
    setInternalVoltageReference(true);
};

/**
 * @brief Write a value to the DAC
 *
 * 8 MSBs are used as control bits and the 16 LSBs are used as data
 * the DAC8554 requires its data with the MSB as the first bit received
 * byte1 --> | A1 | A0 | LD1 | LD0 | X | DAC Select 1 | DAC Select 0 | PD0 |
 * byte2 --> | D15 | D14 | D13 | D12 | D11 | D10 | D9 | D8 |
 * byte3 --> | D7  | D6  | D5  | D4  | D3  | D2  | D1 | D0 |
 */
void DAC8554::write(DAC8554::Channel chan, uint16_t value, uint8_t mode /*DAC8554_SINGLE_WRITE*/ )
{
    uint8_t config = mode | (chan << 1);

    uint8_t byte1 = config;
    uint8_t byte2 = (value >> 8) & 0xFF;
    uint8_t byte3 = value & 0xFF;
    uint8_t buffer[3] = {byte1, byte2, byte3};
    selectPin.write(0);
    spi->write(buffer, 3);
    selectPin.write(1);
};

/**
 * @brief enable or disable the internal voltage reference
 * 
 * @param enabled 
 */
void DAC8554::setInternalVoltageReference(bool enabled)
{
    uint8_t config = DAC85X4_SET_INTERNAL_REFERENCE;
    uint8_t data = enabled ? DAC85X4_INTERNAL_REFERENCE_ENABLED : DAC85X4_INTERNAL_REFERENCE_DISABLED;

    uint8_t byte1 = config;
    uint8_t byte2 = (data >> 8) & 0xFF;
    uint8_t byte3 = data & 0xFF;
    uint8_t buffer[3] = {byte1, byte2, byte3};
    spi->write(buffer, 3);
}