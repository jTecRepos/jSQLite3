#pragma once

#include "jDefines.h"


class jSignalDelay {

public:
	
	jSignalDelay()
	{
		buffer_size = 0;
	}

	jSignalDelay(int16_t dt_ms, int16_t delay_ms)
	{
		dt_ms = MAX(dt_ms, 1);
		buffer_size = MAX(delay_ms / dt_ms, 1);
	}


	// allocates buffer. returns true if buffer is ready/initialized
	bool init();
	void deinit();

	void clear(float default_value=NAN);


	float delay(float v);

protected:

	String tag = "sigdelay";
	
	float* LocalBuffer = nullptr;
	bool buffer_initialized = false;
	int16_t buffer_index = 0;

	uint32_t buffer_size = 64;

};