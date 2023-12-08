#pragma once

#include "jDefines.h"


#define SPI_FRQ_10MHz 10E6
#define SPI_FRQ_8MHz 8E6
#define SPI_FRQ_7MHz 7E6
#define SPI_FRQ_4MHz 4E6
#define SPI_FRQ_1MHz 1E6
#define SPI_FRQ_500kHz 500E3
#define SPI_FRQ_100kHz 100E3
#define SPI_FRQ_10kHz 10E3


#define SPI_FRQ_DEFAULT SPI_FRQ_1MHz


enum SPI_BitOrder {
	SPI_BitOrder_LSBFIRST = 0,
	SPI_BitOrder_MSBFIRST = 1
};


// Mode          Clock Polarity (CPOL)   Clock Phase (CPHA)
// SPI_MODE0             0                     0
// SPI_MODE1             0                     1
// SPI_MODE2             1                     0
// SPI_MODE3             1                     1
enum SPI_DATAMODE {
	SPI_DATAMODE0 = 0x00,
	SPI_DATAMODE1 = 0x01,
	SPI_DATAMODE2 = 0x02,
	SPI_DATAMODE3 = 0x03
};

class jSPI_Port_Base {

public:
	
	jSPI_Port_Base() { }

	virtual void begin() = 0;
	virtual void end() = 0;

	
	virtual void beginTransaction(uint32_t _freq = SPI_FRQ_DEFAULT, SPI_BitOrder _bitOrder = SPI_BitOrder_MSBFIRST, SPI_DATAMODE _dataMode = SPI_DATAMODE0) = 0;
	virtual void endTransaction(void) = 0;
	virtual void transfer(uint8_t * data, uint32_t size) = 0;
	virtual uint8_t transfer(uint8_t data) = 0;

	virtual void init_CS(uint8_t pin) = 0;
	virtual void setCS(uint8_t pin, bool state) = 0;
};

class jSPI
{
protected:
	jSPI_Port_Base* _jspi_port;
	uint8_t _pin_cs = -1;

	uint32_t _freq = SPI_FRQ_DEFAULT;
	SPI_BitOrder _bitOrder = SPI_BitOrder_MSBFIRST;
	SPI_DATAMODE _dataMode = SPI_DATAMODE0;

public:
	jSPI(jSPI_Port_Base* jspi_port, uint8_t pin_cs)
		: _jspi_port(jspi_port), _pin_cs(pin_cs)
	{

	}

	virtual void begin() {
		_jspi_port->init_CS(_pin_cs);
	}

	virtual void end() {

	}	
	
	void setBitOrder(SPI_BitOrder bitOrder)
	{
		_bitOrder = bitOrder;
	}
	void setDataMode(SPI_DATAMODE dataMode)
	{
		_dataMode = dataMode;
	}
	void setFrequency(uint32_t freq)
	{
		_freq = freq;
	}
	

	virtual void beginTransaction() {
		
		_jspi_port->beginTransaction(
			_freq, 
			_bitOrder, 
			_dataMode
		);
		
		setCS(LOW);
	}

	virtual void endTransaction(void) {
		
		setCS(HIGH);

		_jspi_port->endTransaction();
		
	}

	virtual void transfer(uint8_t* data, uint32_t size) {
		_jspi_port->transfer(data, size);
	}

	virtual uint8_t transfer(uint8_t data) {
		return _jspi_port->transfer(data);
	}

	virtual void setCS(bool state) {
		_jspi_port->setCS(_pin_cs, state);
	}



		
	/*!
	*    @brief  Write a buffer or two to the SPI device, with transaction
	* management.
	*    @param  buffer Pointer to buffer of data to write
	*    @param  len Number of bytes from buffer to write
	*    @param  prefix_buffer Pointer to optional array of data to write before
	* buffer.
	*    @param  prefix_len Number of bytes from prefix buffer to write
	*    @return Always returns true because there's no way to test success of SPI
	* writes
	*/
	virtual bool write(const uint8_t *buffer, uint16_t len,
							const uint8_t *prefix_buffer,
							uint16_t prefix_len
	) 
	{
		beginTransaction();

		// do the writing
		{
			for (uint16_t i = 0; i < prefix_len; i++) {
				transfer(prefix_buffer[i]);
			}
			for (uint16_t i = 0; i < len; i++) {
				transfer(buffer[i]);
			}
		}
		
		endTransaction();

		return true;
	}



