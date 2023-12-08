#pragma once

#if defined(PLATFORM_ESP32) && defined(BOARD_HAS_PSRAM)

#include <jDefines.h>
#include "jRingBuffer.h"

class jPSRAMBuffer : public jRingBuffer
{
public:
	jPSRAMBuffer()
	{
		tag = "sprbuff";
		//LogD("erbuff.constructor");
	}
	
	~jPSRAMBuffer()
	{
		//LogD("erbuff.%s.destructor: buffer=x%X, size=%d", tag.c_str(), LocalBuffer, buffer_size);
	}

	jPSRAMBuffer(uint32_t buffer_size, String tag="")
	{
		if(tag.length() > 0)
			this->tag = tag;

		this->buffer_size = buffer_size;
		//LogD("erbuff.%s.destructor: buffer=x%X, size=%d", tag.c_str(), LocalBuffer, buffer_size);
	}
	
	void init_buffer() override;
	void deinit_buffer() override;

protected:
	
};

#endif // PLATFORM_ESP32