#pragma once

#include "jDefines.h"
#include "jSystem.h"

#include "jPrint.h"
#define jStream Stream

#ifndef jStream

// Implementation from ESP32 Firmware

class jStream: public jPrint
{
protected:
	uint32_t _timeout;      // number of milliseconds to wait for the next char before aborting timed read
	uint32_t _startMillis;  // used for timeout measurement
	int timedRead();    // private method to read stream with timeout
	int timedPeek();    // private method to peek stream with timeout
	int peekNextDigit(); // returns the next numeric digit in the stream or -1 if timeout

public:
	virtual int available() = 0;
	virtual int read() = 0;
	virtual int peek() = 0;
	virtual void flush() = 0;

	jStream():_startMillis(0)
	{
		_timeout = 1000;
	}
	virtual ~jStream() {}

// parsing methods

	void setTimeout(uint32_t timeout);  // sets maximum milliseconds to wait for stream data, default is 1 second
	uint32_t getTimeout(void);
	
	bool find(const char *target);   // reads data from the stream until the target string is found
	bool find(uint8_t *target)
	{
		return find((char *) target);
	}
	// returns true if target string is found, false if timed out (see setTimeout)

	bool find(const char *target, size_t length);   // reads data from the stream until the target string of given length is found
	bool find(const uint8_t *target, size_t length)
	{
		return find((char *) target, length);
	}
	// returns true if target string is found, false if timed out

	bool find(char target)
	{
		return find (&target, 1);
	}

	bool findUntil(const char *target, const char *terminator);   // as find but search ends if the terminator string is found
	bool findUntil(const uint8_t *target, const char *terminator)
	{
		return findUntil((char *) target, terminator);
	}

	bool findUntil(const char *target, size_t targetLen, const char *terminate, size_t termLen);   // as above but search ends if the terminate string is found
	bool findUntil(const uint8_t *target, size_t targetLen, const char *terminate, size_t termLen)
	{
		return findUntil((char *) target, targetLen, terminate, termLen);
	}

	long parseInt(); // returns the first valid (long) integer value from the current position.
	// initial characters that are not digits (or the minus sign) are skipped
	// integer is terminated by the first character that is not a digit.

	float parseFloat();               // float version of parseInt

	virtual size_t readBytes(char *buffer, size_t length); // read chars from stream into buffer
	virtual size_t readBytes(uint8_t *buffer, size_t length)
	{
		return readBytes((char *) buffer, length);
	}
	// terminates if length characters have been read or timeout (see setTimeout)
	// returns the number of characters placed in the buffer (0 means no valid data found)

	size_t readBytesUntil(char terminator, char *buffer, size_t length); // as readBytes with terminator character
	size_t readBytesUntil(char terminator, uint8_t *buffer, size_t length)
	{
		return readBytesUntil(terminator, (char *) buffer, length);
	}
	// terminates if length characters have been read, timeout, or if the terminator character  detected
	// returns the number of characters placed in the buffer (0 means no valid data found)

	// Arduino String functions to be added here
	virtual String readString();
	String readStringUntil(char terminator);

protected:
	long parseInt(char skipChar); // as above but the given skipChar is ignored
	// as above but the given skipChar is ignored
	// this allows format characters (typically commas) in values to be ignored

	float parseFloat(char skipChar);  // as above but the given skipChar is ignored

	struct MultiTarget {
	const char *str;  // string you're searching for
	size_t len;       // length of string you're searching for
	size_t index;     // index used by the search routine.
	};

// This allows you to search for an arbitrary number of strings.
// Returns index of the target that is found first or -1 if timeout occurs.
int findMulti(struct MultiTarget *targets, int tCount);

};


#endif

