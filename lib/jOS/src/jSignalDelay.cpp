#include "jSignalDelay.h"

#include <jSystem.h>


bool jSignalDelay::init()
{
	LogD("%s.init: size=%d", tag.c_str(), buffer_size);

	if(!buffer_initialized)
	{
		if(this->buffer_size == 0)
			return false;

		//Log("buff.%s.msg: malloc start", tag.c_str());
		LocalBuffer = (float*) malloc (this->buffer_size * sizeof(float));
		//Log("buff.%s.msg: Localbuffer=%X", tag.c_str(), LocalBuffer);
		if(LocalBuffer == NULL)
		{
			Log("buff.%s.err: init_buffer - malloc failed", tag.c_str());
			return false;
		}
		else
		{
			

			buffer_initialized = true;

			clear();
			return true;
		}
	}

	return true;
}


void jSignalDelay::deinit()
{
	LogD("%s.deinit: size=%d", tag.c_str(), buffer_size);
	
	if(buffer_initialized && LocalBuffer != 0)
		free(LocalBuffer);
	buffer_initialized = false;
}


void jSignalDelay::clear(float default_value)
{
	if(!buffer_initialized)
		return;

	buffer_index = 0;
	for(int i = 0; i < this->buffer_size; i++)
		LocalBuffer[i] = default_value;
}


float jSignalDelay::delay(float v)
{
	if(!buffer_initialized)
		return v;

	float v_out = LocalBuffer[buffer_index];
	LocalBuffer[buffer_index] = v;
	buffer_index = (buffer_index + 1) % buffer_size;

	return v_out;
}