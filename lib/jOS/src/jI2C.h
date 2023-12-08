#pragma once

//#include <jOS.h>
#include <jCommon.h>
//#include <jSystem.h>

#define I2C_SPEED_FAST 400000L
#define I2C_SPEED_NORMAL 100000L
#define I2C_SPEED_HALF (I2C_SPEED_NORMAL >> 1)
#define I2C_SPEED_FORTH (I2C_SPEED_NORMAL >> 2)


#define I2C_FRQ_DEFAULT I2C_SPEED_NORMAL

// https://www.arduino.cc/reference/en/language/functions/communication/wire/

class jI2C
{

public:
	jI2C() { }
	virtual void begin() = 0;
	virtual void end() = 0;

	// Set Bus Frequency
	//freq: [Hz] transmission frquency
	virtual void setFrequency(uint32_t freq) = 0;

	// Starts a I2C Transmission
	// addr: 7-bit address (shiften one left and added R/W bit internal)
	virtual void beginTransmission(uint8_t addr) = 0;
	
	// End ongoing I2C Transmission
	// returns 0 is succesfull (ACK) | non-zero if error (NACK)
	virtual uint8_t endTransmission(bool stopBit) = 0;
	
	// End ongoing I2C Transmission with stopBit
	// returns 0 is succesfull (ACK) | non-zero if error (NACK)
	virtual uint8_t endTransmission(void) = 0;

	// Requests Data to Read from I2C device 
	// https://www.arduino.cc/reference/en/language/functions/communication/wire/requestfrom/
	// address: the 7-bit slave address of the device to request bytes from.
	// quantity: the number of bytes to request.
	// stopBit: true will send a stop message after the request, releasing the bus. False will continually send a restart after the request, keeping the connection active.
	// returns the number of bytes returned from the peripheral device. (if buffered) if not buffered: returns requested quantity
	virtual uint8_t requestFrom(uint8_t address, size_t quantity, bool stopBit) = 0;
	virtual uint8_t requestFrom(uint8_t address, size_t quantity) = 0;

	// Write/Send single byte
	// returns 1 if succesfull | 0 if failed
	virtual size_t write(uint8_t data) = 0;
	
	// Write/Send multiple bytes
	// returns amount of succesfull send bytes
	virtual size_t write(const uint8_t * data, size_t quantity) = 0;

	// Read single byte from I2C (from Device if not buffered, from Buffer if buffered)
	virtual uint8_t read(void) = 0;
	
	// Read multiple bytes from I2C Device
	virtual void read(uint8_t * data, size_t quantity) {
		for(int i = 0; i < quantity; i++)
			data[i] = read();
	}

	
	// Returns number available bytes in RX Buffer (only works with buffered implementations)
	virtual uint16_t available() {
		return 0;
	}

	
	// read a 8bit register value from a 8bit address
	uint8_t read8_reg8(uint8_t device_addr, uint8_t reg)
	{ 
		uint8_t data = 0;
		
		this->beginTransmission(device_addr);
		this->write(reg);
		this->endTransmission(false);
		this->requestFrom(device_addr, 1);
		data = this->read();
		this->endTransmission();
		
		return data;
	}
	
	// read a 8bit register value from a 8bit address
	// returns true if transmission ok | false on error
	bool read8_reg8(uint8_t device_addr, uint8_t reg, uint8_t& data)
	{
		
		bool error = false;
		this->beginTransmission(device_addr);
		error |= this->write(reg) == 0;
		error |= this->endTransmission(false) != 0;
		error |= this->requestFrom(device_addr, 1) != 1;
		data = this->read();
		error |= this->endTransmission() != 0;
		
		return !error;
	}

	// write a 8bit register value to a 8bit address
	bool write8_reg8(uint8_t device_addr, uint8_t reg, uint8_t data) 
	{ 
		bool error = false;
		this->beginTransmission(device_addr);
		error |= this->write(reg) == 0;
		error |= this->write(data) == 0;
		error |= this->endTransmission() != 0;

		return !error;
	}

};



#include <jI2C_Soft.h>

class jI2C_Soft : public jI2C
{
protected:

	SoftwareI2C _i2c;
	//SoftwareI2C WireS;
	

	uint8_t _pin_sda = -1;
	uint8_t _pin_scl = -1;
	
	bool _begun = false;

	uint32_t _freq = I2C_FRQ_DEFAULT;

public:

	jI2C_Soft(uint8_t pin_sda, uint8_t pin_scl)
	{
		_pin_sda = pin_sda;
		_pin_scl = pin_scl;
		//_i2c = &WireS;
	}
	void begin()
	{
		_i2c.begin(_pin_sda, _pin_scl);
	}
	void end()
	{
		//_i2c.end();
	}
	
