#include "jByteBuffer.h"

#include <jSystem.h>
#include <jHelp.h>



uint32_t jBuffer::length()
{
	return LocalBufferIndex;
}


// total capacity
uint32_t jBuffer::size()
{
	// last byte before Index_Read = Index_Write must be left unused -> Index_Read == Index_Write means length == 0
	return buffer_size;
}

bool jBuffer::isfull()
{
	return length() == size();
}

float jBuffer::fill_level()
{
	return ((float)length()) / (float)size();
}


void jBuffer::clear()
{
	if(!buffer_initialized)
		return;
		
	LocalBufferIndex = 0;

#ifdef DEBUG_JBUFFER
	for(int i = 0; i < this->buffer_size; i++)
		LocalBuffer[i] = BUFFER_DEBUG_DEFAULT_VALUE;
#endif // DEBUG_JBUFFER

}

bool jBuffer::write(uint8_t b)
{
	if(!buffer_initialized)
		return false;

	// check there is enough space to add to buffer
	if (this->length() < buffer_size)
	{

		// write to buffer
		LocalBuffer[LocalBufferIndex++] = b;

#ifdef DEBUG_JBUFFER
		stats_write_len_total++;
		stats_max_length = MAX(stats_max_length, this->length());
#endif // DEBUG_JBUFFER

		//success
		return true;
	}
	else
	{
#ifdef DEBUG_JBUFFER
		stats_count_err_full++;
#endif // DEBUG_JBUFFER

		return false;
	}
}


bool jBuffer::write(char value)
{
	return write((uint8_t)value);
}

bool jBuffer::write(const char *Data, uint16_t dlength)
{
	return write((uint8_t*)Data, dlength);
}

bool jBuffer::write(const uint8_t *Data, uint16_t dlength)
{
	if(!buffer_initialized)
		return false;

	// check there is enough space to add to buffer
	if (this->length() + dlength - 1 >= buffer_size)
	{
#ifdef DEBUG_JBUFFER
		stats_count_err_full++;
#endif // DEBUG_JBUFFER

		return false;
	}

	// copy to buffer
	for (int i = 0; i < dlength; i++)
		LocalBuffer[LocalBufferIndex++] = Data[i];
	
#ifdef DEBUG_JBUFFER
	stats_write_len_total += dlength;
	stats_max_length = MAX(stats_max_length, this->length());
#endif // DEBUG_JBUFFER

	// success
	return true;
}

uint8_t* jBuffer::getBuffer()
{
	return LocalBuffer;
}

// get Buffer to Read sequential
// returns max length that is possible to read sequential
uint32_t jBuffer::getBuffer(uint8_t*& pBuffer)
{
	if(!buffer_initialized)
		return 0;
		
	pBuffer = LocalBuffer;
	return length();
}


bool jBuffer::OverWatermark()
{
	return (length() > buffer_watermark);
}

// returns true if buffer contains value
bool jBuffer::contains(uint8_t value)
{
	for(int i = 0; i < length(); i++)
		if(peak(i) == value)
			return true;
	return false;
}

uint32_t jBuffer::find(uint8_t value)
{
	for(int i = 0; i < length(); i++)
		if(peak(i) == value)
			return i;
	return -1;
}


// returns value at postion offset if offset < length() else 0xFF
uint8_t jBuffer::peak(uint32_t offset)
{
	if(buffer_initialized && offset < length())
		return LocalBuffer[offset];
	else
		return 0xFF;
}

void jBuffer::printBuffer()
{
	if(!buffer_initialized)
	{
		LogD("Buffer not initialized!");
	}
	else
	{
		String out = "";
		for (int i = 0; i < size(); i++)
		{
			if (i == 0)
				out += "[";
			if (i == LocalBufferIndex)
				out +="] ";

			if (LocalBuffer[i] == 0)
				out +="_";
			else if (LocalBuffer[i] == '\n')
				out +="\\n";
			else if (LocalBuffer[i] == '\r')
				out +="\\r";
			else if (LocalBuffer[i] == '\t')
				out +="\\t";
			else
				out += format("%c", (char)LocalBuffer[i]);
		}
		if(size() == LocalBufferIndex)
			out +="]";
		
		LogD("buff: len=%d, data=\"%s\"", length(), out.c_str());
	}	
}


// rate: buffer fill rate in bytes / s
void jBuffer::getTrackingStats(uint32_t& max_len, uint32_t& dt, uint32_t& rate, uint16_t& c_write_err_full)
{
#ifdef DEBUG_JBUFFER
	max_len = stats_max_length;
	dt = millis() - time_start_tracking;
	if(dt > 0)
		rate = 1000 * stats_write_len_total / dt;
	else
		rate = 0;
		
	c_write_err_full = stats_count_err_full;
	
	time_start_tracking = millis(); 
	stats_write_len_total = 0;
	stats_max_length = this->length();
#endif // DEBUG_JBUFFER
}

String jBuffer::printTrackingStats(const String& name)
{
#ifdef DEBUG_JBUFFER
	uint32_t max_len, dt; uint32_t rate; uint16_t c_write_err_full;
	getTrackingStats(max_len, dt, rate, c_write_err_full);
	return format("%s: max_len=%d, max_lvl=%d, rate=%d, lvl=%d, err_full=%d", name.length() > 0 ? name.c_str() : tag.c_str(), 
		max_len,
		(100 * max_len) / size(),
		rate,
		(100 * length()) / size(),
		c_write_err_full
	);
#else
	return format("%s: len=%d, lvl=%d", name.length() > 0 ? name.c_str() : tag.c_str(), 
		length(),
		(100 * length()) / size()
	);
#endif // DEBUG_JBUFFER
}


///////////////////////////////////////////////////////////



void jByteBuffer::init_buffer()
{
	if(!buffer_initialized)
	{
		LocalBuffer = (uint8_t*) malloc (this->buffer_size);
		if(LocalBuffer == NULL)
		{
#ifdef DEBUG_JBUFFER
			Log("buff.%s.err: init_buffer - malloc failed", tag.c_str());
#endif // DEBUG_JBUFFER
		}
		else
		{
			buffer_initialized = true;

			clear();
			buffer_watermark = buffer_size * PARAM_WATERMARK_FACTOR;
		}
	}
}

void jByteBuffer::deinit_buffer()
{
	//if (LocalBuffer != NULL)
	if(buffer_initialized && LocalBuffer != nullptr)
		free (LocalBuffer);
	
	buffer_initialized = false;
}
