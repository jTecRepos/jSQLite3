#include "jExternRingBuffer.h"

#include <jSystem.h>



void jExternRingBuffer::init_buffer()
{
#ifdef DEBUG_JBUFFER
	LogD("%s.init: size=%d", tag.c_str(), buffer_size);
#endif // DEBUG_JBUFFER

	Index_Read = 0;
	Index_Write = 0;
	buffer_watermark = PARAM_WATERMARK_FACTOR * buffer_size;
	buffer_initialized = true;

#ifdef DEBUG_JBUFFER
	LogD("%s: addr=x%X, size=%d, watermark=%d", tag.c_str(), LocalBuffer, buffer_size, buffer_watermark);

	for (int i = 0; i < buffer_size; i++)
		LocalBuffer[i] = BUFFER_DEBUG_DEFAULT_VALUE;
#endif // DEBUG_JBUFFER
}


void jExternRingBuffer::deinit_buffer()
{
#ifdef DEBUG_JBUFFER
	LogD("%s.deinit: size=%d", tag.c_str(), buffer_size);
#endif // DEBUG_JBUFFER

	// Not needed because is always present
}