	void setFrequency(uint32_t freq)
	{
		_freq = freq;
		
		//TODO implement freq
		//_i2c.setClock(freq);
	}

	void beginTransmission(uint8_t addr)
	{
		_i2c.beginTransmission(addr);
	}
	uint8_t endTransmission(bool stopBit)
	{
		return _i2c.endTransmission(stopBit);
	}
	uint8_t endTransmission(void)
	{
		return _i2c.endTransmission();
	}

	uint8_t requestFrom(uint8_t address, size_t quantity, bool stopBit)
	{
		//TODO stopBit??
		return _i2c.requestFrom(address, quantity);
	}
	uint8_t requestFrom(uint8_t address, size_t quantity)
	{
		return _i2c.requestFrom(address, quantity);
	}

	size_t write(uint8_t data)
	{
		return _i2c.write(data);
	}
	size_t write(const uint8_t * data, size_t quantity)
	{
		return _i2c.write(data, quantity);
	}

	uint8_t read(void)
	{
		return _i2c.read();
	}
	
	uint16_t available() override {
		return 1;
	}
	
	
};


/**************************************************************************/


#if defined(PLATFORM_AVR) || defined(PLATFORM_SAMD) || defined(PLATFORM_STM32) || defined(VARIANTE_RP2040MBED) || defined(PLATFORM_TEENSY)

#include "Wire.h"


class jI2C_Hard : public jI2C
{
protected:

	TwoWire* _i2c = &Wire;
	uint32_t _freq = I2C_FRQ_DEFAULT;

public:

	jI2C_Hard(TwoWire* I2C)
	{
		_i2c = I2C;
	}
	
	jI2C_Hard(TwoWire* I2C, int8_t pin_sda, int8_t pin_scl)
	{
		_i2c = I2C;
	}

	void begin()
	{
		_i2c->begin();
	}
	void end()
	{
		_i2c->end();
	}

	
	void setFrequency(uint32_t freq)
	{
		_freq = freq;
		_i2c->setClock(freq);
	}

	void beginTransmission(uint8_t addr)
	{
		_i2c->beginTransmission(addr);
	}
	uint8_t endTransmission(bool stopBit)
	{
		return _i2c->endTransmission(stopBit);
	}
	uint8_t endTransmission(void)
	{
				
		// Errors:
		//  0 : Success
		//  1 : Data too long
		//  2 : NACK on transmit of address
		//  3 : NACK on transmit of data
		//  4 : Other error
		return _i2c->endTransmission();
	}

	uint8_t requestFrom(uint8_t address, size_t quantity, bool stopBit)
	{
		//return _i2c->requestFrom(address, quantity, stopBit);
		return _i2c->requestFrom(address, (uint8_t) quantity, (uint8_t) stopBit);
	}
	uint8_t requestFrom(uint8_t address, size_t quantity)
	{
		return _i2c->requestFrom(address, quantity);
	}

	size_t write(uint8_t data)
	{
		return _i2c->write(data);
	}
	size_t write(const uint8_t * data, size_t quantity)
	{
		return _i2c->write(data, quantity);
	}

	uint8_t read(void)
	{
		return _i2c->read();
	}

	
	uint16_t available() override {
		return _i2c->available();
	}
	
};

#endif 


#ifdef PLATFORM_ESP32

#include "Wire.h"


class jI2C_Hard : public jI2C
{
protected:

	TwoWire* _i2c = &Wire;
	uint32_t _freq = I2C_FRQ_DEFAULT;

	int8_t _pin_scl = -1;
	int8_t _pin_sda = -1;

public:

	jI2C_Hard(TwoWire* I2C)
	{
		_i2c = I2C;
	}

	jI2C_Hard(TwoWire* I2C, int8_t pin_sda, int8_t pin_scl)
	{
		_i2c = I2C;
		_pin_sda = pin_sda;
		_pin_scl = pin_scl;
	}


	void begin()
	{
		if(_pin_sda == -1 && _pin_scl == -1 )
			_i2c->begin();
		else
			_i2c->begin(_pin_sda, _pin_scl);
	}
	void end()
	{
		_i2c->end();
	}

	
	void setFrequency(uint32_t freq)
	{
		_freq = freq;
		_i2c->setClock(freq);
	}

	void beginTransmission(uint8_t addr)
	{
		_i2c->beginTransmission(addr);
	}
	uint8_t endTransmission(bool stopBit)
	{
		return _i2c->endTransmission(stopBit);
	}
	uint8_t endTransmission(void)
	{
		return _i2c->endTransmission();
	}

	uint8_t requestFrom(uint8_t address, size_t quantity, bool stopBit)
	{
		return _i2c->requestFrom((uint16_t)address, (uint8_t) quantity, stopBit);
	}
	uint8_t requestFrom(uint8_t address, size_t quantity)
	{
		return _i2c->requestFrom(address, quantity);
	}

