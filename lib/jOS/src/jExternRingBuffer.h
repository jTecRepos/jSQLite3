#pragma once

#include "jDefines.h"

#include "jRingBuffer.h"

#define BUFFER_DEBUG


class jExternRingBuffer : public jRingBuffer
{
public:
	jExternRingBuffer()
	{
		tag = "erbuff";
		//LogD("jExternRingBuffer.constructor");
	}
	
	~jExternRingBuffer()
	{
		//LogD("erbuff.%s.constructor: buffer=x%X, size=%d", tag.c_str(), LocalBuffer, buffer_size);
	}

	jExternRingBuffer(String tag, uint8_t* buffer, uint32_t buffer_size)
	{
		this->tag = tag;

		this->buffer_size = buffer_size;

		this->LocalBuffer = buffer;

		//LogD("erbuff.%s.constructor: buffer=x%X, size=%d", tag.c_str(), buffer, buffer_size);
		
		//init_buffer();
	}
	
	void init_buffer() override;
	void deinit_buffer() override;

protected:
	
};

