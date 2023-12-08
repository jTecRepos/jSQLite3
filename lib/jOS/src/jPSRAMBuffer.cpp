#include "jPSRAMBuffer.h"

#if defined(PLATFORM_ESP32) && defined(BOARD_HAS_PSRAM)

#include <jSystem.h>



void jPSRAMBuffer::init_buffer()
{
#ifdef DEBUG_JBUFFER
	LogD("%s.init: size=%d", tag.c_str(), buffer_size);
#endif // DEBUG_JBUFFER

	LocalBuffer = (byte*) ps_malloc(this->buffer_size);
	
	Index_Read = 0;
	Index_Write = 0;
	buffer_watermark = PARAM_WATERMARK_FACTOR * buffer_size;
	buffer_initialized = true;

#ifdef BUFFER_DEBUG
	for (int i = 0; i < buffer_size; i++)
		LocalBuffer[i] = BUFFER_DEBUG_DEFAULT_VALUE;
#endif // BUFFER_DEBUG
}


void jPSRAMBuffer::deinit_buffer()
{
#ifdef DEBUG_JBUFFER
	LogD("%s.deinit: size=%d", tag.c_str(), buffer_size);
#endif // DEBUG_JBUFFER
	
	if(buffer_initialized && LocalBuffer != 0)
		free(LocalBuffer);
	buffer_initialized = false;
}

#endif // PLATFORM_ESP32