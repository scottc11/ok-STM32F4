
#include "SX1509.h"

void SX1509::init(bool extClock /*false*/) {

  this->reset();
  this->setClockConfig(ClockSource::INTERNAL, OSCIO::INPUT, fOSCOUT::DIVISION_OF_REG_CLOCK);
  this->setMiscConfig(DriverMode::LINEAR, ClockSpeed::EXTRA_SLOW);
  this->setDebounceTime(3); // configure a common debounce time
}

/**
 * @brief set the clock configuration (internal, external)
 * 
 * Oscillator frequency (fOSC) source
 * 00 : OFF. LED driver, keypad engine and debounce features are disabled.
 * 01 : External clock input (OSCIN)
 * 10 : Internal 2MHz oscillator
 * 11 : Reserved
 *
 * OSCIO pin function (Cf. §4.7)
 * 0 : OSCIO is an input (OSCIN)
 * 1 : OSCIO is an output (OSCOUT)
 *
 * Frequency of the signal output on OSCOUT pin:
 * 0x0 : 0Hz, permanent "0" logical level (GPO)
 * 0xF : 0Hz, permanent "1" logical level (GPO)
 * Else : fOSCOUT = fOSC/(2^(RegClock[3:0]-1))
 */
void SX1509::setClockConfig(ClockSource clockSource, OSCIO oscPin, fOSCOUT oscState)
{
  _clockConfig = static_cast<uint8_t>(clockSource) | static_cast<uint8_t>(oscPin) | static_cast<uint8_t>(oscState);
  this->i2cWrite(REG_CLOCK, _clockConfig);
}

/**
 * @brief 
 * BITS 👇
 * 7 LED Driver mode for Bank B ‘s fading capable IOs (IO15-12)
    0: Linear
    1: Logarithmic

 * 6:4 Frequency of the LED Driver clock ClkX of all IOs:
    0 : OFF. LED driver functionality is disabled for all IOs.
    Else : ClkX = fOSC/(2^(RegMisc[6:4]-1))

 * 3 LED Driver mode for Bank A ‘s fading capable IOs (IO7-4)
    0: Linear
    1: Logarithmic

 * 2 NRESET pin function when externally forced low (Cf. §4.3.1 and §4.8.5)
    0: Equivalent to POR
    1: Reset PWM/Blink/Fade counters (not user programmed values)
    This bit is can only be reset manually or by POR, not by NRESET.

 * 1 Auto-increment register address (Cf. §4.4)
    0: ON. When several consecutive data are read/written, register address is incremented.
    1: OFF. When several consecutive data are read/written, register address is kept fixed.
 
 * 0 Autoclear NINT on RegData read (Cf. §4.6)
    0: ON. RegInterruptSourceA/B is also automatically cleared when RegDataA/B is read.
    1: OFF. RegInterruptSourceA/B must be manually cleared, either directly or via RegEventStatusA/B.

 * @param driverMode
 * @param clockSpeed
 */
void SX1509::setMiscConfig(DriverMode driverMode, ClockSpeed clockSpeed)
{
  _miscConfig = static_cast<uint8_t>(driverMode) | static_cast<uint8_t>(clockSpeed);
  _miscConfig |= 0b00000100;               // NRESET pin function
  this->i2cWrite(REG_MISC, _miscConfig);   // very important for LED Driver mode that this config gets set
}

bool SX1509::isConnected() {
  volatile uint8_t temp = this->i2cRead(REG_MISC);
  if (temp == _miscConfig) {
    return true;
  } else {
    return false;
  }
}

void SX1509::pinMode(int pin, PinMode mode, bool invertPolarity /*false*/) {
  switch (mode)
  {
    case PinMode::INPUT:
      setDirection(pin, 1);
      enablePullup(pin);
      setInputDebounce(pin, true);
      if (invertPolarity) { setPolarity(pin, invertPolarity); }
      break;
    case PinMode::OUTPUT:
      setDirection(pin, 0);
      break;
    case PinMode::ANALOG_OUTPUT:
      ledConfig(pin);
      break;
  }
}

/**
 * Enables polarity inversion for each IO
 * 0 : Normal polarity
 * 1 : Inverted polarity
 */
void SX1509::setPolarity(int pin, int polarity) {
  int bank = getBank(pin);
  int pinPos = getPinPos(pin);
  int reg = static_cast<uint8_t>(REG_POLARITY_B) + bank;
  this->i2cWrite(reg, bitwise_write_bit(this->i2cRead(reg), pinPos, polarity));
}

