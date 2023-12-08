#pragma once

#include "jDefines.h"

#include "jByteBuffer.h"

/*
A Ringbuffer for uint8_t

- supports concurent w+r -> read while write is happening
- does NOT support concurrent writes or reads. user code must make sure writes to the buffer or read from buffer is not done in multiple threads at the same time

*/
class jRingBuffer : public jBuffer
{
	
protected:
	
	uint32_t Index_Write;
	uint32_t Index_Read;
	
public:
		
	jRingBuffer() {
		tag = "rbuff";
		//LogD("rbuff.constructor");
	}

	~jRingBuffer() {
		//LogD("rbuff.%s.destructor: buffer=x%X, size=%d", tag.c_str(), LocalBuffer, buffer_size);
	}

	
	// total capacity will be 1 byte less then buffer_size
	jRingBuffer(uint32_t buffer_size, String tag="")
	{
		if(tag.length() > 0)
			this->tag = tag;

		this->buffer_size = buffer_size;
		//LogD("rbuff.%s.constructor: buffer=x%X, size=%d", tag.c_str(), LocalBuffer, buffer_size);
	}


	
	virtual void init_buffer() override;
	virtual void deinit_buffer() override;
	

	// length of filled buffer
	uint32_t length() override;

	// total capacity
	uint32_t size() override;
	
	// resets write index to zero
	void clear() override;

	void clear(uint32_t count);

	// Write byte to Buffer if room left. returns false if buffer full no write
	bool write(uint8_t value) override;

	
	//int16_t read(); // Read one value from buffer. if no value ein buffer returns -1
	bool read(uint8_t& value); // Reads from Buffer if available and write to param value. returns false if no read
	bool read(uint8_t* value);
	
	// Reads from Buffer into given buffer.
	// len: length of buffer to copy to -> max length requested
	// return: length of data copied to buffer
	uint16_t read(uint8_t* buffer, uint16_t len);
	
	//uint16_t read(String& buffer);
	

	// adds to end of buffer if enough space left. otherwise returns false
	bool write(const uint8_t* Data, uint16_t dlength) override;
	

	using jBuffer::write;

	// returns value at position offset if offset < length() else 0xFF
	uint8_t peak(uint32_t offset) override;
	
	// get Buffer to Read sequential
	// returns max length that is possible to read sequential
	uint32_t getBuffer(uint8_t*& pBuffer) override;
	
	void printBuffer() override;


protected:
	
	
	// unchecked length calculation
	uint32_t _length();

	// unchecked but thread save write
	void _write(uint8_t value);
	
	
};

