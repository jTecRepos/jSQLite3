#include "jDevice.h"


#include "jSystem.h"
#include "jHelp.h"


/************************************************
 *                   jDeviceBus                 *
 * **********************************************/


/*
bool jDeviceBus::read_bytes(uint8_t reg, uint8_t* data, uint8_t count)
{
	uint8_t read_count = bus_read_reg(reg, data, count);
	return read_count == count;
}


bool jDeviceBus::write_bytes(uint8_t reg, uint8_t* data, uint8_t count)
{
	return bus_write_reg(reg, data, count);
}
*/


uint16_t  jDeviceBus::read16(uint8_t reg) { 
	uint8_t data[2];
	bus_read_reg(reg, data, 2);
	return ((uint16_t)data[1] << 8) | data[0];
};


bool jDeviceBus::read16(uint8_t reg, uint16_t& res) { 
	
	uint8_t data[2];
	if(2 == bus_read_reg(reg, data, 2))
	{
		res = ((uint16_t)data[1] << 8) | data[0];
		return true;
	} else {
		return false;
	}
}


bool jDeviceBus::write16(uint8_t reg, uint16_t val) { 
	bus_write_reg(reg, (uint8_t*)&val, 2);
	return true;
};


uint8_t  jDeviceBus::read_reg(uint8_t reg) { 
	//uint8_t data[1];
	//bus_read_reg(reg, data, 1);
	//return data[0];
	return bus_read_reg(reg);
};


// read register with mask and return result like read_reg_mask(REGXY, (1 << 2) | (1 << 7))
uint8_t  jDeviceBus::read_reg_mask(uint8_t reg, uint8_t mask) { 
	uint8_t data[1];
	bus_read_reg(reg, data, 1);
	return data[0] & mask;
};

// read bit of register like read_reg_bit(REGXY, 2) to read 2th bit and check if is set
bool  jDeviceBus::read_reg_bit(uint8_t reg, uint8_t bit_index) { 

	uint8_t data[1];
	bus_read_reg(reg, data, 1);
	return (data[0] & (1 << bit_index)) != 0;
};


bool jDeviceBus::write_reg(uint8_t reg, uint8_t val) { 
	//bus_write_reg(reg, (uint8_t*)&val, 1);
	//return true;

	return bus_write_reg(reg, val);
};


// write register with mask and value like write_reg_mask(REGXY, 0x0F, 0x3) to overrite lower 4 bits with value 0011
// returns new register value
uint8_t  jDeviceBus::write_reg_mask(uint8_t reg, uint8_t mask, uint8_t value) { 
	value = (read_reg(reg) & ~mask) | (mask & value);
	write_reg(reg, value);
	return value;
};

// set bits in reg like write_reg_bit(X, (1 << 4)) to set 4th bit in reg X without clearing rest of reg
void jDeviceBus::write_reg_set_bit(uint8_t reg, uint8_t val)
{
	val |= read_reg(reg);
	write_reg(reg, val);
}

// set bits in reg like write_reg_bit(X, (1 << 4)) to clear 4th bit in reg X without clearing rest of reg
void jDeviceBus::write_reg_reset_bit(uint8_t reg, uint8_t val)
{
	val = ~val;
	val &= read_reg(reg);
	write_reg(reg, val);
}


void jDeviceBus::printReg(const String& tag, uint8_t reg)
{
	uint8_t val = read_reg(reg);

	//LogD("%s %s", name, sprintBits(val).c_str());
	LogD("%s.reg: %45s=%02X - b%s", name(), 
		tag.c_str(), 
		val, 
		sprintBits(val).c_str()
	);
}

void jDeviceBus::printRegBit(const String& tag, uint8_t reg, uint8_t bit)
{
	uint8_t val = read_reg(reg);
	bool bit_val = (val & (1 << bit)) != 0;
	
	LogD("%s.reg: %45s= b%s -> %d", name(), 
		tag.c_str(), 
		sprintBits(val).c_str(), 
		bit_val
	);
}



/************************************************
 *                   jDeviceI2C                 *
 * **********************************************/




bool jDeviceI2C::i2c_write_reg(uint8_t address, uint8_t reg, uint8_t data)
{
	_i2c->beginTransmission(address);	// Initialize the Tx buffer
	_i2c->write(reg);					// Put slave register address in Tx buffer
	_i2c->write(data);					// Put data in Tx buffer
	_i2c->endTransmission();			// Send the Tx buffer
	return true;
}


bool jDeviceI2C::i2c_write_reg(uint8_t address, uint8_t reg, uint8_t* data, uint8_t count)
{

	_i2c->beginTransmission(address);	// Initialize the Tx buffer
	_i2c->write(reg);					// Put slave register address in Tx buffer
	//for(int i = 0; i < count; i++)
	//	_i2c->write(data[i]);
	_i2c->write(data, count);
	_i2c->endTransmission();			// Send the Tx buffer
	return true;

}

uint8_t jDeviceI2C::i2c_read_reg(uint8_t address, uint8_t reg)
{
	uint8_t data = 0;							// 'data' will store the register data	
	_i2c->beginTransmission(address);			// Initialize the Tx buffer
	_i2c->write(reg);							// Put slave register address in Tx buffer
	_i2c->endTransmission(false);				// Send the Tx buffer, but send a restart to keep connection alive
	_i2c->requestFrom(address, (uint8_t) 1);	// Read one byte from slave register address 

	data = _i2c->read();						// Fill Rx buffer with result

	_i2c->endTransmission();
	
	return data;								// Return data read from slave register
}

