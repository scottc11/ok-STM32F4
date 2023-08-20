
#ifndef _SX1509_H_
#define _SX1509_H_

#include "BitwiseMethods.h"
#include <stdint.h>
#include "I2C.h"

#define SX1509_ADDR  0x3E

class SX1509 {
public:
  enum class PinMode : uint8_t
  {
    OUTPUT = 0x0,
    INPUT = 0x1,
    ANALOG_OUTPUT = 0x2
  };

  enum class Bank : uint8_t
  {
    BANK_A = 1,
    BANK_B = 0
  };

  enum class DriverMode : uint8_t
  {
    LINEAR = 0b00000000,
    LOGARITHMIC = 0b10001000 // sets both banks LED Driver Mode
  };

  // Oscillator frequency (fOSC) source
  enum class ClockSource : uint8_t
  {
    OFF = 0b00000000,
    EXTERNAL = 0b00100000,
    INTERNAL = 0b01000000
  };

  // OSCIO pin function
  enum class OSCIO : uint8_t
  {
    OUTPUT = 0b00010000,
    INPUT = 0b00000000
  };

  // Frequency of the signal output on OSCOUT pin:
  enum class fOSCOUT : uint8_t
  {
    PERMANENT_LOW = 0,
    PERMANENT_HIGH = 1,
    DIVISION_OF_REG_CLOCK = 2
  };

  enum class ClockSpeed : uint8_t
  {
    ULTRA_FAST = 0b00010000,
    EXTRA_FAST = 0b00100000,
    FAST = 0b00110000,
    MEDIUM = 0b01000000,
    SLOW = 0b01010000,
    EXTRA_SLOW = 0b01100000,
    ULTRA_SLOW = 0b01110000,
  };

  enum class InteruptDirection : uint8_t
  {
    NONE = 0,
    RISING = 1,
    FALLING = 2,
    RISE_FALL = 3,
  };

  SX1509(I2C *_i2c, char _addr = SX1509_ADDR) {
		address = _addr << 1;
		i2c = _i2c;
	};

	I2C * i2c;
	char address;
  char REG_I_ON[16] = { REG_I_ON_0, REG_I_ON_1, REG_I_ON_2, REG_I_ON_3, REG_I_ON_4, REG_I_ON_5, REG_I_ON_6, REG_I_ON_7, REG_I_ON_8, REG_I_ON_9, REG_I_ON_10, REG_I_ON_11, REG_I_ON_12, REG_I_ON_13, REG_I_ON_14, REG_I_ON_15 };
  char REG_T_ON[16] = { REG_T_ON_0, REG_T_ON_1, REG_T_ON_2, REG_T_ON_3, REG_T_ON_4, REG_T_ON_5, REG_T_ON_6, REG_T_ON_7, REG_T_ON_8, REG_T_ON_9, REG_T_ON_10, REG_T_ON_11, REG_T_ON_12, REG_T_ON_13, REG_T_ON_14, REG_T_ON_15 };
  char REG_OFF[16] = { REG_OFF_0, REG_OFF_1, REG_OFF_2, REG_OFF_3, REG_OFF_4, REG_OFF_5, REG_OFF_6, REG_OFF_7, REG_OFF_8, REG_OFF_9, REG_OFF_10, REG_OFF_11, REG_OFF_12, REG_OFF_13, REG_OFF_14, REG_OFF_15};
  /** Software Reset
	 */
  void init(bool extClock=false);
  void reset();
  bool isConnected();
	void digitalWrite(int pin, int value);
	int digitalRead(int pin);
  int digitalToggle(int pin);
  void analogWrite(int pin, uint8_t value);
  uint8_t readBankA();
  uint8_t readBankB();
  void writeBankA(uint8_t data);
  void writeBankB(uint8_t data);

  void setClockConfig(ClockSource clockSource, OSCIO oscPin, fOSCOUT oscState);
  void setMiscConfig(DriverMode driverMode, ClockSpeed clockSpeed);

