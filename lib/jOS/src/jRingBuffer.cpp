#include "jRingBuffer.h"

#include <jSystem.h>
#include <jHelp.h>

#if DEBUG_JBUFFER==1
#define LogX Log
#define LogXD LogD
#else
#define LogX(...)
#define LogXD(...)
#endif 

// lock to protect concurrent modification of write and read cursor
#if 1
MUTEX_CREATE_LOCAL(LOCK_RINGBUFFER_CURSOR) 
#define LOCK_CURSOR MUTEX_LOCK(LOCK_RINGBUFFER_CURSOR)
#define UNLOCK_CURSOR MUTEX_UNLOCK(LOCK_RINGBUFFER_CURSOR)
#else
#define LOCK_CURSOR
#define UNLOCK_CURSOR
#endif


#if 0
MUTEX_CREATE_LOCAL(LOCK_RINGBUFFER_WRITE) 
#define LOCK_WRITE MUTEX_LOCK(LOCK_RINGBUFFER_WRITE)
#define UNLOCK_WRITE MUTEX_UNLOCK(LOCK_RINGBUFFER_WRITE)
#else
#define LOCK_WRITE
#define UNLOCK_WRITE
#endif


#if 0
MUTEX_CREATE_LOCAL(LOCK_RINGBUFFER_READ)
#define LOCK_READ MUTEX_LOCK(LOCK_RINGBUFFER_READ)
#define UNLOCK_READ MUTEX_UNLOCK(LOCK_RINGBUFFER_READ)
#else
#define LOCK_READ
#define UNLOCK_READ
#endif



void jRingBuffer::init_buffer() 
{
	LogXD("%s.init: size=%d", tag.c_str(), buffer_size);

	LocalBuffer = (uint8_t*) malloc (this->buffer_size);
	
	Index_Read = 0;
	Index_Write = 0;
	buffer_watermark = PARAM_WATERMARK_FACTOR * buffer_size;
	buffer_initialized = true;

#if DEBUG_JBUFFER==1
	for (int i = 0; i < buffer_size; i++)
		LocalBuffer[i] = BUFFER_DEBUG_DEFAULT_VALUE;
#endif // DEBUG_JBUFFER
}


void jRingBuffer::deinit_buffer()
{
	LogXD("%s.deinit: size=%d", tag.c_str(), buffer_size);

	
	if(buffer_initialized && LocalBuffer != 0)
		free(LocalBuffer);
	buffer_initialized = false;
}


uint32_t jRingBuffer::_length()
{
	if(!buffer_initialized)
		return 0;

	if (Index_Write >= Index_Read)
		return Index_Write - Index_Read;
	else
		return Index_Write + buffer_size - Index_Read;

	/*
	int32_t l = Index_Write - Index_Read;
	if (l < 0)
		l += buffer_size;
	return l
	*/

}


uint32_t jRingBuffer::length()
{
	uint32_t len = 0;
	
	LOCK_CURSOR;
	len = _length();
	UNLOCK_CURSOR;
	
	return len;
}


uint32_t jRingBuffer::size()
{
	// last byte before Index_Read = Index_Write must be left unused -> Index_Read == Index_Write means length == 0
	return buffer_size - 1;
}


void jRingBuffer::clear()
{
	LOCK_CURSOR;
		Index_Read = Index_Write;
	UNLOCK_CURSOR;
}


void jRingBuffer::clear(uint32_t count)
{
	if(!buffer_initialized)
		return;
	
	// limit clear to length of buffer -> min(length, count)
	//count = count > length() ? length() : count;
	count = MIN(count, length());

	//LOCK_CURSOR;

#if DEBUG_JBUFFER==1
	// only for debug
	for (int i = 0; i < count; i++)
		LocalBuffer[Index_Read + i] = BUFFER_DEBUG_DEFAULT_VALUE;
#endif // DEBUG_JBUFFER
	
	LOCK_CURSOR;
		Index_Read = (Index_Read + count) % buffer_size;
	UNLOCK_CURSOR;
}


/************************ WRITE *************************/


void jRingBuffer::_write(uint8_t value) 
{
	if(!buffer_initialized)
		return;

	LocalBuffer[Index_Write] = value;
	
	LOCK_CURSOR;
		Index_Write = (Index_Write + 1) % buffer_size;
	UNLOCK_CURSOR;


#if DEBUG_JBUFFER==1
	stats_write_len_total++;
	stats_max_length = MAX(stats_max_length, this->length());
#endif // DEBUG_JBUFFER
}


bool jRingBuffer::write(uint8_t value)
{
	if(!buffer_initialized)
		return false;

	// check if buffer full
	if (length() + 1 >= buffer_size) {
#if DEBUG_JBUFFER==1
		stats_count_err_full++;
#endif // DEBUG_JBUFFER
		return false;
	}
	LOCK_WRITE; //<-- WRITE SECTION START
	_write(value);
	UNLOCK_WRITE; //<-- WRITE SECTION END
	return true;
}


bool jRingBuffer::write(const uint8_t *Data, uint16_t dlength)
{
	if(!buffer_initialized)
		return false;

	// check there is enough space to add to buffer
	if (this->length() + dlength >= buffer_size)
	{
#if DEBUG_JBUFFER==1
		stats_count_err_full++;
#endif // DEBUG_JBUFFER
		return false;
	}

	LOCK_WRITE; //<-- WRITE SECTION START

	// copy to buffer
	// TODO optimize by directly adding to buffer
	for (int i = 0; i < dlength; i++)
		_write(Data[i]);

	UNLOCK_WRITE; //<-- WRITE SECTION END

	// success
	return true;
}


/************************ READ *************************/