	size_t write(uint8_t data)
	{
		return _i2c->write(data);
	}
	size_t write(const uint8_t * data, size_t quantity)
	{
		return _i2c->write(data, quantity);
	}

	uint8_t read(void)
	{
		return _i2c->read();
	}

	uint16_t available() override {
		return _i2c->available();
	}
	
};

#endif // PLATFORM_ESP32


#ifdef PLATFORM_ESP8266

#include "Wire.h"


class jI2C_Hard : public jI2C
{
protected:

	TwoWire* _i2c = &Wire;
	uint32_t _freq = I2C_FRQ_DEFAULT;

	int8_t _pin_scl = -1;
	int8_t _pin_sda = -1;

public:

	jI2C_Hard(TwoWire* I2C)
	{
		_i2c = I2C;
	}

	jI2C_Hard(TwoWire* I2C, int8_t pin_sda, int8_t pin_scl)
	{
		_i2c = I2C;
		_pin_sda = pin_sda;
		_pin_scl = pin_scl;
	}


	void begin()
	{
		if(_pin_sda == -1 && _pin_scl == -1 )
			_i2c->begin();
		else
			_i2c->begin(_pin_sda, _pin_scl);
	}
	void end()
	{
		//_i2c->end(); // <- has no end()
	}

	
	void setFrequency(uint32_t freq)
	{
		_freq = freq;
		_i2c->setClock(freq);
	}

	void beginTransmission(uint8_t addr)
	{
		_i2c->beginTransmission(addr);
	}
	uint8_t endTransmission(bool stopBit)
	{
		return _i2c->endTransmission(stopBit);
	}
	uint8_t endTransmission(void)
	{
		return _i2c->endTransmission();
	}

	uint8_t requestFrom(uint8_t address, size_t quantity, bool stopBit)
	{
		return _i2c->requestFrom((int)address, (int) quantity, (int) stopBit);
	}
	uint8_t requestFrom(uint8_t address, size_t quantity)
	{
		return _i2c->requestFrom(address, quantity);
	}

	size_t write(uint8_t data)
	{
		return _i2c->write(data);
	}
	size_t write(const uint8_t * data, size_t quantity)
	{
		return _i2c->write(data, quantity);
	}

	uint8_t read(void)
	{
		return _i2c->read();
	}

	uint16_t available() override {
		return _i2c->available();
	}
	
};

#endif // PLATFORM_ESP8266


#if defined(VARIANTE_RP2040ALT)

#include "Wire.h"


class jI2C_Hard : public jI2C
{
protected:

	TwoWire* _i2c = &Wire;
	uint32_t _freq = I2C_FRQ_DEFAULT;

	int8_t _pin_sda = -1;
	int8_t _pin_scl = -1;

	bool begun = false;

public:

	jI2C_Hard(TwoWire* I2C)
	{
		_i2c = I2C;
	}
	
	jI2C_Hard(TwoWire* I2C, int8_t pin_sda, int8_t pin_scl)
		: _pin_sda(pin_sda), _pin_scl(pin_scl)
	{
		_i2c = I2C;
	}

	void begin()
	{
		if(begun)
			return;

		if(_pin_sda != -1)
			_i2c->setSDA(_pin_sda);
		if(_pin_scl != -1)
			_i2c->setSCL(_pin_scl);
		
		_i2c->begin();

		begun = true;
	}
	void end()
	{
		_i2c->end();

		begun = false;
	}

	
	void setFrequency(uint32_t freq)
	{
		_freq = freq;
		_i2c->setClock(freq);
	}

	void beginTransmission(uint8_t addr)
	{
		_i2c->beginTransmission(addr);
	}
	uint8_t endTransmission(bool stopBit)
	{
		return _i2c->endTransmission(stopBit);
	}
	uint8_t endTransmission(void)
	{
				
		// Errors:
		//  0 : Success
		//  1 : Data too long
		//  2 : NACK on transmit of address
		//  3 : NACK on transmit of data
		//  4 : Other error
		return _i2c->endTransmission();
	}

	uint8_t requestFrom(uint8_t address, size_t quantity, bool stopBit)
	{
		//return _i2c->requestFrom(address, quantity, stopBit);
		return _i2c->requestFrom(address, (uint8_t) quantity, (uint8_t) stopBit);
	}
	uint8_t requestFrom(uint8_t address, size_t quantity)
	{
		return _i2c->requestFrom(address, quantity);
	}

	size_t write(uint8_t data)
	{
		return _i2c->write(data);
	}
	size_t write(const uint8_t * data, size_t quantity)
	{
		return _i2c->write(data, quantity);
	}

	uint8_t read(void)
	{
		return _i2c->read();
	}

	
	uint16_t available() override {
		return _i2c->available();
	}
	
};

#endif 