/**
 * An interrupt NINT can be generated on any programmed combination of I/Os rising and/or falling edges through the RegInterruptMask and RegSense registers.
 * If needed, the I/Os which triggered the interrupt can then be identified by reading RegInterruptSource register.
 * 
 * 0 : An event on this IO will trigger an interrupt
 * 1 : An event on this IO will NOT trigger an interrupt
 * 
 * We enable interrupt on I/O[1] in RegInterruptMask
 * We set edge sense for I/O[1] in RegSense
 * 
 * 00 : None
 * 01 : Rising
 * 10 : Falling
 * 11 : Both
*/

void SX1509::setInterupt(int pin, bool willNotInterupt, InteruptDirection type)
{
  int hotValue;
  int mask;
  int bank = getBank(pin);
  int pinPos = getPinPos(pin);
  int reg = static_cast<uint8_t>(REG_INTERRUPT_MASK_B) + bank;
  this->i2cWrite(reg, bitwise_write_bit(this->i2cRead(reg), pinPos, willNotInterupt));

  // Set Sense
  if (pin > 7) {              // Bank B IOs
    if (pinPos > 3) {         // Bank B pins 4:7
      reg = REG_SENSE_HIGH_B;
      hotValue = this->i2cRead(reg);
      mask = 0b00000011 << (pinPos - 4) * 2; // shift by twos
      hotValue &= ~mask;
      hotValue |= static_cast<uint8_t>(type) << (pinPos - 4) * 2;
      this->i2cWrite(reg, hotValue);
    } else {                  // Bank B pins 4:7
      reg = REG_SENSE_LOW_B;
      hotValue = this->i2cRead(reg);
      mask = 0b00000011 << pinPos * 2; // shift by twos
      hotValue &= ~mask;
      hotValue |= static_cast<uint8_t>(type) << pinPos * 2;
      this->i2cWrite(reg, hotValue);
    }
  } else {
    if (pinPos > 3) { // Bank A pins 4:7
      reg = REG_SENSE_HIGH_A;
      hotValue = this->i2cRead(reg);
      mask = 0b00000011 << (pinPos - 4) * 2; // shift by twos
      hotValue &= ~mask;
      hotValue |= static_cast<uint8_t>(type) << (pinPos - 4) * 2;
      this->i2cWrite(reg, hotValue);
    }
    else { // Bank A pins 4:7
      reg = REG_SENSE_LOW_A;
      hotValue = this->i2cRead(reg);
      mask = 0b00000011 << pinPos * 2; // shift by twos
      hotValue &= ~mask;
      hotValue |= static_cast<uint8_t>(type) << pinPos * 2;
      this->i2cWrite(reg, hotValue);
    }
  }
}

void SX1509::enableInterupt(int pin, InteruptDirection type)
{
  this->setInterupt(pin, false, type);
}

void SX1509::disableInterupt(int pin)
{
  this->setInterupt(pin, true, InteruptDirection::NONE);
}



/** 
 * Interrupt source (from IOs set in RegInterruptMask)
    0 : No interrupt has been triggered by this IO
    1 : An interrupt has been triggered by this IO (an event as configured in relevant RegSense register occured).

 * Writing '1' clears the bit in RegInterruptSource and in RegEventStatus When all bits are cleared, NINT signal goes back high.
*/
int SX1509::getInteruptSource(Bank bank)
{
  return this->i2cRead(REG_INTERRUPT_SOURCE_B + static_cast<uint8_t>(bank));
}

/** 
 * Each input can be individually debounced by setting corresponding bits in RegDebounce register.
 * At power up the debounce function is disabled. After enabling the debouncer, the change of the input value 
 * is accepted only if the input value is identical at two consecutive sampling times.
*/
void SX1509::setInputDebounce(int pin, bool debounce)
{
  int bank = getBank(pin);
  int pinPos = getPinPos(pin);
  int reg = REG_DEBOUNCE_ENABLE_B + bank;
  int hotValue = this->i2cRead(reg);
  this->i2cWrite(reg, bitwise_write_bit(hotValue, pinPos, debounce));
  hotValue = this->i2cRead(reg);
}

/**
 * The debounce time common to all IOs can be set in RegDebounceConfig register from 0.5 to 64ms (fOSC = 2MHz).

  0: 0.5ms x 2MHz/fOSC
  1: 1ms x 2MHz/fOSC
  2: 2ms x 2MHz/fOSC 
  3: 4ms x 2MHz/fOSC
  4: 8ms x 2MHz/fOSC
  5: 16ms x 2MHz/fOSC
  6: 32ms x 2MHz/fOSC
  7: 64ms x 2MHz/fOSC
*/
void SX1509::setDebounceTime(int value) {
  this->i2cWrite(REG_DEBOUNCE_CONFIG, value);
}

/** Software Reset
 * Writing consecutively 0x12 and 0x34 to RegReset register will reset all registers to their default values.
*/
void SX1509::reset() {
  this->i2cWrite(REG_RESET, 0x12);
  this->i2cWrite(REG_RESET, 0x34);
}