  void ledConfig(int pin);
  void setDirection(int pin, int inOut);
  void setPolarity(int pin, int polarity);
  void pinMode(int pin, PinMode mode, bool invertPolarity = false); // should handle all types of pin modes
  void setPWM(int pin, int value);
  void blinkLED(int pin, uint8_t onTime, uint8_t offTime, uint8_t onIntensity, uint8_t offIntensity);
  void setBlinkFrequency(ClockSpeed speed);
  void setOnTime(int pin, uint8_t onTime);
  void setOffTime(int pin, uint8_t offTime, uint8_t offIntensity);
  

  void setDriverMode(bool linear);
  void setInputDebounce(int pin, bool debounce);
  void setDebounceTime(int value);
  void setInterupt(int pin, bool willNotInterupt, InteruptDirection sense);
  void setOpenDrain(int pin, int value);
  
  void enablePullup(int pin);
  void disablePullup(int pin);
  void enableInterupt(int pin, InteruptDirection type);
  void disableInterupt(int pin);
  int getInteruptSource(Bank bank);

  int getBank(int pin) { return (pin < 8) ? 1 : 0; };        // for bank A increment all commands by 1, else don't increment
  int getPinPos(int pin) { return (pin < 8) ? pin : pin - 8; }; // pin bit position

private:
  uint8_t _clockConfig = 0x00;
  uint8_t _miscConfig = 0x00;

  inline void i2cWrite(char _command, char _data1, char _data2){
		uint8_t commands[3];
		commands[0] = _command;
		commands[1] = _data1;
		commands[2] = _data2;

		i2c->write(address, commands, 3);
	}
	
	inline void i2cWrite(char _command, char _data1){
    uint8_t commands[2];
    commands[0] = _command;
		commands[1] = _data1;
		
		i2c->write(address, commands, 2);
	}
	
	inline char i2cRead(char _command){
    uint8_t commands[2];
    commands[0] = _command;
		i2c->write(address, commands, 1);
		//return (char)i2c->read(0);
		i2c->read(address,commands,1);
		return commands[0];
	}

  // registors
  enum SX1509_REG
  {
    // Stolen from SparkFun 🙂 https://github.com/sparkfun/SparkFun_SX1509_Arduino_Library/blob/master/