	/*!
	*    @brief  Write some data, then read some data from SPI into another buffer,
	* with transaction management. The buffers can point to same/overlapping
	* locations. This does not transmit-receive at the same time!
	*    @param  write_buffer Pointer to buffer of data to write from
	*    @param  write_len Number of bytes from buffer to write.
	*    @param  read_buffer Pointer to buffer of data to read into.
	*    @param  read_len Number of bytes from buffer to read.
	*    @param  sendvalue The 8-bits of data to write when doing the data read,
	* defaults to 0xFF
	*    @return Always returns true because there's no way to test success of SPI
	* writes
	*/
	bool write_then_read(const uint8_t *write_buffer,
							uint16_t write_len, uint8_t *read_buffer,
							uint16_t read_len, uint8_t sendvalue
	) 
	{
		beginTransaction();
		
		// do the writing
		for (uint16_t i = 0; i < write_len; i++)
			transfer(write_buffer[i]);


		// do the reading
		for (uint16_t i = 0; i < read_len; i++)
			read_buffer[i] = transfer(sendvalue);

		endTransaction();

		return true;
	}
};



/**************************************************************************/



#ifdef PLATFORM_AVR

#include "SPI.h"

class jSPI_Hard : public jSPI_Port_Base
{
protected:

	SPIClass* _spi = &SPI;


public:

	jSPI_Hard(SPIClass* _SPI)
	{
		_spi = _SPI;
	}
	jSPI_Hard(SPIClass* _SPI, int8_t sck, int8_t miso, int8_t mosi)
	{
		//Variable PINS are not supported
		_spi = _SPI;
	}

	void begin()
	{
		_spi->begin();
	}

	void end()
	{
		_spi->end();
	}
	
	void beginTransaction(uint32_t _freq = SPI_FRQ_DEFAULT, SPI_BitOrder _bitOrder = SPI_BitOrder_MSBFIRST, SPI_DATAMODE _dataMode = SPI_DATAMODE0) override
	{
		//if(_pin_cs >= 0)
		//	digitalWrite(_pin_cs, LOW);

				
		//#define LSBFIRST 0
		//#define MSBFIRST 1

		//#define SPI_MODE0 0x00
		//#define SPI_MODE1 0x04
		//#define SPI_MODE2 0x08
		//#define SPI_MODE3 0x0C

		uint8_t dm = SPI_MODE0;
		switch(_dataMode)
		{
			case SPI_DATAMODE0: dm = SPI_MODE0; break;
			case SPI_DATAMODE1: dm = SPI_MODE1; break;
			case SPI_DATAMODE2: dm = SPI_MODE2; break;
			case SPI_DATAMODE3: dm = SPI_MODE3; break;
		}

		_spi->beginTransaction(
			SPISettings(
				_freq, 
				(uint8_t) _bitOrder, 
				dm
			)
		);

	}
	void endTransaction(void)
	{
		_spi->endTransaction();
		
		//if(_pin_cs >= 0)
		//	digitalWrite(_pin_cs, HIGH);
	}
	
	uint8_t transfer(uint8_t data)
	{
		return _spi->transfer(data);
	}

	void transfer(uint8_t * data, uint32_t size)
	{
		_spi->transfer(data, size);
	}

	void init_CS(uint8_t pin) override
	{
		pinMode(pin, OUTPUT);
		digitalWrite(pin, HIGH);
	}
	
	void setCS(uint8_t pin, bool state) override {
		if(pin >= 0)
			digitalWrite(pin, state);
	}
	
};

#endif // PLATFORM_AVR


#ifdef PLATFORM_TEENSY

#include "SPI.h"

class jSPI_Hard : public jSPI_Port_Base
{
protected:

	SPIClass* _spi = &SPI;


public:

	jSPI_Hard(SPIClass* _SPI)
	{
		_spi = _SPI;
	}
	jSPI_Hard(SPIClass* _SPI, int8_t sck, int8_t miso, int8_t mosi)
	{
		//Variable PINS are not supported
		_spi = _SPI;
	}

