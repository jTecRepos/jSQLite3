#pragma once 
#include "Arduino.h"

class jRingBufferMini
{
protected: 

	uint8_t* _buffer;
	uint16_t _size = 0;

	
	uint16_t _iw;
	uint16_t _ir;

public:

	
	jRingBufferMini(uint8_t* buffer, uint16_t size)
	: _buffer(buffer), _size(size)
	{
		_ir = 0;
		_iw = 0;
	}

	
	void clear()
	{
		//MUTEX_LOCK(LOCK_RINGBUFFER_ACCESS);
		_ir = _iw;
		//MUTEX_UNLOCK(LOCK_RINGBUFFER_ACCESS);
	}


	// length of filled buffer
	uint16_t length() {

		if (_iw >= _ir)
			return _iw - _ir;
		else
			return _iw + _size - _ir;
	}
	
	// total capacity
	uint16_t size() {
		return _size - 1;
	}
	
	
	// Write byte to Buffer if room left. returns false if buffer full no write
	bool write(uint8_t value) 
	{
		if(length() < size())
		{
			_buffer[_iw] = value;
			
			//MUTEX_LOCK(LOCK_RINGBUFFER_ACCESS);
			_iw = (_iw + 1) % _size;
			//MUTEX_UNLOCK(LOCK_RINGBUFFER_ACCESS);

			return true;
		}

		return false;
	}

	uint16_t write(const uint8_t* data, uint16_t len)
	{
		for(int i = 0; i < len; i++)
		{
			if(!write(data[i]))
				return i;
		}
		return len;

	}

	
	uint8_t read()
	{
		uint8_t v;
		if(read(v))
			return v;
		else
			return 0xFF;
	}

	// Reads from Buffer if available and write to param value. returns false if no read
	bool read(uint8_t& value)
	{
		// check if buffer empty or not initialized
		if (length() == 0)
			return false;

		value = _buffer[_ir];
		
		//MUTEX_LOCK(LOCK_RINGBUFFER_ACCESS);
		_ir = (_ir + 1) % _size;
		//MUTEX_UNLOCK(LOCK_RINGBUFFER_ACCESS);
		
		return true;
	}

};