    // Configuration
    REG_INPUT_DISABLE_B = 0x00,     //	RegInputDisableB Input buffer disable register _ I/O[15_8] (Bank B) 0000 0000
    REG_INPUT_DISABLE_A = 0x01,     //	RegInputDisableA Input buffer disable register _ I/O[7_0] (Bank A) 0000 0000
    REG_LONG_SLEW_B = 0x02,         //	RegLongSlewB Output buffer long slew register _ I/O[15_8] (Bank B) 0000 0000
    REG_LONG_SLEW_A = 0x03,         //	RegLongSlewA Output buffer long slew register _ I/O[7_0] (Bank A) 0000 0000
    REG_LOW_DRIVE_B = 0x04,         //	RegLowDriveB Output buffer low drive register _ I/O[15_8] (Bank B) 0000 0000
    REG_LOW_DRIVE_A = 0x05,         //	RegLowDriveA Output buffer low drive register _ I/O[7_0] (Bank A) 0000 0000
    REG_PULL_UP_B = 0x06,           //	RegPullUpB Pull_up register _ I/O[15_8] (Bank B) 0000 0000
    REG_PULL_UP_A = 0x07,           //	RegPullUpA Pull_up register _ I/O[7_0] (Bank A) 0000 0000
    REG_PULL_DOWN_B = 0x08,         //	RegPullDownB Pull_down register _ I/O[15_8] (Bank B) 0000 0000
    REG_PULL_DOWN_A = 0x09,         //	RegPullDownA Pull_down register _ I/O[7_0] (Bank A) 0000 0000
    REG_OPEN_DRAIN_B = 0x0A,        //	RegOpenDrainB Open drain register _ I/O[15_8] (Bank B) 0000 0000
    REG_OPEN_DRAIN_A = 0x0B,        //	RegOpenDrainA Open drain register _ I/O[7_0] (Bank A) 0000 0000
    REG_POLARITY_B = 0x0C,          //	RegPolarityB Polarity register _ I/O[15_8] (Bank B) 0000 0000
    REG_POLARITY_A = 0x0D,          //	RegPolarityA Polarity register _ I/O[7_0] (Bank A) 0000 0000
    REG_DIR_B = 0x0E,               //	RegDirB Direction register _ I/O[15_8] (Bank B) 1111 1111
    REG_DIR_A = 0x0F,               //	RegDirA Direction register _ I/O[7_0] (Bank A) 1111 1111
    REG_DATA_B = 0x10,              //	RegDataB Data register _ I/O[15_8] (Bank B) 1111 1111*
    REG_DATA_A = 0x11,              //	RegDataA Data register _ I/O[7_0] (Bank A) 1111 1111*
    REG_INTERRUPT_MASK_B = 0x12,    //	RegInterruptMaskB Interrupt mask register _ I/O[15_8] (Bank B) 1111 1111
    REG_INTERRUPT_MASK_A = 0x13,    //	RegInterruptMaskA Interrupt mask register _ I/O[7_0] (Bank A) 1111 1111
    REG_SENSE_HIGH_B = 0x14,        //	RegSenseHighB Sense register for I/O[15:12] 0000 0000
    REG_SENSE_LOW_B = 0x15,         //	RegSenseLowB Sense register for I/O[11:8] 0000 0000
    REG_SENSE_HIGH_A = 0x16,        //	RegSenseHighA Sense register for I/O[7:4] 0000 0000
    REG_SENSE_LOW_A = 0x17,         //	RegSenseLowA Sense register for I/O[3:0] 0000 0000
    REG_INTERRUPT_SOURCE_B = 0x18,  //	RegInterruptSourceB Interrupt source register _ I/O[15_8] (Bank B) 0000 0000
    REG_INTERRUPT_SOURCE_A = 0x19,  //	RegInterruptSourceA Interrupt source register _ I/O[7_0] (Bank A) 0000 0000
    REG_EVENT_STATUS_B = 0x1A,      //	RegEventStatusB Event status register _ I/O[15_8] (Bank B) 0000 0000
    REG_EVENT_STATUS_A = 0x1B,      //	RegEventStatusA Event status register _ I/O[7_0] (Bank A) 0000 0000
    REG_LEVEL_SHIFTER_1 = 0x1C,     //	RegLevelShifter1 Level shifter register 0000 0000
    REG_LEVEL_SHIFTER_2 = 0x1D,     //	RegLevelShifter2 Level shifter register 0000 0000
    REG_CLOCK = 0x1E,               //	RegClock Clock management register 0000 0000
    REG_MISC = 0x1F,                //	RegMisc Miscellaneous device settings register 0000 0000
    REG_LED_DRIVER_ENABLE_B = 0x20, //	RegLEDDriverEnableB LED driver enable register _ I/O[15_8] (Bank B) 0000 0000
    REG_LED_DRIVER_ENABLE_A = 0x21, //	RegLEDDriverEnableA LED driver enable register _ I/O[7_0] (Bank A) 0000 0000

    // Debounce and Keypad Engine
    REG_DEBOUNCE_CONFIG = 0x22,   //	RegDebounceConfig Debounce configuration register 0000 0000
    REG_DEBOUNCE_ENABLE_B = 0x23, //	RegDebounceEnableB Debounce enable register _ I/O[15_8] (Bank B) 0000 0000
    REG_DEBOUNCE_ENABLE_A = 0x24, //	RegDebounceEnableA Debounce enable register _ I/O[7_0] (Bank A) 0000 0000
    REG_KEY_CONFIG_1 = 0x25,      //	RegKeyConfig1 Key scan configuration register 0000 0000
    REG_KEY_CONFIG_2 = 0x26,      //	RegKeyConfig2 Key scan configuration register 0000 0000
    REG_KEY_DATA_1 = 0x27,        //	RegKeyData1 Key value (column) 1111 1111
    REG_KEY_DATA_2 = 0x28,        //	RegKeyData2 Key value (row) 1111 1111

