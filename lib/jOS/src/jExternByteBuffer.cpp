#include "jExternByteBuffer.h"

#include <jSystem.h>


void jExternByteBuffer::init_buffer()
{
	LogD("%s.init: size=%d", tag.c_str(), buffer_size);
	
	LocalBufferIndex = 0;
	buffer_watermark = PARAM_WATERMARK_FACTOR * buffer_size;
	buffer_initialized = true;
}


void jExternByteBuffer::deinit_buffer()
{
	
}
