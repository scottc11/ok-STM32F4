#pragma once

#include "common.h"
#include "I2C.h"
#include "DigitalOut.h"

// Device-specific constants
#define M24256_I2C_ADDRESS_BASE 0xA0 // 8-bit base address
#define M24256_PAGE_SIZE 64           // Page size in bytes
#define M24256_WRITE_CYCLE_TIME_MS 5   // Max write cycle time

class M24256
{
public:
    M24256(I2C *i2c_ptr, uint8_t addr = M24256_I2C_ADDRESS_BASE, PinName _wc = NC) : writeControlPin(_wc)
    {
        address = addr;
        i2c = i2c_ptr;
    };

    uint8_t address;
    I2C *i2c;
    DigitalOut writeControlPin;
    bool isConnected = false;

    void init();
    void writeByte(uint16_t memAddress, uint8_t data);
    uint8_t readByte(uint16_t memAddress);
    // void writePage(uint16_t address, uint8_t *data, uint8_t length);
    void setWriteControl(bool state);
};