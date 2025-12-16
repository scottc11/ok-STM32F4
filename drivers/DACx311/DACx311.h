#pragma once

#include "common.h"
#include "SPI.h"

/**
 * @brief The 8-bit DAC5311, 10-bit DAC6311, and 12-bit DAC7311 devices (DACx311) are low-power, single-channel,
 * voltage output DACs. These devices are monotonic by design, provide excellent linearity, and minimize
 * undesired code-to-code transient voltages while offering an easy upgrade path within a pin-compatible family. All
 * devices use a versatile, three-wire serial interface that operates at clock rates of up to 50 MHz and is compatible
 * with standard SPI, QSPI, Microwire, and digital signal processor (DSP) interfaces.
 *
 * @note The DACx311 has a 3-wire serial interface (SYNC, SCLK, and DIN) compatible with SPI, QSPI, and Microwire
 * interface standards, as well as most DSPs. For an example of a typical write sequence, see Figure 7-1.
 *
 * @note The DAC samples data on the falling edge of SCLK, so the SPI instance you pass in must be configured 
 * equivalent to SPI mode 1 (CPOL = 0, CPHA = 1 / sample on 2nd edge, MSB‑first)
 */
class DACx311 {
public:

    enum class Resolution {
        BIT_8 = 0,
        BIT_10 = 1,
        BIT_12 = 2,
    };

    DACx311(SPI *_spi, PinName _sync, Resolution _resolution) : sync(_sync, 1)
    {
        spi = _spi;
        resolution = _resolution;
    }

    SPI *spi;
    DigitalOut sync;
    Resolution resolution;


    void init();
    void write(uint16_t value);
};