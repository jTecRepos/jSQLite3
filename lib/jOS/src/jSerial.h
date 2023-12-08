#pragma once

#include "jDefines.h"

#include "jStream.h"

#define SERIAL_BAUD_9600 9600
#define SERIAL_BAUD_38400 38400
#define SERIAL_BAUD_115200 115200
#define SERIAL_BAUD_230400 230400
#define SERIAL_BAUD_460800 460800
#define SERIAL_BAUD_921600 9021600

enum jSerial_DataBits {
	jSERIAL_DATABITS_7,
	jSERIAL_DATABITS_8
};

enum jSerial_StopBits {
	jSERIAL_STOPBITS_1,
	jSERIAL_STOPBITS_2
};

enum jSerial_Parity {
	jSERIAL_PARITY_NONE,
	jSERIAL_PARITY_EVEN,
	jSERIAL_PARITY_ODD
};


class jSerial
	: public jStream
{
public:
	jSerial() { }

	
	virtual void begin() = 0;
	virtual void end() = 0;


	virtual void updateBaudRate(uint32_t baud) = 0;
	virtual uint32_t baudRate() = 0;
	
	//size_t setRxBufferSize(size_t);

	//operator bool() const;
	virtual bool isOpen() = 0;

	//virtual uint16_t available(void) = 0;
	virtual int available(void) = 0;
	//int availableForWrite(void);
	
	virtual int peek(void) {
		return 0;
	}

	// read single byte from serial input / rx buffer
	//virtual uint8_t read(void) = 0;
	virtual int read(void) = 0;
	
	// writes tx buffer out
	virtual void flush() {  }
	
	// writes to Serial. 
	// returns length of written. 0 if failed. 1 if success
	virtual size_t write(uint8_t c) = 0;
	
	// returns length of written. 0 if failed. len of data send to Serial
	virtual size_t write(const uint8_t *buffer, size_t size) {
		for(size_t i = 0; i < size; i++)
			if(!write(buffer[i]))
				return i;
		return size;
	}

	inline size_t write(const char * s)
	{
		return write((uint8_t*) s, strlen(s));
	}
	inline size_t write(const char *buffer, size_t size)
	{
		return write((const uint8_t *) buffer, size);
	}


	virtual size_t printf(const char * format, ...)	__attribute__ ((format (printf, 2, 3)));
	//size_t print(const __FlashStringHelper *);
	virtual size_t print(const String &s)
	{
		return write(s.c_str(), s.length());
	}

	//size_t print(const char[]);
	virtual size_t print(char c)
	{
		return write(c);
	}

	using jPrint::print;

	//size_t print(unsigned char, int = DEC);
	//size_t print(int, int = DEC);
	//size_t print(unsigned int, int = DEC);
	//size_t print(long, int = DEC);
	//size_t print(unsigned long, int = DEC);
	//size_t print(double, int = 2);
	//size_t print(const Printable&);
	//size_t print(struct tm * timeinfo, const char * format = NULL);

	//size_t println(const __FlashStringHelper *);
	//virtual size_t println(const String &s);
	//virtual size_t println(const char[]);
	//size_t println(char);
	//size_t println(unsigned char, int = DEC);
	//size_t println(int, int = DEC);
	//size_t println(unsigned int, int = DEC);
	//size_t println(long, int = DEC);
	//size_t println(unsigned long, int = DEC);
	//size_t println(double, int = 2);
	//size_t println(const Printable&);
	//size_t println(struct tm * timeinfo, const char * format = NULL);
	//size_t println(void);
	
};



/**************************************************************************/


#ifdef PLATFORM_RP2040
#include "jSerial_RP2040.h"
#endif // PLATFORM_RP2040


#ifdef PLATFORM_ESP32


/*

#define SERIAL_5N1 0x8000010
#define SERIAL_6N1 0x8000014
#define SERIAL_7N1 0x8000018
#define SERIAL_8N1 0x800001c
#define SERIAL_5N2 0x8000030
#define SERIAL_6N2 0x8000034
#define SERIAL_7N2 0x8000038
#define SERIAL_8N2 0x800003c
#define SERIAL_5E1 0x8000012
#define SERIAL_6E1 0x8000016
#define SERIAL_7E1 0x800001a
#define SERIAL_8E1 0x800001e
#define SERIAL_5E2 0x8000032
#define SERIAL_6E2 0x8000036
#define SERIAL_7E2 0x800003a
#define SERIAL_8E2 0x800003e
#define SERIAL_5O1 0x8000013
#define SERIAL_6O1 0x8000017
#define SERIAL_7O1 0x800001b
#define SERIAL_8O1 0x800001f
#define SERIAL_5O2 0x8000033
#define SERIAL_6O2 0x8000037
#define SERIAL_7O2 0x800003b
#define SERIAL_8O2 0x800003f
*/

