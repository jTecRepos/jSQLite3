#pragma once

#include "jDefines.h"

#include "jByteBuffer.h"

//default buffer Size
#ifndef PARAM_WATERMARK_FACTOR
#define PARAM_WATERMARK_FACTOR 0.25f
#endif
//#define BUFFER_DEBUG


class jExternByteBuffer : public jBuffer
{
public:
	jExternByteBuffer()
	{
		tag = "ebbuff";
		//LogD("jExternByteBuffer.constructor");
	}
	~jExternByteBuffer()
	{
		//LogD("ebbuff.%s.constructor: buffer=x%X, size=%d", tag.c_str(), LocalBuffer, buffer_size);
	}

	jExternByteBuffer(String tag, uint8_t* buffer, uint32_t buffer_size)
	{
		this->tag = tag;

		this->buffer_size = buffer_size;

		this->LocalBuffer = buffer;

		//LogD("ebbuff.%s.constructor: buffer=x%X, size=%d", tag.c_str(), buffer, buffer_size);
		
		//init_buffer();
	}

	void init_buffer() override;
	void deinit_buffer() override;

protected:

	
};