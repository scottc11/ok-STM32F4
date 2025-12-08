#pragma once

#include "common.h"
#include "I2C.h"

#define IS31FL3730_ADDR_GND 0xC0 // 8-bit I2C write address (AD = GND)
#define IS31FL3730_ADDR_VCC 0xC6 // 8-bit I2C write address (AD = VCC)
#define IS31FL3730_ADDR_SCL 0xC2 // 8-bit I2C write address (AD = SCL)
#define IS31FL3730_ADDR_SDA 0xC4 // 8-bit I2C write address (AD = SDA)

class IS31FL3730 {
public:
    enum DisplayMode
    {
        MatrixOne = 0b00000000,
        MatrixTwo = 0b00010000,
        MatrixOneAndTwo = 0b00011000
    };

    enum MatrixMode
    {
        _8x8 = 0b00000000,
        _7x9 = 0b00000001,
        _6x10 = 0b00000010,
        _5x11 = 0b00000011
    };

    IS31FL3730(I2C *i2c_ptr, uint8_t addr = IS31FL3730_ADDR_GND)
    {
        address = addr;
        i2c = i2c_ptr;
    }

    uint8_t address;
    I2C *i2c;
    MatrixMode matrix_mode = _8x8;
    DisplayMode display_mode = MatrixOne;

    void init();
    void setConfigRegister(bool software_shutdown, DisplayMode display_mode, bool audio_enable, MatrixMode matrix_mode);
    void setMatrixDataRegister(uint8_t matrix, uint8_t column, uint8_t data);
    void updateMatrix();
    void setLightingEffectRegister(uint8_t ags, uint8_t cs);
    void setPWMRegister(uint8_t pwm);
    void reset();

private:
    // Shadow buffer for all matrix data registers.
    // framebuffer[matrix][column] holds the byte that will be written to the corresponding data register (bits R8..R1).
    uint8_t framebuffer[2][11] = {{0}};
};