uint8_t jDeviceI2C::i2c_read_reg(uint8_t address, uint8_t reg, uint8_t* dest, uint8_t count)
{
	_i2c->beginTransmission(address);				// Initialize the Tx buffer
	_i2c->write(reg);								// Put slave register address in Tx buffer
	last_bus_error = _i2c->endTransmission(false);	// Send Tx buffer, send a restart to keep connection alive
	if (last_bus_error != 0)						// endTransmission should return 0 on success
		return 0;

	uint8_t retVal = _i2c->requestFrom(address, count);	// Read bytes from slave register address 
	
	if (retVal != count)
	{
		LogD("%s.i2c.err: i2c_read_reg - NACK", _name.c_str());
		last_bus_error = NACK_ON_DATA;
		return 0;
	}

	//for (int i = 0; i < count; i++)
	//	dest[i] = _i2c->read();
	_i2c->read(dest, count);

	_i2c->endTransmission();

	return count;
}



bool jDeviceI2C::bus_write_reg(uint8_t reg, uint8_t data) {
	return i2c_write_reg(DEVICE_ADDR, reg, data);
}
bool jDeviceI2C::bus_write_reg(uint8_t reg, uint8_t* data, uint8_t count) {
	return i2c_write_reg(DEVICE_ADDR, reg, data, count);
}
uint8_t jDeviceI2C::bus_read_reg(uint8_t reg) {
	return i2c_read_reg(DEVICE_ADDR, reg);
}
uint8_t jDeviceI2C::bus_read_reg(uint8_t reg, uint8_t* dest, uint8_t count) {
	return i2c_read_reg(DEVICE_ADDR, reg, dest, count);
}




/************************************************
 *                   jDeviceSPI                 *
 * **********************************************/




uint8_t jDeviceSPI::spi_transfer_data(uint8_t data) {
	
	_jSPI->beginTransaction();

	uint8_t val = _jSPI->transfer(data);
	
	_jSPI->endTransaction();

	return val;

}

void jDeviceSPI::spi_transfer_data(uint8_t* data, uint8_t count) {
	
	_jSPI->beginTransaction();

	
	//for(int i = 0; i < count; i++)
	//	data[i] = _jSPI->transfer(data[i]);
	_jSPI->transfer(data, count);
	
	_jSPI->endTransaction();
	
}

void jDeviceSPI::spi_transfer_data(uint8_t* data_tx, uint16_t len_tx, uint8_t* data_rx, uint16_t len_rx) {
	
	_jSPI->beginTransaction();


	//for(int i = 0; i < len_tx; i++)
	//	_jSPI->transfer(data_tx[i]);
	_jSPI->transfer(data_tx, len_tx);
	
	for(int i = 0; i < len_rx; i++)
		data_rx[i] = _jSPI->transfer(0);	//TODO maybe better to send 0xFF?
	
	_jSPI->endTransaction();
	
}


bool jDeviceSPI::spi_write_reg(uint8_t reg, uint8_t data) {
	
	_jSPI->beginTransaction();


	// Write Operation: (0)(Addr)
	_jSPI->transfer(((reg & 0x7F) | 0x00));
	_jSPI->transfer(data);
	
	_jSPI->endTransaction();
	

	return true;
}

bool jDeviceSPI::spi_write_reg(uint8_t reg, uint8_t* data, uint8_t count) {
	
	_jSPI->beginTransaction();


	// Write Operation: (0)(Addr)
	_jSPI->transfer(((reg & 0x7F) | 0x00));
	
	//for(int i = 0; i < count; i++)
	//	_jSPI->transfer(data[i]);
	_jSPI->transfer(data, count);
	
	_jSPI->endTransaction();
	

	return true;
}

uint8_t jDeviceSPI::spi_read_reg(uint8_t reg) {
		
	_jSPI->beginTransaction();


	// Read Operation: (1)(Addr)
	_jSPI->transfer(((reg & 0x7F) | 0x80));
	uint8_t val = _jSPI->transfer(0x00);
	
	_jSPI->endTransaction();
	

	return val;
}

uint8_t jDeviceSPI::spi_read_reg(uint8_t reg, uint8_t* dest, uint8_t count) {
	
	_jSPI->beginTransaction();


	// Read Operation: (1)(Addr)
	_jSPI->transfer(((reg & 0x7F) | 0x80));

	for(int i = 0; i < count; i++)
		dest[i] = _jSPI->transfer(0x00);
	
	_jSPI->endTransaction();
	

	return count;
}



bool jDeviceSPI::bus_write_reg(uint8_t reg, uint8_t data) {
	return spi_write_reg(reg, data);
}
bool jDeviceSPI::bus_write_reg(uint8_t reg, uint8_t* data, uint8_t count) {
	return spi_write_reg(reg, data, count);
}
uint8_t jDeviceSPI::bus_read_reg(uint8_t reg) {
	return spi_read_reg(reg);
}
uint8_t jDeviceSPI::bus_read_reg(uint8_t reg, uint8_t* dest, uint8_t count) {
	return spi_read_reg(reg, dest, count);
}