    // LED Driver (PWM, blinking, breathing)
    REG_T_ON_0 = 0x29,    //	RegTOn0 ON time register for I/O[0] 0000 0000
    REG_I_ON_0 = 0x2A,    //	RegIOn0 ON intensity register for I/O[0] 1111 1111
    REG_OFF_0 = 0x2B,     //	RegOff0 OFF time/intensity register for I/O[0] 0000 0000
    REG_T_ON_1 = 0x2C,    //	RegTOn1 ON time register for I/O[1] 0000 0000
    REG_I_ON_1 = 0x2D,    //	RegIOn1 ON intensity register for I/O[1] 1111 1111
    REG_OFF_1 = 0x2E,     //	RegOff1 OFF time/intensity register for I/O[1] 0000 0000
    REG_T_ON_2 = 0x2F,    //	RegTOn2 ON time register for I/O[2] 0000 0000
    REG_I_ON_2 = 0x30,    //	RegIOn2 ON intensity register for I/O[2] 1111 1111
    REG_OFF_2 = 0x31,     //	RegOff2 OFF time/intensity register for I/O[2] 0000 0000
    REG_T_ON_3 = 0x32,    //	RegTOn3 ON time register for I/O[3] 0000 0000
    REG_I_ON_3 = 0x33,    //	RegIOn3 ON intensity register for I/O[3] 1111 1111
    REG_OFF_3 = 0x34,     //	RegOff3 OFF time/intensity register for I/O[3] 0000 0000
    REG_T_ON_4 = 0x35,    //	RegTOn4 ON time register for I/O[4] 0000 0000
    REG_I_ON_4 = 0x36,    //	RegIOn4 ON intensity register for I/O[4] 1111 1111
    REG_OFF_4 = 0x37,     //	RegOff4 OFF time/intensity register for I/O[4] 0000 0000
    REG_T_RISE_4 = 0x38,  //	RegTRise4 Fade in register for I/O[4] 0000 0000
    REG_T_FALL_4 = 0x39,  //	RegTFall4 Fade out register for I/O[4] 0000 0000
    REG_T_ON_5 = 0x3A,    //	RegTOn5 ON time register for I/O[5] 0000 0000
    REG_I_ON_5 = 0x3B,    //	RegIOn5 ON intensity register for I/O[5] 1111 1111
    REG_OFF_5 = 0x3C,     //	RegOff5 OFF time/intensity register for I/O[5] 0000 0000
    REG_T_RISE_5 = 0x3D,  //	RegTRise5 Fade in register for I/O[5] 0000 0000
    REG_T_FALL_5 = 0x3E,  //	RegTFall5 Fade out register for I/O[5] 0000 0000
    REG_T_ON_6 = 0x3F,    //	RegTOn6 ON time register for I/O[6] 0000 0000
    REG_I_ON_6 = 0x40,    //	RegIOn6 ON intensity register for I/O[6] 1111 1111
    REG_OFF_6 = 0x41,     //	RegOff6 OFF time/intensity register for I/O[6] 0000 0000
    REG_T_RISE_6 = 0x42,  //	RegTRise6 Fade in register for I/O[6] 0000 0000
    REG_T_FALL_6 = 0x43,  //	RegTFall6 Fade out register for I/O[6] 0000 0000
    REG_T_ON_7 = 0x44,    //	RegTOn7 ON time register for I/O[7] 0000 0000
    REG_I_ON_7 = 0x45,    //	RegIOn7 ON intensity register for I/O[7] 1111 1111
    REG_OFF_7 = 0x46,     //	RegOff7 OFF time/intensity register for I/O[7] 0000 0000
    REG_T_RISE_7 = 0x47,  //	RegTRise7 Fade in register for I/O[7] 0000 0000
    REG_T_FALL_7 = 0x48,  //	RegTFall7 Fade out register for I/O[7] 0000 0000
    REG_T_ON_8 = 0x49,    //	RegTOn8 ON time register for I/O[8] 0000 0000
    REG_I_ON_8 = 0x4A,    //	RegIOn8 ON intensity register for I/O[8] 1111 1111
    REG_OFF_8 = 0x4B,     //	RegOff8 OFF time/intensity register for I/O[8] 0000 0000
    REG_T_ON_9 = 0x4C,    //	RegTOn9 ON time register for I/O[9] 0000 0000
    REG_I_ON_9 = 0x4D,    //	RegIOn9 ON intensity register for I/O[9] 1111 1111
    REG_OFF_9 = 0x4E,     //	RegOff9 OFF time/intensity register for I/O[9] 0000 0000
    REG_T_ON_10 = 0x4F,   //	RegTOn10 ON time register for I/O[10] 0000 0000
    REG_I_ON_10 = 0x50,   //	RegIOn10 ON intensity register for I/O[10] 1111 1111
    REG_OFF_10 = 0x51,    //	RegOff10 OFF time/intensity register for I/O[10] 0000 0000
    REG_T_ON_11 = 0x52,   //	RegTOn11 ON time register for I/O[11] 0000 0000
    REG_I_ON_11 = 0x53,   //	RegIOn11 ON intensity register for I/O[11] 1111 1111
    REG_OFF_11 = 0x54,    //	RegOff11 OFF time/intensity register for I/O[11] 0000 0000
    REG_T_ON_12 = 0x55,   //	RegTOn12 ON time register for I/O[12] 0000 0000
    REG_I_ON_12 = 0x56,   //	RegIOn12 ON intensity register for I/O[12] 1111 1111
    REG_OFF_12 = 0x57,    //	RegOff12 OFF time/intensity register for I/O[12] 0000 0000
    REG_T_RISE_12 = 0x58, //	RegTRise12 Fade in register for I/O[12] 0000 0000
    REG_T_FALL_12 = 0x59, //	RegTFall12 Fade out register for I/O[12] 0000 0000
    REG_T_ON_13 = 0x5A,   //	RegTOn13 ON time register for I/O[13] 0000 0000
    REG_I_ON_13 = 0x5B,   //	RegIOn13 ON intensity register for I/O[13] 1111 1111
    REG_OFF_13 = 0x5C,    //	RegOff13 OFF time/intensity register for I/O[13] 0000 0000
    REG_T_RISE_13 = 0x5D, //	RegTRise13 Fade in register for I/O[13] 0000 0000
    REG_T_FALL_13 = 0x5E, //	RegTFall13 Fade out register for I/O[13] 0000 0000
    REG_T_ON_14 = 0x5F,   //	RegTOn14 ON time register for I/O[14] 0000 0000
    REG_I_ON_14 = 0x60,   //	RegIOn14 ON intensity register for I/O[14] 1111 1111
    REG_OFF_14 = 0x61,    //	RegOff14 OFF time/intensity register for I/O[14] 0000 0000
    REG_T_RISE_14 = 0x62, //	RegTRise14 Fade in register for I/O[14] 0000 0000
    REG_T_FALL_14 = 0x63, //	RegTFall14 Fade out register for I/O[14] 0000 0000
    REG_T_ON_15 = 0x64,   //	RegTOn15 ON time register for I/O[15] 0000 0000
    REG_I_ON_15 = 0x65,   //	RegIOn15 ON intensity register for I/O[15] 1111 1111
    REG_OFF_15 = 0x66,    //	RegOff15 OFF time/intensity register for I/O[15] 0000 0000
    REG_T_RISE_15 = 0x67, //	RegTRise15 Fade in register for I/O[15] 0000 0000
    REG_T_FALL_15 = 0x68, //	RegTFall15 Fade out register for I/O[15] 0000 0000

    // 	Miscellaneous
    REG_HIGH_INPUT_B = 0x69, //	RegHighInputB High input enable register _ I/O[15_8] (Bank B) 0000 0000
    REG_HIGH_INPUT_A = 0x6A, //	RegHighInputA High input enable register _ I/O[7_0] (Bank A) 0000 0000

    //  Software Reset
    REG_RESET = 0x7D,  //	RegReset Software reset register 0000 0000
    REG_TEST_1 = 0x7E, //	RegTest1 Test register 0000 0000
    REG_TEST_2 = 0x7F, //	RegTest2 Test register 0000 0000
  };

}; // end of class

#endif

