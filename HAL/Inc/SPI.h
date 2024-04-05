#pragma once

#include "common.h"
#include "DigitalOut.h"
#include "Mutex.h"

class SPI {
public:
    SPI(SPI_TypeDef *_instance, PinName mosi, PinName miso, PinName sclk, int mode, PinName ssel = NC) : _slaveSelect(ssel, 1)
    {
        instance = _instance;
        _mosi = mosi;
        _miso = miso;
        _sclk = sclk;
        _mode = mode;
        _ssel = ssel;
    }

    void init(uint32_t prescaler = SPI_BAUDRATEPRESCALER_32, uint8_t bitOrder = SPI_FIRSTBIT_MSB);

    void write(uint8_t *data, int length);
    void readWrite(uint8_t *data_in, uint8_t *data_out, int length);

    void mode(int mode);

private:
    PinName _mosi;
    PinName _miso;
    PinName _sclk;
    PinName _ssel;

    int _bits; // size of SPI frame
    int _mode; // clock polarity and phase

    DigitalOut _slaveSelect;

    SPI_HandleTypeDef _hspi;
    SPI_TypeDef *instance;
    static Mutex _mutex;
};