/** @brief LED Driver Configuration

Please note that in this configuration the IO must be programmed as open drain output (RegOpenDrain)
with no pull-up (RegPullUp) and input buffer must be disabled (RegInputBufferDisable).

Below are the steps required to use the LED driver with the typical LED connection described §6.2:

- Disable input buffer (RegInputDisable)
- Disable pull-up (RegPullUp)
- Enable open drain (RegOpenDrain)
- Set direction to output (RegDir) – by default RegData is set high => LED OFF
- Enable oscillator (RegClock)
- Configure LED driver clock and mode if relevant (RegMisc)
- Enable LED driver operation (RegLEDDriverEnable)
- Configure LED driver parameters (RegTOn, RegIOn, RegOff, RegTRise, RegTFall)
- Set RegData bit low => LED driver started 
*/
void SX1509::ledConfig(int pin)
{
  int hotConfig; // the current value of the register
  int reg;       // register to read/write
  int bank = getBank(pin);
  int pinNum = getPinPos(pin);

  reg = REG_INPUT_DISABLE_B + bank;
  hotConfig = this->i2cRead(reg);
  this->i2cWrite(reg, bitwise_write_bit(hotConfig, pinNum, 1)); // Disable the input buffer (HIGH)

  this->disablePullup(pin); // remove internal pull-up (LOW)
  
  reg = REG_OPEN_DRAIN_B + bank;
  hotConfig = this->i2cRead(reg);
  this->i2cWrite(reg, bitwise_write_bit(hotConfig, pinNum, 1)); // set IO to open drain (HIGH)

  this->setDirection(pin, 0); // Set IO direction to output (LOW)

  reg = REG_LED_DRIVER_ENABLE_B + bank;
  hotConfig = this->i2cRead(reg);
  this->i2cWrite(reg, bitwise_write_bit(hotConfig, pinNum, 1));

  this->digitalWrite(pin, 0); // setting data LOW means LED Driver started
}

/**
 * pin: ppin number
 * value: 0..255 (0 == led off)
*/
void SX1509::analogWrite(int pin, uint8_t value)
{
  this->setPWM(pin, value);
}

/**
 * NOTE: For ANALOG_OUTPUT pins, a value of 0 will "start" the LED Driver, and a value of 1 will disable LED driver (I think)
*/ 
void SX1509::digitalWrite(int pin, int value) {
  int bank = getBank(pin);
  int pinPos = getPinPos(pin);
  int reg = REG_DATA_B + bank;
  this->i2cWrite(reg, bitwise_write_bit(this->i2cRead(reg), pinPos, value));
}

int SX1509::digitalRead(int pin)
{
  int bank = getBank(pin);
  int pinPos = getPinPos(pin);
  int reg = REG_DATA_B + bank;
  int data = this->i2cRead(reg);
  return bitwise_read_bit(data, pinPos);
}

int SX1509::digitalToggle(int pin) {
  int bank = getBank(pin);
  int pinPos = getPinPos(pin);
  int reg = REG_DATA_B + bank;
  this->i2cWrite(reg, bitwise_flip_bit(this->i2cRead(reg), pinPos));
}

/**
 * returns bank A IO states in 8-bits
*/ 
uint8_t SX1509::readBankA()
{
  return this->i2cRead(REG_DATA_A);
}

uint8_t SX1509::readBankB()
{
  return this->i2cRead(REG_DATA_B);
}

/**
 * write an 8-bit value to all 8 bank A pins
*/ 
void SX1509::writeBankA(uint8_t data)
{
  this->i2cWrite(REG_DATA_A, data);
}

/**
 * write an 8-bit value to all 8 bank B pins
*/
void SX1509::writeBankB(uint8_t data)
{
  this->i2cWrite(REG_DATA_B, data);
}



/**
 * 0 : IO is configured as an output
 * 1 : IO is configured as an input
*/
void SX1509::setDirection(int pin, int inOut)
{
  int bank = getBank(pin);
  int pinPos = getPinPos(pin);
  int reg = REG_DIR_B + bank;
  this->i2cWrite(reg, bitwise_write_bit(this->i2cRead(reg), pinPos, inOut));
}

/**
Enables open drain operation for each [output-configured] IO
@param: value 1 or 0
0 : Regular push-pull operation
1 : Open drain operation
*/
void SX1509::setOpenDrain(int pin, int value)
{
  if (value > 1) {
    value = 1;
  } else {
    value = 0;
  }
  int bank = getBank(pin);
  int pinPos = getPinPos(pin);
  int reg = REG_OPEN_DRAIN_B + bank;
  this->i2cWrite(reg, bitwise_write_bit(this->i2cRead(reg), pinPos, value));
}