#ifndef SERIAL_RX_BUFF_SIZE
	#define SERIAL_RX_BUFF_SIZE 512
#endif // SERIAL_RX_BUFF_SIZE


#if ARDUINO_USB_MODE
#if ARDUINO_USB_CDC_ON_BOOT // Serial used for USB CDC

#define HAS_SERIAL_USB
#define SERIAL_USB_CLASS HWCDC

#else
//HWCDC USBSerial;
#endif
#endif


class jSerialHard : public jSerial
{

protected:

	jStream* stream = nullptr;
	
	HardwareSerial* ser = nullptr;
	
#ifdef HAS_SERIAL_USB
	SERIAL_USB_CLASS* serUSB = nullptr;
	bool is_USB = false;
#endif // HAS_SERIAL_USB

	int8_t _pin_tx=-1;
	int8_t _pin_rx=-1;
	uint32_t _baud = SERIAL_BAUD_115200;
	bool is_open = false;


public:
	
	jSerialHard(HardwareSerial& serial) 
	: ser(&serial)
	{ 
		stream = ser;
	}

#ifdef HAS_SERIAL_USB

	jSerialHard(SERIAL_USB_CLASS& serial) 
	: serUSB(&serial)
	{ 
		stream = serUSB;
		is_USB = true;
	}

#endif // HAS_SERIAL_USB

	jSerialHard(HardwareSerial& serial, int8_t txPin, int8_t rxPin) 
	: ser(&serial)
	{ 
		stream = ser;
		_pin_tx = txPin;
		_pin_rx = rxPin;
	}

	void begin() {
		begin(_baud);
	}
	
	void begin(uint32_t baud) {
		if(is_open)
			end();
			
#if defined(VARIANTE_ESP32C3) || defined(VARIANTE_ESP32S3)
		int Freq = getCpuFrequencyMhz();
		if (Freq < 80) {
			baud = 80 / Freq * baud;
		}
#endif 

		_baud = baud;

#ifdef HAS_SERIAL_USB
		if(is_USB)
			serUSB->begin(baud);
		else
#endif // HAS_SERIAL_USB
		{
		
			ser->setRxBufferSize(SERIAL_RX_BUFF_SIZE);
		
			ser->begin(baud, SERIAL_8N1, _pin_rx, _pin_tx);

		}

		is_open = true;

	}

	bool isOpen() {
		return is_open;
	}

	void end() {
		if(is_open) 
		{
#ifdef HAS_SERIAL_USB
			if(is_USB)
				serUSB->end();
			else
#endif // HAS_SERIAL_USB
				ser->end();

			is_open = false;
		}
	}