	void begin()
	{
		_spi->begin();
	}

	void end()
	{
		_spi->end();
	}
	
	void beginTransaction(uint32_t _freq = SPI_FRQ_DEFAULT, SPI_BitOrder _bitOrder = SPI_BitOrder_MSBFIRST, SPI_DATAMODE _dataMode = SPI_DATAMODE0) override
	{
		//if(_pin_cs >= 0)
		//	digitalWrite(_pin_cs, LOW);

				
		//#define LSBFIRST 0
		//#define MSBFIRST 1

		//#define SPI_MODE0 0x00
		//#define SPI_MODE1 0x04
		//#define SPI_MODE2 0x08
		//#define SPI_MODE3 0x0C

		uint8_t dm = SPI_MODE0;
		switch(_dataMode)
		{
			case SPI_DATAMODE0: dm = SPI_MODE0; break;
			case SPI_DATAMODE1: dm = SPI_MODE1; break;
			case SPI_DATAMODE2: dm = SPI_MODE2; break;
			case SPI_DATAMODE3: dm = SPI_MODE3; break;
		}

		_spi->beginTransaction(
			SPISettings(
				_freq, 
				(uint8_t) _bitOrder, 
				dm
			)
		);

	}
	void endTransaction(void)
	{
		_spi->endTransaction();
		
		//if(_pin_cs >= 0)
		//	digitalWrite(_pin_cs, HIGH);
	}
	
	uint8_t transfer(uint8_t data)
	{
		return _spi->transfer(data);
	}

	void transfer(uint8_t * data, uint32_t size)
	{
		_spi->transfer(data, size);
	}

	void init_CS(uint8_t pin) override
	{
		pinMode(pin, OUTPUT);
		digitalWrite(pin, HIGH);
	}
	
	void setCS(uint8_t pin, bool state) override {
		if(pin >= 0)
			digitalWrite(pin, state);
	}
	
};

#endif // PLATFORM_TEENSY


#ifdef PLATFORM_SAMD

#include "SPI.h"

class jSPI_Hard : public jSPI_Port_Base
{
protected:

	SPIClass* _spi = &SPI;


public:

	jSPI_Hard(SPIClass* _SPI)
	{
		_spi = _SPI;
	}

	jSPI_Hard(SPIClass* _SPI, int8_t sck, int8_t miso, int8_t mosi)
	{
		//Variable PINS are not supported
		_spi = _SPI;
	}


	void begin()
	{
		_spi->begin();
	}

	void end()
	{
		_spi->end();
	}
	
	void beginTransaction(uint32_t _freq = SPI_FRQ_DEFAULT, SPI_BitOrder _bitOrder = SPI_BitOrder_MSBFIRST, SPI_DATAMODE _dataMode = SPI_DATAMODE0) override
	{
#ifdef SERCOM_SPI_FREQ_REF
		SPI.setClockDivider(SERCOM_SPI_FREQ_REF / _freq);
#endif

		uint8_t dm = SPI_MODE0;
		switch(_dataMode)
		{
			case SPI_DATAMODE0: dm = SPI_MODE0; break;
			case SPI_DATAMODE1: dm = SPI_MODE1; break;
			case SPI_DATAMODE2: dm = SPI_MODE2; break;
			case SPI_DATAMODE3: dm = SPI_MODE3; break;
		}

		_spi->beginTransaction(SPISettings(
					_freq, 
					_bitOrder == SPI_BitOrder_MSBFIRST ? MSBFIRST : LSBFIRST,
					dm
				)
			);

	}
	void endTransaction(void)
	{
		_spi->endTransaction();
	}
	
	uint8_t transfer(uint8_t data)
	{
		return _spi->transfer(data);
	}

	void transfer(uint8_t * data, uint32_t size)
	{
		_spi->transfer(data, size);
	}

	void init_CS(uint8_t pin) override
	{
		pinMode(pin, OUTPUT);
		digitalWrite(pin, HIGH);
	}
	