void SX1509::enablePullup(int pin)
{
  int bank = getBank(pin);
  int pinPos = getPinPos(pin);
  int reg = REG_PULL_UP_B + bank;
  this->i2cWrite(reg, bitwise_write_bit(this->i2cRead(reg), pinPos, 1));
}

void SX1509::disablePullup(int pin)
{
  int bank = getBank(pin);
  int pinPos = getPinPos(pin);
  int reg = REG_PULL_UP_B + bank;
  this->i2cWrite(reg, bitwise_write_bit(this->i2cRead(reg), pinPos, 0));
}

/** LED PWM
 * ON Intensity of IO[X]
 * 
 * Linear mode : IOnX = RegIOnX
 * Logarithmic mode (fading capable IOs only) : IOnX = f(RegIOnX)
*/
void SX1509::setPWM(int pin, int value)
{
  this->i2cWrite(REG_I_ON[pin], value); // sets the PWM / brightness
}

/** LED Driver Blink Mode
 * 
 * Invoked when TOnX != 0 and TOffX != 0. (they both default to 0x00)
 * If the I/O doesn’t support fading the LED intensity will step directly to the IOnX/IOffX value.
 * When RegData(X) is cleared, the LED will complete any current ramp, and then stay at minimum intensity
*/
void SX1509::blinkLED(int pin, uint8_t onTime, uint8_t offTime, uint8_t onIntensity, uint8_t offIntensity)
{
  this->setOnTime(pin, onTime);
  this->setPWM(pin, onIntensity);
  this->setOffTime(pin, offTime, offIntensity);
}

/**
 * @brief Set the amount of time the LED will remain ON in blink mode
 * @param onTime:
      0 : Infinite (Static mode, TOn directly controlled by RegData, Cf §4.8.2)
      1 - 15 : TOnX = 64 * RegTOnX * (255/ClkX)
      16 - 31 : TOnX = 512 * RegTOnX * (255/ClkX)
*/
void SX1509::setOnTime(int pin, uint8_t onTime) {
  this->i2cWrite(REG_T_ON[pin], (onTime > 31) ? 31 : onTime);
}

/**
OFF Time + OFF Intensity of IO[X]:

bits 7:3
  0 : Infinite (Single shot mode, TOff directly controlled by RegData, Cf §4.8.3) 1 - 15 : TOffX = 64 * RegOffX[7:3] * (255/ClkX)
  16 - 31 : TOffX = 512 * RegOffX[7:3] * (255/ClkX)

bits 2:0
  OFF Intensity of IO[X]
    - Linear mode : IOffX = 4 x RegOff[2:0]
    - Logarithmic mode (fading capable IOs only) : IOffX = f(4 x RegOffX[2:0]) , Cf §4.8.5
*/

void SX1509::setOffTime(int pin, uint8_t offTime, uint8_t offIntensity)
{
  uint8_t offValue = offTime << 3;   // offTime is 5 bits, from bit 7:3
  offValue |= (offIntensity & 0x07); // offIntensity is a 3 bit number, from bit 2:0
  this->i2cWrite(REG_OFF[pin], offValue);
}

void SX1509::setBlinkFrequency(ClockSpeed speed)
{
  _miscConfig = this->i2cRead(REG_MISC);
  int mask = 0b10001111; // targeting bits 6:4
  _miscConfig &= mask;   // clear bits 6:4
  _miscConfig |= static_cast<uint8_t>(speed);
  this->i2cWrite(REG_MISC, _miscConfig);
}

/**
 * implementation not fully complete. 
 */
void SX1509::setDriverMode(bool linear)
{
  int hotValue = this->i2cRead(REG_MISC);
  if (linear) {
    hotValue = bitwise_clear_bit(hotValue, 7);
  }
  else {
    hotValue = bitwise_set_bit(hotValue, 7);
  }
  this->i2cWrite(REG_MISC, hotValue);
};

// configure Clock

/**

Bit 7 = unused

Oscillator frequency (fOSC) source (bits 6:5)
00 : OFF. LED driver, keypad engine and debounce features are disabled.
01 : External clock input (OSCIN)
10 : Internal 2MHz oscillator
11 : Reserved

OSCIO pin function (bit 4)
0 : OSCIO is an input (OSCIN)
1 : OSCIO is an output (OSCOUT)

Frequency of the signal output on OSCOUT pin (bits 3:0):
0x0 : 0Hz, permanent "0" logical level (GPO)
0xF : 0Hz, permanent "1" logical level (GPO)
Else : fOSCOUT = fOSC/(2^(RegClock[3:0]-1))

*/

/* Input Debouncer

Each input can be individually debounced by setting corresponding bits in RegDebounce register. At power up the debounce function is disabled. After enabling the debouncer, the change of the input value is accepted only if the input value is identical at two consecutive sampling times.
The debounce time common to all IOs can be set in RegDebounceConfig register from 0.5 to 64ms (fOSC = 2MHz)
*/