bool jRingBuffer::read(uint8_t* value)
{
	// check if buffer empty
	if (!buffer_initialized || length() == 0)
		return false;

	LOCK_READ;

		*value = LocalBuffer[Index_Read];

	#if DEBUG_JBUFFER==1
		LocalBuffer[Index_Read] = BUFFER_DEBUG_DEFAULT_VALUE;
	#endif // DEBUG_JBUFFER
		
		LOCK_CURSOR;
			Index_Read = (Index_Read + 1) % buffer_size;
		UNLOCK_CURSOR;

	UNLOCK_READ;
	
	return true;
}


bool jRingBuffer::read(uint8_t& value)
{
	/*
	// check if buffer empty or not initialized
	if (!buffer_initialized || length() == 0)
		return false;

	LOCK_READ;

		value = LocalBuffer[Index_Read];

#if DEBUG_JBUFFER==1
		LocalBuffer[Index_Read] = BUFFER_DEBUG_DEFAULT_VALUE;
#endif // DEBUG_JBUFFER
		
		LOCK_CURSOR;
			Index_Read = (Index_Read + 1) % buffer_size;
		UNLOCK_CURSOR;
		
	UNLOCK_READ;
	
	return true;
	*/

	return read(&value);
}


/*
int16_t jRingBuffer::read()
{
	uint8_t v;
	
	if(!read(v))
		return -1;
	
	return v;
}
*/


uint16_t jRingBuffer::read(uint8_t* buffer, uint16_t len)
{
	if(!buffer_initialized)
		return 0;

	uint32_t buffer_index = 0; // index to write to output buffer next (also functions as length tracker of buffer)
	uint8_t* pBuffer; // pointer to start from where to read from buffer
	uint32_t len_copy; // length to read from buffer for readout iteration
	uint32_t len_aligned; // length of data that can be copied out of buffer aligned (without wrap around in ring)

	

	// - - - First Readout - - - 
	LOCK_READ;
	len_copy = len; // default to requested length
	len_aligned = getBuffer(pBuffer);
	if(len_aligned < len) // limit to aligned length
		len_copy = len_aligned;
	
	memcpy(buffer + buffer_index, pBuffer, len_copy);
	
	buffer_index += len_copy;
	clear(len_copy);

	UNLOCK_READ;

	// if length was reached
	if(buffer_index >= len)
		return buffer_index;



	// - - - Second readout if more readout needed - - - 
	LOCK_READ;
	len_aligned = getBuffer(pBuffer);
	len_copy = MIN(len_aligned, len - buffer_index); // calc remaining copy to be done
	
	memcpy(buffer + buffer_index, pBuffer, len_copy);
	
	buffer_index += len_copy;
	clear(len_copy);

	UNLOCK_READ;


	return buffer_index;
	
}


/*
uint16_t jRingBuffer::read(String& buffer)
{
	buffer = "";
	buffer.reserve(length());
	while(length())
		buffer += (char) read();
	return buffer.length();
}
*/


uint8_t jRingBuffer::peak(uint32_t offset)
{
	uint8_t res = 0xFF;
	
	LOCK_READ;
	LOCK_CURSOR;
	if(buffer_initialized && offset < _length())
		res = LocalBuffer[(Index_Read + offset) % buffer_size];
	UNLOCK_CURSOR;
	UNLOCK_READ;
	
	return res;
}


uint32_t jRingBuffer::getBuffer(uint8_t*& pBuffer)
{
	if(!buffer_initialized)
		return 0;

	//LOCK_READ;

	pBuffer = &LocalBuffer[Index_Read];
	
	// length to end of internal buffer
	uint32_t len = (buffer_size - Index_Read);
	
	// if ring-buffer is not filled to end of internal buffer
	// int_buffer: [____RXXXW___] length()=4
	if (len > _length()) 
		len = _length();
	
	// if ring-buffer is filled to end of internal buffer (and further) 
	// int_buffer: [XXW___RXXXXX] length()=8
	//else 
	//	return len; 

	//UNLOCK_READ;

	return len;
}


/******************** MISC ************************/


void jRingBuffer::printBuffer()
{
#if DEBUG_JBUFFER==1

	LogD("____________________________________________jRingBuffer::printBuffer()___________________________________________________");
	LogD("RingBuffer: %s", this->tag.c_str());

	if(!buffer_initialized)
	{
		LogD("Buffer not initialized!");
	}
	else
	{
		
		String out = "LocalBuffer: \n|";
		for (int i = 0; i < size() + 1; i++)
		{
			if (i == Index_Read)
				out += "[";
			if (i == Index_Write)
				out += "]";

			if (LocalBuffer[i] == 0)
				out += "_";
			else if (LocalBuffer[i] == BUFFER_DEBUG_DEFAULT_VALUE)
				out += "-";
			else if (LocalBuffer[i] == '\n')
				out +="\\n";
			else if (LocalBuffer[i] == '\r')
				out +="\\r";
			else if (LocalBuffer[i] == '\t')
				out +="\\t";
			//else if (isAlpha(LocalBuffer[i]))
			else if (isPrintable(LocalBuffer[i]))
				out += format("%c", LocalBuffer[i]);
			else
				//out += format("%c", (char)LocalBuffer[i]);
				out += format(" x%02X", (uint8_t)LocalBuffer[i]);

			if(i >= 128 && i % 128 == 0)
				out += "\n";
		}
		out += "|\n";
		LogD(out);
	}
	
	
	LogD("Length=%d", length());
	LogD("ReadIndex=%d", Index_Read);
	LogD("WriteIndex=%d", Index_Write);
	LogD("______________________________________________________________________________________________________________________________");
	LogD("");

#endif // DEBUG_JBUFFER
}
