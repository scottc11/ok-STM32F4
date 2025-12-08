#include "IS31FL3730.h"

/**
 * @brief default initialization
 * 
 * @return void
 */
void IS31FL3730::init()
{
    this->reset();
    this->setConfigRegister(false, DisplayMode::MatrixOne, false, MatrixMode::_8x8);
    this->setLightingEffectRegister(0x00, 0x00);
    this->setPWMRegister(10);
}

/**
 * @brief Configure the IS31FL3730
 * 
 * @param software_shutdown Software Shutdown Enable (0: Normal operation, 1: Software shutdown mode)
 * @param display_mode Display Mode (00: Matrix 1 only, 01: Matrix 2 only, 11: Matrix 1 and Matrix 2)
 * @param audio_enable Audio Input Enable (0: Matrix intensity is controlled by the current setting in the Lighting Effect Register (0Dh), 1: Enable audio signal to modulate the intensity of the matrix)
 * @param matrix_mode Matrix Mode Selection (00: 8x8 dot matrix display mode, 01: 7x9 dot matrix display mode, 10: 6x10 dot matrix display mode, 11: 5x11 dot matrix display mode)
 */
void IS31FL3730::setConfigRegister(bool software_shutdown, DisplayMode display_mode, bool audio_enable, MatrixMode matrix_mode) {

    uint8_t cfg = 0;

    // SSD (D7)
    if (software_shutdown) {
        cfg |= (1u << 7);
    }

    // DM (D4:D3)
    cfg |= display_mode;

    // A_EN (D2)
    if (audio_enable) {
        cfg |= (1u << 2);
    }

    // ADM (D1:D0)
    cfg |= matrix_mode;

    // Write to Configuration Register (address 0x00)
    uint8_t buffer[2];
    buffer[0] = 0x00; // Configuration Register address
    buffer[1] = cfg;

    i2c->write(address, buffer, 2);

    // Remember current display and matrix modes so helpers can map (x, y) correctly.
    this->display_mode = display_mode;
    this->matrix_mode = matrix_mode;
}

/**
 * @brief Set the Matrix Data Register
 *
 * The Data Registers (Matrix 1/Matrix 2) store the on or off state of each LED in the Matrix.
 * 11x2 registers are assigned to C1~C11 columns respectively; the LED at a particular (row, column) location will be turned on when the respective data is set to "1".
 * When configured for more than 8 column operation, only the required numbers of LSBs are used in each data register.
 * For example, in 5x11 dot matrix mode, only bits R1 through R5 are used, and bits R6 through R8 are ignored.
 *
 * @param matrix Matrix Number (0: Matrix 1, 1: Matrix 2)
 * @param column Column Number (0: C1, 1: C2, ..., 10: C11)
 * @param data Data Register value (0: LED off, 1: LED on)
 */
void IS31FL3730::setMatrixDataRegister(uint8_t matrix, uint8_t column, uint8_t data) {

    // framebuffer[matrix][col] = matrix_data;

    uint8_t matrix_reg = (matrix == 0 ? 0x01 : 0x0E);

    uint8_t reg = matrix_reg + column;

    uint8_t buffer[2];
    buffer[0] = reg;        // Matrix Data Register address
    buffer[1] = data;
    i2c->write(address, buffer, 2);
}

/**
 * @brief Update the Matrix (Update Column Register)
 *
 * The data sent to the Data Registers will be stored in temporary registers.
 * A write operation of "0000 0000" value to the Update Column Register is
 * required to update the Data Registers (01h~0Bh, 0Eh~18h).
 */
void IS31FL3730::updateMatrix()
{
    uint8_t buffer[2];
    buffer[0] = 0x0C; // Update Column Register address
    buffer[1] = 0x00; // Update Column Register value
    i2c->write(address, buffer, 2);
}

/**
 * @brief Set the PWM Register
 * 
 * The PWM Register can modulate LED light with 128 different items.
 * When the D7 set to “1”, the PWM is the 128 item.
 * When the D7 set to “0”, D6:D0 set the PWM from the 0 item to the 127 item.
 * 
 * @param pwm PWM Register value (0..127)
 */
void IS31FL3730::setPWMRegister(uint8_t pwm) {
    uint8_t buffer[2];
    buffer[0] = 0x19; // PWM Register address
    buffer[1] = pwm > 127 ? 127 : pwm;
    i2c->write(address, buffer, 2);
}

/**
 * @brief Set the Lighting Effect Register
 *
 * The Lighting Effect Register stores the intensity control
 * settings for all of the LEDs in the Matrix.
 *
 * AGS: Audio Input Gain Selection
 * 000 Gain= 0dB
 * 001 Gain= +3dB
 * 010 Gain= +6dB
 * 011 Gain= +9dB
 * 100 Gain= +12dB
 * 101 Gain= +15dB
 * 110 Gain= +18dB
 * 111 Gain= -6dB
 *
 * CS: Full Current Setting for Each Row Output
 * 0000 40mA
 * 0001 45mA
 * 0010 50mA
 * 0011 55mA
 * 0100 60mA
 * 0101 65mA
 * 0110 70mA
 * 0111 75mA
 *
 */
void IS31FL3730::setLightingEffectRegister(uint8_t ags, uint8_t cs)
{
    uint8_t buffer[2];
    buffer[0] = 0x0D; // Lighting Effect Register address
    buffer[1] = (ags << 4) | cs;
    i2c->write(address, buffer, 2);
}

/**
 * @brief Reset the IS31FL3730
 * 
 * Once user writes “0000 0000” to the Reset Register,  IS31FL3730 will reset all registers to default value. On  initial power-up, the IS31FL3730 registers are reset to  their default values for a blank display. 
 * 
 */
void IS31FL3730::reset() {
    uint8_t buffer[2];
    buffer[0] = 0xFF; // Reset Register address
    buffer[1] = 0x00; // Reset Register value
    i2c->write(address, buffer, 2);
}