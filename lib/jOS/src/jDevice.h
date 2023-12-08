#pragma once

#include "jDefines.h"



class jDevice
{
protected: 

	String _name = "";
	bool initialized = false;

public :

	jDevice() {
		
	}

	jDevice(const String& name)
	{
		this->_name = name;
	}

	virtual bool begin()
	{
		initialized = true;
		return true;
	}

	virtual void end() { }

	virtual void update() { }

	//virtual void reset() { }
	

	const char* name() { return this->_name.c_str(); }

	String print()
	{
		return print(this->_name);
	}

	virtual String print(const String& tag) { return ""; }

};


//Bus based sensor connected over I2C and SPI communcation based on register read and writes
class jDeviceBus : public jDevice {
	
protected:
	
	// write a single byte to a given register
	virtual bool bus_write_reg(uint8_t reg, uint8_t data) = 0;
	
	// write multiple bytes starting at given register
	virtual bool bus_write_reg(uint8_t reg, uint8_t* data, uint8_t count) = 0;
	
	// read the value of the given register
	virtual uint8_t bus_read_reg(uint8_t reg) = 0;
	
	// read the values starting from the given register
	// returns amount of bytes red
	virtual uint8_t bus_read_reg(uint8_t reg, uint8_t* dest, uint8_t count)  = 0;

	//virtual bool read_bytes(uint8_t reg, uint8_t* data, uint8_t count);
	//virtual bool write_bytes(uint8_t reg, uint8_t* data, uint8_t count);

	virtual uint16_t read16(uint8_t reg);
	virtual bool read16(uint8_t reg, uint16_t& res);
	virtual bool write16(uint8_t reg, uint16_t val);

	virtual uint8_t read_reg(uint8_t reg);
	virtual bool write_reg(uint8_t reg, uint8_t val);

	virtual uint8_t read_reg_mask(uint8_t reg, uint8_t mask);
	virtual bool read_reg_bit(uint8_t reg, uint8_t bit_index);

	virtual uint8_t write_reg_mask(uint8_t reg, uint8_t mask, uint8_t value);
	virtual void write_reg_set_bit(uint8_t reg, uint8_t val);
	virtual void write_reg_reset_bit(uint8_t reg, uint8_t val);
		
	virtual void printReg(const String& tag, uint8_t reg);

	virtual void printRegBit(const String& tag, uint8_t reg, uint8_t bit);

public:
	
	jDeviceBus(const String& name)
		:jDevice(name)
	{

	};

};

#include "jI2C.h"

class jDeviceI2C : public jDeviceBus {

protected:
	
	jI2C* _i2c;
	uint8_t DEVICE_ADDR; // 7-bit i2c-address

	// i2c specific regsiter read and write functions. override these if sensor has other implementation
	virtual bool i2c_write_reg(uint8_t address, uint8_t reg, uint8_t data);
	virtual bool i2c_write_reg(uint8_t address, uint8_t reg, uint8_t* data, uint8_t count);
	virtual uint8_t i2c_read_reg(uint8_t address, uint8_t reg);
	virtual uint8_t i2c_read_reg(uint8_t address, uint8_t reg, uint8_t* dest, uint8_t count);
	
	// implements bus register read/write using i2c_XXX_reg funktions
	bool bus_write_reg(uint8_t reg, uint8_t data);
	bool bus_write_reg(uint8_t reg, uint8_t* data, uint8_t count);
	uint8_t bus_read_reg(uint8_t reg);
	uint8_t bus_read_reg(uint8_t reg, uint8_t* dest, uint8_t count);
	

	uint8_t last_bus_error;

public:

	jDeviceI2C(const String& name, jI2C* _ji2c, uint8_t addr)
	: jDeviceBus(name)
	{
		_i2c = _ji2c;
		DEVICE_ADDR = addr;
	}

	bool begin() { return jDeviceBus::begin();	}

	
	enum BUS_ERROR_TYPES {
		BUS_NO_ERROR,
		DATA_TO_LONG,
		NACK_ON_ADDR,
		NACK_ON_DATA,
		OTHER
	};

	// If any library command fails, you can retrieve an extended
	// error code using this command. Errors are from the wire library: 
	// 0 = Success
	// 1 = Data too long to fit in transmit buffer
	// 2 = Received NACK on transmit of address
	// 3 = Received NACK on transmit of data
	// 4 = Other error
	uint8_t getBusError() { return last_bus_error; }

};





#include "jSPI.h"


class jDeviceSPI : public jDeviceBus {

protected:

	jSPI* _jSPI;

	// spi helper functions to send/transfer raw data
	void spi_transfer_data(uint8_t* data, uint8_t count); //transfers bytes in data and exchanges it with received bytes
	uint8_t spi_transfer_data(uint8_t data); //sends a single byte over spi and returns received byte
	void spi_transfer_data(uint8_t* data_tx, uint16_t len_tx, uint8_t* data_rx, uint16_t len_rx); //First sends TX data then reads RX data

	// spi specific regsiter read and write functions. override these if sensor has other implementation
	virtual bool spi_write_reg(uint8_t reg, uint8_t data);
	virtual bool spi_write_reg(uint8_t reg, uint8_t* data, uint8_t count);
	virtual uint8_t spi_read_reg(uint8_t reg);
	virtual uint8_t spi_read_reg(uint8_t reg, uint8_t* dest, uint8_t count);
	
	// implements bus register read/write using spi_XXX_reg funktions
	bool bus_write_reg(uint8_t reg, uint8_t data);
	bool bus_write_reg(uint8_t reg, uint8_t* data, uint8_t count);
	uint8_t bus_read_reg(uint8_t reg);
	uint8_t bus_read_reg(uint8_t reg, uint8_t* dest, uint8_t count);

	uint8_t last_bus_error;

public:

	jDeviceSPI(const String& name, jSPI* _jspi)
		: jDeviceBus(name)
	{
		this->_jSPI = _jspi;
	}

	virtual bool begin() { return jDeviceBus::begin(); }


};
