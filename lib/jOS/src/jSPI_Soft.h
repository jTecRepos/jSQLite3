#pragma once


#include "jSPI.h"

class jSPI_Soft : public jSPI_Port_Base
{
protected:

	//uint8_t _pin_cs = -1;
	uint8_t pin_sck;
	uint8_t pin_miso; // set to -1 to disable MISO Pin (only data output mode)
	uint8_t pin_mosi;

	uint32_t _freq = SPI_FRQ_DEFAULT;
	SPI_BitOrder _bitOrder = SPI_BitOrder_MSBFIRST;
	SPI_DATAMODE _dataMode = SPI_DATAMODE0;


	uint8_t startbit;


public:

	jSPI_Soft(uint8_t sckpin, uint8_t misopin, uint8_t mosipin)
	{
		pin_sck = sckpin;
		pin_miso = misopin;
		pin_mosi = mosipin;
		//_pin_cs = pin_cs;
	}
	void begin()
	{
		//pinMode(_pin_cs, OUTPUT);
		//digitalWrite(_pin_cs, HIGH);

		
		pinMode(pin_sck, OUTPUT);


	}
	void end()
	{
		
	}

	void beginTransaction(uint32_t freq = SPI_FRQ_DEFAULT, SPI_BitOrder bitOrder = SPI_BitOrder_MSBFIRST, SPI_DATAMODE dataMode = SPI_DATAMODE0) override
	{
		_freq = freq;
		_bitOrder = bitOrder;
		_dataMode = dataMode;

		
		// Bit Order
		startbit = _bitOrder == SPI_BitOrder_LSBFIRST ? 0x01 : 0x80;

		// idle low on mode 0 and 1 | idle high on mode 2 or 3
		digitalWrite(pin_sck, ((_dataMode == SPI_DATAMODE0) || (_dataMode == SPI_DATAMODE1)) ? LOW : HIGH);
		
		
		pinMode(pin_mosi, OUTPUT);
		digitalWrite(pin_mosi, HIGH);
		
		if(pin_miso != -1)
			pinMode(pin_miso, INPUT);
		
		//if(_pin_cs >= 0)
		//	digitalWrite(_pin_cs, LOW);

		//if (pin_mosi != -1) 
		//	digitalWrite(pin_mosi, LOW);

	}
	void endTransaction(void)
	{
		//if(_pin_cs >= 0)
		//	digitalWrite(_pin_cs, HIGH);
			
		
		digitalWrite(pin_mosi, LOW);

	}
	
	uint8_t transfer(uint8_t data)
	{
		//LogD("transfer(%02X)\n",data);

		bool towrite;
		uint8_t bitdelay_us = (1000000 / _freq) / 2;
		
		//uint8_t _data = data;
		//transfer(&_data, 1);
		//return _data;

		// software SPI
		uint8_t reply = 0;
		uint8_t send = data;

		
		//String s = format("SPI TX: 0x%X", send);
		//s += " - [";
		
		// Send byte bit by bit
		for (uint8_t b = startbit; b != 0; b = (_bitOrder == SPI_BitOrder_LSBFIRST) ? b << 1 : b >> 1) 
		{

			if (bitdelay_us)
				delayMicroseconds(bitdelay_us);

			towrite = send & b;
			//s += (towrite ? "1" : "0");

			// DataMode SPI_MODE0 or SPI_MODE2
			if (_dataMode == SPI_DATAMODE0 || _dataMode == SPI_DATAMODE2) {					

				if (pin_mosi != -1) 
					digitalWrite(pin_mosi, towrite);

				digitalWrite(pin_sck, HIGH);

				if (bitdelay_us)
					delayMicroseconds(bitdelay_us);

				if (pin_miso != -1) {
					if (digitalRead(pin_miso))
						reply |= b;
				}

				digitalWrite(pin_sck, LOW);

			} else {  // DataMode SPI_MODE1 or SPI_MODE3

				digitalWrite(pin_sck, HIGH);

				if (bitdelay_us)
					delayMicroseconds(bitdelay_us);

				if (pin_mosi != -1)
					digitalWrite(pin_mosi, send & b);

				digitalWrite(pin_sck, LOW);


				if (pin_miso != -1) {
					if (digitalRead(pin_miso)) {
						reply |= b;
					}
				}
			}
		}

		//s += format("] -> 0x%X", reply, HEX);
		//LogD(s);
	
		

		return reply;
	}
		
	void transfer(uint8_t * data, uint32_t size)
	{
		for (size_t i = 0; i < size; i++) 
		{
			data[i] = transfer(data[i]);
		}
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