	void setCS(uint8_t pin, bool state) override {
		if(pin >= 0)
			digitalWrite(pin, state);
	}
	
};

#endif // PLATFORM_SAMD


#ifdef PLATFORM_ESP32

#include "SPI.h"


class jSPI_Hard : public jSPI_Port_Base
{
protected:

	SPIClass* _spi = &SPI;

	int8_t pin_sck = -1;
	int8_t pin_miso = -1;
	int8_t pin_mosi = -1;


public:

	jSPI_Hard(SPIClass* _SPI)
	{
		_spi = _SPI;
	}
	
	jSPI_Hard(SPIClass* _SPI, int8_t sck, int8_t miso, int8_t mosi)
	 : pin_sck(sck), pin_miso(miso), pin_mosi(mosi)
	{
		_spi = _SPI;
	}

	void begin(int8_t sck, int8_t miso, int8_t mosi)
	{
		this->pin_sck = sck;
		this->pin_miso = miso;
		this->pin_mosi = mosi;
		//_spi->begin(sck, miso, mosi);
		
		this->begin();
	}

	void begin()
	{
		//_spi->begin();
		_spi->begin(pin_sck, pin_miso, pin_mosi);
	}

	void end()
	{
		_spi->end();
	}
	
	void beginTransaction(uint32_t _freq = SPI_FRQ_DEFAULT, SPI_BitOrder _bitOrder = SPI_BitOrder_MSBFIRST, SPI_DATAMODE _dataMode = SPI_DATAMODE0) override
	{
		_spi->beginTransaction(SPISettings(_freq, (uint8_t) _bitOrder, (uint8_t) _dataMode));
	}
	void endTransaction(void)
	{
		_spi->endTransaction();
	}
	
	uint8_t transfer(uint8_t data)
	{
		return _spi->transfer(data);
	}

	void transfer(uint8_t * data, uint32_t size)
	{
		_spi->transfer(data, size);
	}

	void init_CS(uint8_t pin) override
	{
		pinMode(pin, OUTPUT);
		digitalWrite(pin, HIGH);
	}
	
	void setCS(uint8_t pin, bool state) override {
		if(pin >= 0)
			digitalWrite(pin, state);
	}
	
};


#endif // PLATFORM_ESP32


#ifdef PLATFORM_ESP8266

#include "SPI.h"


class jSPI_Hard : public jSPI_Port_Base
{
protected:

	SPIClass* _spi = &SPI;

	int8_t pin_sck = -1;
	int8_t pin_miso = -1;
	int8_t pin_mosi = -1;


public:

	jSPI_Hard(SPIClass* _SPI)
	{
		_spi = _SPI;
	}
	
	jSPI_Hard(SPIClass* _SPI, int8_t sck, int8_t miso, int8_t mosi)
	 : pin_sck(sck), pin_miso(miso), pin_mosi(mosi)
	{
		_spi = _SPI;
	}

	void begin(int8_t sck, int8_t miso, int8_t mosi)
	{
		this->pin_sck = sck;
		this->pin_miso = miso;
		this->pin_mosi = mosi;
		//_spi->begin(sck, miso, mosi);
		
		this->begin();
	}

	void begin()
	{
		_spi->begin();
		//_spi->pins(pin_sck, pin_miso, pin_mosi, 0);
	}

	void end()
	{
		_spi->end();
	}
	
	void beginTransaction(uint32_t _freq = SPI_FRQ_DEFAULT, SPI_BitOrder _bitOrder = SPI_BitOrder_MSBFIRST, SPI_DATAMODE _dataMode = SPI_DATAMODE0) override
	{
		_spi->beginTransaction(SPISettings(_freq, (uint8_t) _bitOrder, (uint8_t) _dataMode));
	}
	void endTransaction(void)
	{
		_spi->endTransaction();
	}
	
	uint8_t transfer(uint8_t data)
	{
		return _spi->transfer(data);
	}

	void transfer(uint8_t * data, uint32_t size)
	{
		_spi->transfer(data, size);
	}

	void init_CS(uint8_t pin) override
	{
		pinMode(pin, OUTPUT);
		digitalWrite(pin, HIGH);
	}
	
