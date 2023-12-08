#pragma once

#include "jFS_config.h"

#if JFS_ENABLE_SDFAT == 1

#include "jFile.h"

#include <SdFat.h>



#ifdef PLATFORM_ESP8266
using namespace sdfat;
#endif 



class jFileSdFat : public jFile 
{
protected:
	FsFile file; // the sdFat file handle
	uint32_t time_last_write = 0;

public:

	jFileSdFat() {
		
	}

	~jFileSdFat()
	{
		
	}

	jFileSdFat(FsFile _file, jFILE_MODE mode) 
	: jFile(mode), file(_file)
	{

	}

	FsFile getBaseFile() {
		return file;
	}

	bool isOpen() {
		return !!file;
	}


	bool close();

	bool getWriteError();

	uint64_t getLastWrite();

	bool isBusy() override;
	
	/** This function reports if the current file is a directory or not.
	 * \return true if the file is a directory.
	 */
	bool isDirectory();
	
	/** No longer implemented due to Long File Names.
	 *
	 * Use getName(char* name, uint32_t size).
	 * \return a pointer to replacement suggestion.
	 */
	String name();
	
	/** Ensure that any bytes written to the file are saved to the SD card. */
	void flush();
	
	/** \return the file's size. */
	uint32_t size();

	/** \return the current file position. */
	uint32_t position();
	
	/**
	 * Seek to a new position in the file, which must be between
	 * 0 and the size of the file (inclusive).
	 *
	 * \param[in] pos the new file position.
	 * \return true for success or false for failure.
	 */
	bool seek(uint32_t pos);
	
	uint32_t available();

	/** Return the next available byte without consuming it.
	 *
	 * \return The byte if no error and not at eof else -1;
	 */
	int peek();
	
	/** Read the next byte from a file.
	 *
	 * \return For success return the next byte in the file as an int.
	 * If an error occurs or end of file is reached return -1.
	 */
	int read();

	
	/** Read data from a file starting at the current position.
	 *
	 * \param[out] buf Pointer to the location that will receive the data.
	 *
	 * \param[in] len Maximum number of bytes to read.
	 *
	 * \return For success read() returns the number of bytes read.
	 * A value less than \a len, including zero, will be returned
	 * if end of file is reached.
	 * If an error occurs, read() returns -1.  Possible errors include
	 * read() called before a file has been opened, corrupt file system
	 * or an I/O error occurred.
	 */
	uint32_t read(uint8_t* buf, uint32_t len) override;
	
	/** Write a byte to a file. Required by the Arduino Print class.
	 * \param[in] b the byte to be written.
	 * Use getWriteError to check for errors.
	 * \return true for success and false for failure.
	 */
	bool write(uint8_t b);
	
	/** Write data to an open file.
	 *
	 * \note Data is moved to the cache but may not be written to the
	 * storage device until sync() is called.
	 *
	 * \param[in] buffer Pointer to the location of the data to be written.
	 *
	 * \param[in] len Number of bytes to write.
	 *
	 * \return For success write() returns the number of bytes written, always
	 * \a len.
	 */
	uint32_t write(const uint8_t* buffer, uint32_t len);
	

};

#endif // JFS_ENABLE_SDFAT