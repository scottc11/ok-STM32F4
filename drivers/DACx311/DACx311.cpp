#include "DACx311.h"

void DACx311::init() {
    // spi->init();
}

/**
 * @brief Write a value to the DAC
 *
 * The write sequence begins by bringing the SYNC line low. Data from the DIN line are clocked into the 16-bit shift
 * register on each falling edge of SCLK. The serial clock frequency can be as high as 50 MHz, making
 * the DACx311 compatible with high-speed DSPs. On the 16th falling edge of the serial clock, the last data bit is
 * clocked in and the programmed function is executed.
 * 
 * At this point, the SYNC line can be kept low or brought high. In either case, SYNC must be brought high for
 * a minimum of 20 ns before the next write sequence so that a falling edge of SYNC can initiate the next write
 * sequence.
 *
 * @param value
 */
void DACx311::write(uint16_t value) {
    uint16_t cmd = 0;

    switch (resolution)
    {
    case Resolution::BIT_8:
        value &= 0x00FF; // 8‑bit DAC5311
        cmd = static_cast<uint16_t>(value << 6);
        break;

    case Resolution::BIT_10:
        value &= 0x03FF; // 10‑bit DAC6311
        cmd = static_cast<uint16_t>(value << 4);
        break;

    case Resolution::BIT_12:
        value &= 0x0FFF; // 12‑bit DAC7311
        cmd = static_cast<uint16_t>(value << 2);
        break;
    }

    sync.write(0);

    // PD1/PD0 (bits 15:14) are left at 0 => normal mode

    // Send MSB first, as required by the DACx311
    uint8_t buffer[2];
    buffer[0] = static_cast<uint8_t>((cmd >> 8) & 0xFF); // DB15..DB8
    buffer[1] = static_cast<uint8_t>(cmd & 0xFF);        // DB7..DB0

    spi->write(buffer, 2);

    sync.write(1);
}