	void setCS(uint8_t pin, bool state) override {
		if(pin >= 0)
			digitalWrite(pin, state);
	}
	
};


#endif // PLATFORM_ESP8266


#ifdef PLATFORM_STM32

#include "SPI.h"

class jSPI_Hard : public jSPI_Port_Base
{
protected:

	SPIClass* _spi = &SPI;


public:

	jSPI_Hard(SPIClass* _SPI)
	{
		_spi = _SPI;
	}
	
	jSPI_Hard(SPIClass* _SPI, int8_t sck, int8_t miso, int8_t mosi)
	{
		//Variable PINS are not supported (check if this is true)
		_spi = _SPI;
	}


	void begin()
	{
		_spi->begin();
	}

	void end()
	{
		_spi->end();
	}
	
	void beginTransaction(uint32_t _freq = SPI_FRQ_DEFAULT, SPI_BitOrder _bitOrder = SPI_BitOrder_MSBFIRST, SPI_DATAMODE _dataMode = SPI_DATAMODE0) override
	{
		
		_spi->beginTransaction(SPISettings(
				_freq, 
				_bitOrder == SPI_BitOrder_MSBFIRST ? MSBFIRST : LSBFIRST,
				(uint8_t) _dataMode)
			);

	}
	void endTransaction(void)
	{
		_spi->endTransaction();
	}
	
	uint8_t transfer(uint8_t data)
	{
		return _spi->transfer(data);
	}

	void transfer(uint8_t * data, uint32_t size)
	{
		_spi->transfer(data, size);
	}

	void init_CS(uint8_t pin) override
	{
		pinMode(pin, OUTPUT);
		digitalWrite(pin, HIGH);
	}
	
	void setCS(uint8_t pin, bool state) override {
		if(pin >= 0)
			digitalWrite(pin, state);
	}
	
};


#endif // PLATFORM_STM32


#ifdef PLATFORM_RP2040

#include "SPI.h"

#ifdef VARIANTE_RP2040ALT
	#define _SPIClass SPIClassRP2040
	
#else
	#define _SPIClass SPIClass
#endif 

class jSPI_Hard : public jSPI_Port_Base
{
protected:
	_SPIClass* _spi = &SPI;

	int8_t _pin_sck = -1;
	int8_t _pin_miso = -1; 
	int8_t _pin_mosi = -1;

public:

	jSPI_Hard(_SPIClass* _SPI)
	{
		_spi = _SPI;
	}
	
	jSPI_Hard(_SPIClass* _SPI, uint8_t _pin_sck, uint8_t _pin_miso, uint8_t _pin_mosi)
	: _pin_sck(_pin_sck), _pin_miso(_pin_miso), _pin_mosi(_pin_mosi)
	{
		_spi = _SPI;
	}

	void begin()
	{
#ifdef VARIANTE_RP2040ALT
		if(_pin_sck != -1)
			_spi->setSCK(_pin_sck);
		if(_pin_mosi != -1)
			_spi->setTX(_pin_mosi);
		if(_pin_miso != -1)
			_spi->setRX(_pin_miso);
#endif // VARIANTE_RP2040ALT

		_spi->begin();
	}

	void end()
	{
		_spi->end();
	}
	
	void beginTransaction(uint32_t _freq = SPI_FRQ_DEFAULT, SPI_BitOrder _bitOrder = SPI_BitOrder_MSBFIRST, SPI_DATAMODE _dataMode = SPI_DATAMODE0) override
	{
		_spi->beginTransaction(SPISettings(_freq, (BitOrder) _bitOrder, (uint8_t) _dataMode));
	}
	void endTransaction(void)
	{
		_spi->endTransaction();
	}
	
	uint8_t transfer(uint8_t data)
	{
		return _spi->transfer(data);
	}

	void transfer(uint8_t * data, uint32_t size)
	{
		_spi->transfer(data, size);
	}

	void init_CS(uint8_t pin) override
	{
		pinMode(pin, OUTPUT);
		digitalWrite(pin, HIGH);
	}
	
	void setCS(uint8_t pin, bool state) override {
		if(pin >= 0)
			digitalWrite(pin, state);
	}
	
};

#endif // PLATFORM_RP2040