	void updateBaudRate(uint32_t baud)
	{
#if defined(VARIANTE_ESP32C3) || defined(VARIANTE_ESP32S3)
		int Freq = getCpuFrequencyMhz();
		if (Freq < 80) {
			baud = 80 / Freq * baud;
		}
#endif 
		
		_baud = baud;
		
#ifdef ARDUINO_ESP32_RELEASE_1_0_4
		// end/begin crash with espressif32@3.0.0
		ser->updateBaudRate(baud);
#elif defined(ARDUINO_ESP32_RELEASE_1_0_6)
		// updateBaudRate does not work in espressif32@3.3.2
		this->end();
		this->begin();
#else 
		//default : 
		ser->updateBaudRate(baud);
#endif 
	
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


#endif // PLATFORM_ESP32


#ifdef PLATFORM_ESP8266

/*

enum SerialConfig {
	SERIAL_5N1 = UART_5N1,
	SERIAL_6N1 = UART_6N1,
	SERIAL_7N1 = UART_7N1,
	SERIAL_8N1 = UART_8N1,
	SERIAL_5N2 = UART_5N2,
	SERIAL_6N2 = UART_6N2,
	SERIAL_7N2 = UART_7N2,
	SERIAL_8N2 = UART_8N2,
	SERIAL_5E1 = UART_5E1,
	SERIAL_6E1 = UART_6E1,
	SERIAL_7E1 = UART_7E1,
	SERIAL_8E1 = UART_8E1,
	SERIAL_5E2 = UART_5E2,
	SERIAL_6E2 = UART_6E2,
	SERIAL_7E2 = UART_7E2,
	SERIAL_8E2 = UART_8E2,
	SERIAL_5O1 = UART_5O1,
	SERIAL_6O1 = UART_6O1,
	SERIAL_7O1 = UART_7O1,
	SERIAL_8O1 = UART_8O1,
	SERIAL_5O2 = UART_5O2,
	SERIAL_6O2 = UART_6O2,
	SERIAL_7O2 = UART_7O2,
	SERIAL_8O2 = UART_8O2,
};

enum SerialMode {
	SERIAL_FULL = UART_FULL,
	SERIAL_RX_ONLY = UART_RX_ONLY,
	SERIAL_TX_ONLY = UART_TX_ONLY
};
*/

#ifndef SERIAL_RX_BUFF_SIZE
	#define SERIAL_RX_BUFF_SIZE 256
#endif // SERIAL_RX_BUFF_SIZE

class jSerialHard : public jSerial
{
protected:
	HardwareSerial* ser = nullptr;
	int8_t _pin_tx=-1;
	int8_t _pin_rx=-1;
	uint32_t _baud = SERIAL_BAUD_115200;
	bool is_open = false;
public:
	jSerialHard(HardwareSerial& serial) 
	: ser(&serial)
	{ 

	}

	jSerialHard(HardwareSerial& serial, int8_t txPin, int8_t rxPin) 
	: ser(&serial)
	{ 
		_pin_tx = txPin;
		_pin_rx = rxPin;
	}

	void begin() {
		begin(_baud);
	}
	
	void begin(uint32_t baud) {
		if(is_open)
			end();

		_baud = baud;
		
		ser->setRxBufferSize(SERIAL_RX_BUFF_SIZE);
		
		ser->begin(baud, SERIAL_8N1);

		is_open = true;

	}

	bool isOpen() {
		return is_open;
	}

	void end() {
		if(is_open) 
		{
			ser->end();

			is_open = false;
		}
	}

	void updateBaudRate(uint32_t baud)
	{
		
		_baud = baud;
		
		ser->updateBaudRate(baud);
	
	}

	uint32_t baudRate() {
		return _baud;
	}
	
	int available(void) {
		return ser->available();
	}
	
	
	int peek(void) override {
		return ser->peek();
	}

	// read single byte from serial input / rx buffer
	int read(void) {
		return ser->read();
	}
	
	// writes tx buffer out
	void flush(void) {
		ser->flush();
	}
	
	// writes to Serial. 
	// returns length of written. 0 if failed. 1 if success
	size_t write(uint8_t c) {
		return ser->write(c);
	}
	
	// returns length of written. 0 if failed. len of data send to Serial
	size_t write(const uint8_t *buffer, size_t size) override {
		return ser->write(buffer, size);
	}

	using jSerial::write;

};


#endif // PLATFORM_ESP8266


#ifdef PLATFORM_AVR

#ifdef USBCON
#define HAS_SERIAL_USB
#endif

class jSerialHard : public jSerial
{
protected:
	
	jStream* stream = nullptr;
	HardwareSerial* ser = nullptr;
#ifdef HAS_SERIAL_USB
	Serial_* serUSB = nullptr;
	bool is_USB = false;
#endif // HAS_SERIAL_USB

	uint32_t _baud = SERIAL_BAUD_115200;
	bool is_open = false;

public:
	jSerialHard(HardwareSerial& serial) 
	: ser(&serial)
	{ 
		stream = ser;
	}
	
	jSerialHard(HardwareSerial& serial, int8_t txPin, int8_t rxPin) 
	: ser(&serial)
	{ 
		stream = ser;
		//_pin_tx = txPin;
		//_pin_rx = rxPin;
	}
	
#ifdef HAS_SERIAL_USB
	jSerialHard(Serial_& serial) 
	: serUSB(&serial)
	{ 
		stream = serUSB;
		is_USB = true;
	}
#endif // HAS_SERIAL_USB

	void begin() {
		begin(SERIAL_BAUD_115200);
	}
	
	void begin(uint32_t baud) 
	{		
#ifdef HAS_SERIAL_USB
		if(is_USB)
			serUSB->begin(baud);
		else
#endif // HAS_SERIAL_USB
			ser->begin(baud);

		_baud = baud;
		is_open = true;
	}

	void end() 
	{
#ifdef HAS_SERIAL_USB
		if(is_USB)
			serUSB->end();
		else
#endif // HAS_SERIAL_USB
			ser->end();
		
		is_open = false;
	}

	
	bool isOpen() {
		return is_open;
	}

	void updateBaudRate(uint32_t baud)
	{
		begin(baud);
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


#endif // PLATFORM_AVR

#ifdef PLATFORM_TEENSY

// https://www.pjrc.com/teensy/td_uart.html

#define HAS_SERIAL_USB


class jSerialHard : public jSerial
{
protected:
	jStream* stream = nullptr;
	HardwareSerial* ser = nullptr;
#ifdef HAS_SERIAL_USB
	usb_serial_class* serUSB = nullptr;
	bool is_USB = false;
#endif // HAS_SERIAL_USB

	uint32_t _baud = SERIAL_BAUD_115200;
	bool is_open = false;

public:
	jSerialHard(HardwareSerial& serial) 
	: ser(&serial)
	{ 
		stream = ser;
	}

	jSerialHard(HardwareSerial& serial, int8_t txPin, int8_t rxPin) 
	: ser(&serial)
	{ 
		stream = ser;
		//_pin_tx = txPin;
		//_pin_rx = rxPin;
	}
	
#ifdef HAS_SERIAL_USB
	jSerialHard(usb_serial_class& serial) 
	: serUSB(&serial)
	{ 
		stream = serUSB;
		is_USB = true;
	}
#endif // HAS_SERIAL_USB

	
	void begin() {
		begin(_baud);
	}
	
	void begin(uint32_t baud) 
	{
#ifdef HAS_SERIAL_USB
		if(is_USB)
			serUSB->begin(baud);
		else
#endif // HAS_SERIAL_USB
			ser->begin(baud);

		_baud = baud;
		is_open = true;
	}

	void end() 
	{
#ifdef HAS_SERIAL_USB
		if(is_USB)
			serUSB->end();
		else
#endif // HAS_SERIAL_USB
			ser->end();

		is_open = false;
	}

	
	bool isOpen() {
		return is_open;
	}


	void updateBaudRate(uint32_t baud)
	{
		begin(baud);
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


#endif // PLATFORM_TEENSY

#ifdef PLATFORM_STM32

#if defined (USBCON) && defined(USBD_USE_CDC)
#define HAS_SERIAL_USB
#endif

class jSerialHard : public jSerial
{
protected:
	jStream* stream = nullptr;
	HardwareSerial* ser = nullptr;

#ifdef HAS_SERIAL_USB
	USBSerial* serUSB = nullptr;
	bool is_USB = false;
#endif // HAS_SERIAL_USB

	uint32_t _baud = SERIAL_BAUD_115200;
	bool is_open = false;

public:
	jSerialHard(HardwareSerial& serial) 
	: ser(&serial)
	{ 
		stream = &serial;
	}

#ifdef HAS_SERIAL_USB
	jSerialHard(USBSerial& serial) 
	: serUSB(&serial)
	{ 
		is_USB = true;
		stream = &serial;
	}
#endif // HAS_SERIAL_USB
	
	jSerialHard(HardwareSerial& serial, int8_t txPin, int8_t rxPin) 
	: ser(&serial)
	{ 
		//_pin_tx = txPin;
		//_pin_rx = rxPin;
		stream = &serial;
	}

	

	void begin() {
		begin(_baud);
	}
		
	void begin(uint32_t baud) {
		
#ifdef HAS_SERIAL_USB
		if(is_USB)
			serUSB->begin(baud);
		else
#endif // HAS_SERIAL_USB
			ser->begin(baud);

		_baud = baud;
		is_open = true;
	}

	void end() {
#ifdef HAS_SERIAL_USB
		if(is_USB)
			serUSB->end();
		else
#endif // HAS_SERIAL_USB
			ser->end();

		is_open = false;
	}

	
	bool isOpen() {
		return is_open;
	}

	void updateBaudRate(uint32_t baud)
	{
		begin(baud);
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


#endif // PLATFORM_STM32

#ifdef PLATFORM_SAMD

class jSerialHard : public jSerial
{
protected:
	jStream* stream = nullptr;
	HardwareSerial* ser = nullptr;
	Serial_* serUSB = nullptr;
	bool is_USB = false;

	uint32_t _baud = SERIAL_BAUD_115200;
	bool is_open = false;	

public:
	jSerialHard(HardwareSerial& serial) 
	: ser(&serial)
	{ 
		stream = ser;
	}
	
	jSerialHard(HardwareSerial& serial, int8_t txPin, int8_t rxPin) 
	: ser(&serial)
	{ 
		stream = ser;
		//_pin_tx = txPin;
		//_pin_rx = rxPin;
	}

	jSerialHard(Serial_& serial) 
	: serUSB(&serial)
	{ 
		stream = serUSB;
		is_USB = true;
	}
	

	void begin() {
		begin(_baud);
	}
	
	void begin(uint32_t baud) {
		if(is_USB)
			serUSB->begin(baud);
		else
			ser->begin(baud);

		_baud = baud;
		is_open = true;
	}

	void end() {
		if(is_USB)
			serUSB->end();
		else
			ser->end();

		is_open = false;
	}

	
	bool isOpen() {
		return is_open;
	}

	void updateBaudRate(uint32_t baud)
	{
		begin(baud);
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

#endif // PLATFORM_SAMD
