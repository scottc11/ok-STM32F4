/* MCP23017 Library for mbed
 * Copyright (c) 2014, Takuya Urakawa
 * 
 * This library is released under the MIT License
 * See http://opensource.org/licenses/mit-license.php
 */

#include "MCP23017.h"

void MCP23017::init(void) {
	
	// init config, mirror interrupts
	setConfig(0b01000000);

	// port 0
	i2cSend(REG_GPIO, 0, 0);

	// port direction all output
	i2cSend(REG_IODIR, 0, 0);

	// interupt off
	i2cSend(REG_GPINTEN, 0, 0);

	// clear interrupt
	digitalRead(MCP23017_PORTA);
	digitalRead(MCP23017_PORTB);
}

bool MCP23017::isConnected()
{
	// write some reg
	bool status;
	uint8_t originalConfig = getConfig();          // obtain current configuration
	char writeData = 0b01010100;				   // test data
	setConfig(writeData);						   // write test data to register
	uint8_t readData = getConfig();				   // read back that reg
	status = readData == writeData ? true : false; // confirm
	setConfig(originalConfig);                      // clear / reset that reg back to default
	return status;
}

/**
 * @brief
 *   Bit 7 ............................................... Bit 0
 * | BANK | MIRROR | SEQOP | DISSLW | HAEN | ODR | INTPOL |  -  |
 * 
	BANK: Controls how the registers are addressed
		1 = The registers associated with each port are separated into different banks.
		0 = The registers are in the same bank (addresses are sequential).
	MIRROR: INT Pins Mirror bit
		1 = The INT pins are internally connected
		0 = The INT pins are not connected. INTA is associated with PORTA and INTB is associated with PORTB
	SEQOP: Sequential Operation mode bit
		1 = Sequential operation disabled, address pointer does not increment.
		0 = Sequential operation enabled, address pointer increments.
	DISSLW: Slew Rate control bit for SDA output
		1 = Slew rate disabled
		0 = Slew rate enabled
	HAEN: Hardware Address Enable bit (MCP23S17 only) (Note 1)
		1 = Enables the MCP23S17 address pins.
		0 = Disables the MCP23S17 address pins.
	ODR: Configures the INT pin as an open-drain output
		1 = Open-drain output (overrides the INTPOL bit.)
		0 = Active driver output (INTPOL bit sets the polarity.)
	INTPOL: This bit sets the polarity of the INT output pin
		1= Active-high
		0= Active-low
	UNIMPLEMENTED: Read as ‘0’
 *
 * @note The INTn interrupt output can be configured as active-low, active-high or open-drain via the IOCON register.
 * @param config
 */
void MCP23017::setConfig(char config) {
	i2cSend(REG_IOCON, config);
}

uint8_t MCP23017::getConfig() {
	return i2cRead(REG_IOCON);
}

/**
 * @brief Controls the direction of the data I/O.
 * When a bit is set, the corresponding pin becomes an input. When a bit is clear, the corresponding pin becomes an output.
 * 1 = input, 0 = output
*/
void MCP23017::setDirection(char _port, char _value) {
	i2cSend(REG_IODIR + _port, _value);
}

/**
 * @brief The GPPU register controls the pull-up resistors for the port pins. 
 * If a bit is set and the corresponding pin is configured as an input, the corresponding port pin is internally pulled up with a 100 k􏰁 resistor.
*/
void MCP23017::setPullUp(char _port, char _value) {
	i2cSend(REG_GPPU + _port, _value);
}

/**
 * @brief This register allows the user to configure the polarity on the corresponding GPIO port bits.
 * If a bit is set, the corresponding GPIO register bit will reflect the inverted value on the pin.
*/
void MCP23017::setInputPolarity(char _port, char _value) {
	i2cSend(REG_IPOL + _port, _value);
}

/**
 * If enabled, the MCP23X17 activates the INTn interrupt output when one of the port pins changes state or when a pin does not match the preconfigured default.
 * Additionally, the INTn pins can be configured to mirror each other so that any interrupt will cause both pins to go active. This is controlled via IOCON.MIRROR.
 * There are two interrupt pins: INTA and INTB. By default, INTA is associated with GPAn pins (PORTA) and INTB is associated with GPBn pins (PORTB).
 * NOTE: Each port has an independent signal which is cleared if its associated GPIO or INTCAP register is read.
**/
void MCP23017::setInterupt(char _port, char _value) {
	i2cSend(REG_GPINTEN + _port, _value );
}

/**
 * This register allows the user to configure the polarity on the corresponding GPIO port bits.
 * If a bit is set, the corresponding GPIO register bit will reflect the inverted value on the pin.
 * for some reason, I think the default is inverted...
**/
void MCP23017::digitalWrite(char _port, char _value) {
	i2cSend(REG_GPIO + _port, _value);
}

/**
 * @brief Write to a specific pin on a specific port
 * 
 * @param port 
 * @param pin 
 * @param set 
 */
void MCP23017::digitalWrite(char port, int pin, bool set)
{
	char value = i2cRead(REG_GPIO + port);
	if (set)
	{
		value |= (1 << pin);
	}
	else
	{
		value &= ~(1 << pin);
	}
	i2cSend(REG_GPIO + port, value);
}

void MCP23017::digitalToggle(char _port, char _pin) {
	char value = i2cRead(REG_GPIO + _port);
	value ^= (1 << _pin);
	i2cSend(REG_GPIO + _port, value);
}


char MCP23017::digitalRead(char _port) {
	return i2cRead(REG_GPIO + _port);
}


/**
 * read both ports and append into a 16 bit value 
*/
uint16_t MCP23017::digitalReadAB() {
	int portA = this->digitalRead(MCP23017_PORTA);
	int portB = this->digitalRead(MCP23017_PORTB);
	prevPinStates = currPinStates;
	currPinStates = (portB << 8) | portA;
	return currPinStates;
}

int MCP23017::getBitStatus(int byte, int bit) {
	return (byte & (1 << bit));
}
