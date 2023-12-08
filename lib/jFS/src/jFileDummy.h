#pragma once


#include <jFile.h>

/*
jFileDummy

A Dummy file that can only be read but not be written to
Content is just ascii data

*/

class jFileDummy : public jFile 
{
protected:

	String _filename = "DummyFile.txt"; // file name
	uint32_t _size = (1 << 13); // file size
	uint32_t _posi = 0; // file cursor position

	// generate data function for read
	uint8_t _read(uint32_t p)
	{
		return (p % 27 == 26) ? '\n' : 'A' + (p % 27);
	}

public:

	jFileDummy()
	: jFile(jFILE_MODE::FILE_FLAG_R)
	{
		
	}

	~jFileDummy()
	{
		
	}

	jFileDummy(const String& filename, uint32_t size)
	{
		_filename = filename;
		_size = size;
	}

	
	bool isOpen() {
		return true;
	}

	bool close()
	{
		return true;
	}

	bool getWriteError() { 
		return false;
	}

	uint64_t getLastWrite() {
		return 0;
	}

	uint32_t available() {
		return _size - _posi;
	}

	bool isBusy() override {
		return false;
	}
	
	void flush() {
		
	}
	
	
	bool isDirectory() {
		return false;
	}
	
	
	String name() {
		return _filename;
	}
	
	
	int peek() {
		return _read(_posi);
	}
	

	uint32_t position() {
		return _posi;
	}
	
	
	int read() {
		return _read(_posi++);
	}

	
	
	uint32_t read(uint8_t* buf, uint32_t len) override {
		int i = 0;
		for(i = 0; i < len && _posi < _size; i++)
			buf[i] = _read(_posi++);
		return i;
	}
	
	
	bool seek(uint32_t pos) {
		_posi = pos;
		return true;
	}
	
	
	uint32_t size() {
		return _size;
	}
	
	
	bool write(uint8_t b) {
		
		return true;
	}
	
	
	uint32_t write(const uint8_t* buffer, uint32_t len) {
		return len;
	}
	
	uint32_t write(const char* buffer, uint32_t len) {
		return len;
	}
};
