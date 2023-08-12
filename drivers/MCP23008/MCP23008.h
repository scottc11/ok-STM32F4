
#ifndef _MCP23008_H_
#define _MCP23008_H_

#include <mbed.h>
#include <OK_I2C.h>

#define MCP23008_DEFAULT_ADDR 0x20

// supports I2C speeds 400kHz, 1.7MHz

class MCP23008 : public OK_I2C {
	
public:
    MCP23008(I2C *_i2c, char _addr)
    {
        address = _addr << 1;
		i2c = _i2c;
    };

    I2C * i2c;

	void init(void);
	void setConfig(char value);
    void setDirection(char value);      // set port pins to input or output.  1 = input, 0 = output
    void setPullUp(char value);         // activate pin pull-ups
	void setInputPolarity(char value);  // invert pin input polarity
	void setInterupt(char value);       // enable/disable interupts per pin/port
	void writePins(char value);
	char readPins();
	
	int getBitStatus(int byte, int bit);
	

private:
    enum MCP23008_REG
    {
        // Controls the direction of the data I/O.
        IODIR_REG = 0x00,

        // Input polarity
        IPOL_REG = 0x01,

        // Interrupt-On-Change enable
        GPINTEN_REG = 0x02,

        // Default interupt comparison value
        // Refer to INTCON
        DEFVAL_REG = 0x03,

        // Interupt compare value
        // 1 = compare against DEFVAL value
        // 0 = compare against the previous value
        INTCON_REG = 0x04,

        // Configuration Register
        IOCON_REG = 0x05,

        // Pullup
        GPPU_REG = 0x06,

        // Interrupt Flag
        // 1 = pin caused interrupt
        // 0 = no interrupt
        // Read-Only
        INTF_REG = 0x07,

        // Interrupt capture
        // value of GPIO at time of last interrupt
        // Read-Only
        INTCAP_REG = 0x08,

        // GPIO
        GPIO_REG = 0x09,

        // Output Latch
        OLAT_REG = 0x0A
    };

}; // end of class

#endif

