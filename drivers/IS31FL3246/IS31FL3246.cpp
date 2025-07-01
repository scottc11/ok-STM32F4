#include "IS31FL3246.h"


void IS31FL3246::init()
{
    reset();
    setControlRegister(false, false, PWM_Frequency::_128kHz, false);
    setGlobalCurrent(255, 255, 255); // max current
    for (int i = 0; i < IS31FL3246_CHANNEL_COUNT - 1; i++)
    {
        pwmFrame1[i] = 0;
        pwmFrame2[i] = 0;
        setChannelMode(i, 0b00000100);
    }
}

/**
 * @brief The Control Register sets software shutdown mode, pulse width modulated (PWM) high/low frequency and PWM resolution.
 *
 * @param shutdown SSD Software Shutdown Enable: 0 Software shutdown mode, 1 Normal operation
 * @param bit_mode PMS High PWM freqyency Resolution: 1 = 10-bit mode, 0 = 8-bit mode
 * @param freq     HFPS High Frequency PWM Select: 00 32kHz, 01 64kHz, 1x 128kHz
 * @param RGB_mode RGBM RGB Register Mode Select: 1 = 12 RGB Mode, 0 = 36 Channel Mode
 */
void IS31FL3246::setControlRegister(bool shutdown, bool bit_mode, uint8_t freq, bool RGB_mode)
{
    uint8_t config = 0;

    config = bitwise_write_bit(config, 0, 1);
    config = bitwise_write_bit(config, 1, bit_mode);
    
    // Shift the two bits of the "bits" value to the left
    // by 4 positions to match the positions of bits 4 and 5
    uint8_t bits = (freq & 0x03) << 4;

    // Clear bits 4 and 5 of the "value" parameter
    config &= ~(0x03 << 4);

    // Set bits 4 and 5 of the "value" parameter to the
    // two bits of the "bits" parameter
    config |= bits;

    config = bitwise_write_bit(config, 6, RGB_mode);

    writeRegister(Registers::CONTROL_REG, config);

    uint8_t connected = readRegister(Registers::CONTROL_REG);
    if (connected == config)
    {
        isConnected = true;
    }
}

/**
 * @brief The Global Current Control Register modulates all channels DC current which is noted as IOUT in 256 steps.
 *
 * @param red 
 * @param green
 * @param blue
 */
void IS31FL3246::setGlobalCurrent(uint8_t red, uint8_t green, uint8_t blue)
{
    writeRegister(Registers::CURR_R_REG, red);
    writeRegister(Registers::CURR_G_REG, green);
    writeRegister(Registers::CURR_B_REG, blue);
}

/**
 * @brief High Frequency PWM Low Byte Duty Value (0x00~0xFF)
 * 
 * @param channel number from 1 to 36 corresponding to the LED channel
 *
 */
void IS31FL3246::setChannelPWM(int channel, uint8_t pwm, uint8_t frame /* 1 */, bool update /* = true*/)
{
    if (frame == 1) {
        pwmFrame1[channel] = pwm;
    } else {
        pwmFrame2[channel] = pwm;
    }

    if (frame == currentFrame) {
        uint8_t pwm_reg = 2 * channel - 1;
        writeRegister(pwm_reg, pwm);
        if (update)
        {
            // When SDB = “H” and SSD = “1”, a write of “0000 0000” to 6Dh is to update the PWM Register (01h~6Ch) values.
            writeRegister(Registers::UPDATE_REG, 0x00);
        }
    }
}

/**
 * @brief Set the current frame to be drawn
 * 
 * @param frame 
 */
void IS31FL3246::setFrame(uint8_t frame)
{
    currentFrame = frame;
}

void IS31FL3246::drawFrame(uint8_t frame)
{
    setFrame(frame);
    if (frame == 1)
    {
        for (int i = 0; i < IS31FL3246_CHANNEL_COUNT - 1; i++)
        {
            setChannelPWM(i, pwmFrame1[i], frame);
        }
    } else
    {
        for (int i = 0; i < IS31FL3246_CHANNEL_COUNT - 1; i++)
        {
            setChannelPWM(i, pwmFrame2[i], frame);
        }
    }
}

void IS31FL3246::clearFrame(uint8_t frame)
{
    if (frame == 1)
    {
        for (int i = 0; i < IS31FL3246_CHANNEL_COUNT - 1; i++)
        {
            pwmFrame1[i] = 0;
        }
    } else
    {
        for (int i = 0; i < IS31FL3246_CHANNEL_COUNT - 1; i++)
        {
            pwmFrame2[i] = 0;
        }
    }
}

/**
 * @brief Bits 3 and 4 (D3:D2) are the only ones worth setting here. They are the "PWM Frequency Mode Select" bits (FMS)
 * They determine the state of each individual LED channel.
 * 
 * They can be set to 1 of 4 values:
 * HFP + LFP
 * Only HFP, LFP=256
 * DC Mode, no PWM, output always on
 * Channel Shutdown mode
 *
 * @param channel
 * @param mode
 */
void IS31FL3246::setChannelMode(int channel, uint8_t mode)
{
    uint8_t mode_reg = 2 * channel;
    writeRegister(mode_reg, mode);
}

/**
 * @brief A write of “0000 0000” to 7Fh will reset all registers to their default values.
 *
 */
void IS31FL3246::reset()
{
    writeRegister(RESET_REG, 0x00);
}

/**
 * @brief Phase Delay feature and Clock Phase options can work together to minimize the voltage ripple of LED power supply. Check Phase Delay and Clock Phase section for more information
 *
 */
void IS31FL3246::setPhaseDelayAndClockPhase()
{
    // do something
}