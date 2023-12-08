#pragma once

#include "jDefines.h"

// default buffer Size
#ifndef PARAM_WATERMARK_FACTOR
	#define PARAM_WATERMARK_FACTOR 0.50f
#endif

#define BUFFER_DEBUG_DEFAULT_VALUE 0x00

#ifdef ENABLE_JOS_DEBUG
	#define DEBUG_JBUFFER 1 // track buffer stats
#endif // ENABLE_JOS_DEBUG


// abstract implementation of Buffer
class jBuffer
{
public:
	jBuffer() 
	: LocalBuffer(nullptr)
	{

	};
	
	~jBuffer() {

	};

	
	// sets/allocates LocalBuffer pointer to array
	virtual void init_buffer() = 0;

	// releases allocated Buffer
	virtual void deinit_buffer() = 0;

	bool is_initialized() { return buffer_initialized; }
	

	// length of filled buffer
	virtual uint32_t length();

	// total capacity
	virtual uint32_t size();

	// returns maximum bytes free in buffer -> max available length to write to buffer
	virtual uint32_t write_available()
	{
		return size() - length();
	}

	bool isfull();
	
	// returns how full the buffer is as factor (0-1)
	float fill_level();

	
	// resets write index to zero
	virtual void clear();

	virtual bool write(uint8_t b);
	bool write(char value);	// char version
	
	// adds to end of buffer if enough space left. otherwise returns false
	virtual bool write(const uint8_t *Data, uint16_t dlength);
	bool write(const String& Data) {
		return write((uint8_t*) Data.c_str(), Data.length());
	}
	bool write(const char *Data, uint16_t dlength); // char version

	
	virtual bool contains(uint8_t value); // returns true if buffer contains given value
	virtual uint32_t find(uint8_t value); // returns index of first occurance of given value if contains. else return -1
	virtual uint8_t peak(uint32_t offset); // returns value at postion offset if offset < length() else 0xFF
	
	// get Buffer to Read sequential
	// returns max length that is possible to read sequential
	uint8_t* getBuffer();

	// get Buffer to Read sequential
	// returns max length that is possible to read sequential
	virtual uint32_t getBuffer(uint8_t*& pBuffer);
	
	bool OverWatermark();

	void setWatermark(uint32_t watermark_new)
	{
		watermark_new = LIMIT(0, watermark_new, buffer_size);

		buffer_watermark = watermark_new;
	}
	
	// set watermark as percentage of buffer size in (0.0 to 1.0)
	void setWatermark(float watermark_filllevel)
	{
		uint32_t watermark_new = buffer_size * LIMIT(0.0, watermark_filllevel, 1.0f);

		buffer_watermark = watermark_new;
	}

	virtual void printBuffer();

	// returns stats of buffer
	// max_len: maximum length (fill_level) seen sine last call
	// dt: time since last call
	// rate: kbytes/sec. added since last call
	void getTrackingStats(uint32_t& max_len, uint32_t& dt, uint32_t& rate, uint16_t& c_write_err_full);
	String printTrackingStats(const String& name = "");
	

protected:
	String tag = "buff";
	
	uint8_t* LocalBuffer = nullptr;
	int LocalBufferIndex = 0;
	bool buffer_initialized = false;

	uint32_t buffer_size = 64;
	uint32_t buffer_watermark = 32; // default is half of the buffer size

#ifdef DEBUG_JBUFFER
	// Traffic Tracking
	uint32_t time_start_tracking = millis();
	uint32_t stats_write_len_total = 0; // total bytes written to buffer in tracking epoch
	uint16_t stats_count_err_full = 0; // counts write not possible because buffer full
	uint32_t stats_max_length = 0; // tracks maximum length of data in FIFO
#endif // DEBUG_JBUFFER

};

class jByteBuffer : public jBuffer
{
public:
	jByteBuffer() : jBuffer()
	{

	};
	~jByteBuffer()
	{
		deinit_buffer();
	};
	jByteBuffer(uint32_t buffer_size, const String& tag="bbuff")
	: jBuffer()
	{
		this->tag = tag;
		this->buffer_size = buffer_size;
	};
	
	virtual void init_buffer() override;
	virtual void deinit_buffer() override;

protected:

};