#pragma once

#include "common.h"
#include "I2C.h"
#include "BitwiseMethods.h"

#define IS31FL3246_ADDR_GND 0x60 // 8-bit addr
#define IS31FL3246_ADDR_VCC 0x66 // 8-bit addr
#define IS31FL3246_ADDR_SCL 0x62 // 8-bit addr
#define IS31FL3246_ADDR_SDA 0x64 // 8-bit addr

class IS31FL3246 {
public:
    
    enum PWM_Frequency
    {
        _32kHz = 0x00,
        _64kHz = 0x1,
        _128kHz = 0x2
    };

    IS31FL3246(I2C *i2c_ptr, uint8_t addr = IS31FL3246_ADDR_GND)
    {
        address = addr;
        i2c = i2c_ptr;
    };
    
    uint8_t address;
    I2C *i2c;

    void init();
    void setControlRegister(bool shutdown, bool bit_mode, uint8_t freq, bool RGB_mode);
    void setGlobalCurrent(uint8_t red, uint8_t green, uint8_t blue);
    void setPhaseDelayAndClockPhase();
    void setChannelPWM(int channel, uint8_t pwm, bool update=true);
    void setChannelMode(int channel, uint8_t mode);
    void reset();


private:
    void writeRegister(char reg, char _data1, char _data2)
    {
        uint8_t commands[3];
        commands[0] = reg;
        commands[1] = _data1;
        commands[2] = _data2;

        i2c->write(address, commands, 3);
    }

    void writeRegister(char reg, char _data1)
    {
        uint8_t commands[2];
        commands[0] = reg;
        commands[1] = _data1;

        i2c->write(address, commands, 2);
    }

    enum Registers
    {
        CONTROL_REG = 0x0,
        HF_PWM_REG = 0x01, // High Frequency PWM Duty
        LF_PWM_REG = 0x49,
        UPDATE_REG = 0x6D,
        CURR_G_REG = 0x6E,
        CURR_R_REG = 0x6F,
        CURR_B_REG = 0x70,
        PHASE_REG = 0x71,
        RESET_REG = 0x7F
    };
};