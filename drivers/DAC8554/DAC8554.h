#ifndef __DAC8554_H
#define __DAC8554_H

/**
 * Pin 9  --> Select
 * Pin 10 --> SCLK
 * Pin 11 --> MOSI
*/

#include "SPI.h"

#define DAC85X4_BUFFER_WRITE  0b00000000
#define DAC85X4_SINGLE_WRITE  0b00010000
#define DAC85X4_ALL_WRITE     0b00100000
#define DAC85X4_BROADCAST     0b00110000
#define DAC85X4_SET_INTERNAL_REFERENCE 0b00000001
#define DAC85X4_INTERNAL_REFERENCE_ENABLED 0x0
#define DAC85X4_INTERNAL_REFERENCE_DISABLED 0x2000

class DAC8554 {

public:
  
  enum Channel {
    CHAN_A = 0x0,
    CHAN_B = 0x1,
    CHAN_C = 0x2,
    CHAN_D = 0x3,
  };
  
  SPI *spi;
  DigitalOut selectPin;
  int baseline = 485;
  int ceiling = 64741;

  /**
   * NOTE: Texas Instruments requires special SPI formatting, most commonly known as SPI Mode #1
   * NOTE: Certain STM32 SPI periferals have limited bus speeds
  */
  DAC8554(SPI *_spi, PinName _selectPin) : selectPin(_selectPin, 1)
  {
    spi = _spi;
  }
  
  void init();
  void write(DAC8554::Channel chan, uint16_t value, uint8_t mode = DAC85X4_SINGLE_WRITE);
  void setInternalVoltageReference(bool enabled);
};

#endif