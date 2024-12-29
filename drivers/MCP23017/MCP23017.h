/**
 * Example: All pins as GPIO Inputs

mcp.init();
mcp.setDirection(MCP23017_PORTA, 0xff);
mcp.setDirection(MCP23017_PORTB, 0b11111111);
mcp.setInterupt(MCP23017_PORTA, 0xff);
mcp.setInterupt(MCP23017_PORTB, 0b11111111);
mcp.setPullUp(MCP23017_PORTA, 0xff);
mcp.setPullUp(MCP23017_PORTB, 0b01111111);
mcp.setInputPolarity(MCP23017_PORTA, 0xff);
mcp.setInputPolarity(MCP23017_PORTB, 0b11111111);

mcp.digitalReadAB(); // clear any stray interupts after configuration

*/

#ifndef _MCP23017_H_
#define _MCP23017_H_

#include "I2C.h"

#define MCP23017_DEFAULT_ADDR 0x20
#define MCP23017_I2C_ADDR_010 0x22 // 0b0100010
#define MCP23017_I2C_ADDR_100 0x24 // 0b0100100
#define MCP23017_CTRL_ADDR 0x24	   // 0b0100100
#define MCP23017_PORTA 0x00
#define MCP23017_PORTB 0x01

/**
 * supports I2C speeds 400kHz, 1.7MHz 
 * NOTE: This class assumes the use of bank = 0 for register addresses. Reference the datasheet.
 * NOTE: 
*/
class MCP23017 {
	
	public:
	
	MCP23017(I2C *_i2c, char _addr) {
		address = _addr << 1;
		i2c = _i2c;
	};

	I2C * i2c;
	char address;
	uint16_t currPinStates;
	uint16_t prevPinStates;

	/** Software Reset
	 */
	void init(void);
	bool isConnected();
	void setConfig(char _value);
	uint8_t getConfig();
	void setDirection(char _port, char _value);       // set port pins to input or output.  1 = input, 0 = output	
	void setPullUp(char _port, char _value);          // activate pin pull-ups
	void setInputPolarity(char _port, char _value);   // invert pin input polarity
	void setInterupt(char _port, char _value);        // enable/disable interupts per pin/port
	void digitalWrite(char _port, char _value);
	void digitalWrite(char port, int pin, bool set);
	void digitalToggle(char _port, char _pin);
	char digitalRead(char _port);
	uint16_t digitalReadAB();
	
	int getBitStatus(int byte, int bit);
	
	// void setInterruptConfig(char _port, char _intcon, char _defval);
	// void interrupt(char _port, char _value);
	// char readIntcap(char _port);


private:

	inline void i2cSend(char _command, char _data1, char _data2){
		uint8_t commands[3];
		commands[0] = _command;
		commands[1] = _data1;
		commands[2] = _data2;

		i2c->write(address, commands, 3);
	}
	
	inline void i2cSend(char _command, char _data1){
		uint8_t commands[2];
		commands[0] = _command;
		commands[1] = _data1;
		
		i2c->write(address, commands, 2);
	}

	inline char i2cRead(char _command)
	{
		uint8_t commands[2];
		commands[0] = _command;
		i2c->write(address, commands, 1);
		i2c->read(address,commands,1);
		return commands[0];
	}

	// registors
	enum MCP23017_REG {
		// IO Dirrection
		REG_IODIR = 0x00,

		// Input polarity
		REG_IPOL = 0x02,

		// Interrupt-On-Change enable
		REG_GPINTEN = 0x04,

		// Default intterupt comparison value
		// Refer to INTCON
		REG_DEFVAL = 0x06,

		// Interupt compare value
		// 1 = compare against DEFVAL value
		// 0 = compare against the previous value
		REG_INTCON = 0x08,

		// IC setting
		// bank/mirror/seqop/disslw/haen/odr/intpol/notimp
		REG_IOCON = 0x0A,

		// Pullup
		REG_GPPU = 0x0C,

		// Interrupt Flag
		// 1 = pin caused interrupt
		// 0 = no interrupt 
		// Read-Only
		REG_INTF = 0x0E,

		// Interrupt capture
		// value of GPIO at time of last interrupt
		// Read-Only
		REG_INTCAP = 0x10,

		// GPIO
		REG_GPIO = 0x12,

		// Output Latch
		REG_OLAT = 0x14
	};
	
}; // end of class

#endif

