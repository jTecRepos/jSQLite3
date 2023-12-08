#pragma once
#include "jSerial.h"



#ifdef PLATFORM_RP2040

#ifdef USE_TINYUSB
	#define _SerialUSB Adafruit_USBD_CDC
#else
	#define _SerialUSB SerialUSB
#endif 


class jSerialHard : public jSerial
{
protected:
	
	Stream* stream = nullptr;
#ifdef VARIANTE_RP2040MBED
	UART* ser = nullptr;
	bool _ser_created = false;
#else 
	SerialUART* ser = nullptr;
	_SerialUSB* serUSB = nullptr;
	bool is_USB = false;
	int8_t txPin = -1; int8_t rxPin = -1;
#endif 
	
	uint32_t _baud = SERIAL_BAUD_115200;
	bool is_open = false;	

	

public:
#ifdef VARIANTE_RP2040MBED
	
	jSerialHard(UART& serial) 
	: ser(&serial)
	{ 
		stream = ser;
	}
	
	jSerialHard(int8_t txPin, int8_t rxPin) 
	{ 
		ser = new UART(txPin, rxPin);
		stream = ser;
		_ser_created = true;
	}
#else
	jSerialHard(_SerialUSB& serial) 
	: serUSB(&serial)
	{ 
		stream = serUSB;
		is_USB = true;
	}

	jSerialHard(SerialUART& serial) 
	: ser(&serial)
	{ 
		stream = ser;
	}
	
	jSerialHard(SerialUART& serial, int8_t txPin, int8_t rxPin) 
	: ser(&serial), txPin(txPin), rxPin(rxPin)
	{
		
		stream = ser;
	}
#endif 

	~jSerialHard();


	void begin() {
#ifdef VARIANTE_RP2040ALT
		if(rxPin != -1)
			ser->setRX(rxPin);
		if(txPin != -1)
			ser->setTX(txPin);
#endif // VARIANTE_RP2040ALT

		this->begin(115200);
	}
	
	void begin(uint32_t baud) {
#ifdef VARIANTE_RP2040MBED
		ser->begin(baud);
#else
		if(is_USB)
			serUSB->begin(baud);
		else
			ser->begin(baud);
#endif

		_baud = baud;
		is_open = true;
	}

	void end() 
	{
#ifdef VARIANTE_RP2040MBED
		ser->end();
#else
		if(is_USB)
			serUSB->end();
		else
			ser->end();
#endif

		is_open = false;
	}

	
	bool isOpen() {
		return is_open;
	}


	void updateBaudRate(uint32_t baud)
	{
		this->begin(baud);
	}
	
	uint32_t baudRate() {
		return _baud;
	}

	int available(void) {
		return stream->available();
	}
	
	int peek(void) override {
		return stream->peek();
	}

	// read single byte from serial input / rx buffer
	int read(void) {
		return stream->read();
	}
	
	// writes tx buffer out
	void flush(void) {
		stream->flush();
	}
	
	// writes to Serial. 
	// returns length of written. 0 if failed. 1 if success
	size_t write(uint8_t c) {
		return stream->write(c);
	}
	
	// returns length of written. 0 if failed. len of data send to Serial
	size_t write(const uint8_t *buffer, size_t size) override {
		return stream->write(buffer, size);
	}

	using jSerial::write;

};


#endif // PLATFORM_RP2040