#pragma once

#include <jCommon.h>

#include "jFS_config.h"

// TODO make enum class and rename values to only "RW_APPEND" etc
enum jFILE_MODE {
	FILE_FLAG_RW_APPEND, // file read and write open at end of file (write can be done anywhere in the file only open at end by default)
	FILE_FLAG_RW, // file read and write, writes into the file overrides following content
	FILE_FLAG_RW_OVERRIDE, // file read and write, deletes file if exists
	FILE_FLAG_R, // file read only. position at start of file

	FILE_FLAG_RW_PREALOC, // file read and write open and prealoc for less write delays
};


#define JFILE_MAX_NAME_LENGTH 128

class jFile 
{
	protected:
		
		jFILE_MODE file_mode; // the file mode the file was opened with

	public:

		jFile() {}
		
		jFile(jFILE_MODE mode)
		: file_mode(mode)
		{

		}

		virtual ~jFile() {}

		//jFile(String filename);

		//jFile(String filename, jFILE_MODE mode);

		virtual bool isOpen() = 0;

		operator bool();
		

		virtual bool close() = 0;

		virtual bool getWriteError() = 0;

		// get last write time / timestamp of file as unix timestamp in seconds like 1659177432 for "Sat Jul 30 2022 10:37:12 GMT+0000"
		virtual uint64_t getLastWrite() = 0;


		virtual bool isBusy() {
			return false;
		}


		/** This function reports if the current file is a directory or not.
		 * \return true if the file is a directory.
		 */
		virtual bool isDirectory() = 0;
		

		/** No longer implemented due to Long File Names.
		 *
		 * Use getName(char* name, uint32_t size).
		 * \return a pointer to replacement suggestion.
		 */
		virtual String name() = 0;
		

		/** Ensure that any bytes written to the file are saved to the SD card. */
		virtual void flush() = 0;
		

		/** \return the file's size. */
		virtual uint32_t size() = 0;
		

		/** \return the current file position. */
		virtual uint32_t position() = 0;
		

		/**
		 * Seek to a new position in the file, which must be between
		 * 0 and the size of the file (inclusive).
		 *
		 * \param[in] pos the new file position.
		 * \return true for success or false for failure.
		 */
		virtual bool seek(uint32_t pos) = 0;
		
		
		// bytes available to read until end of file is reached
		virtual uint32_t available() = 0;

		/** Return the next available byte without consuming it.
		 *
		 * \return The byte if no error and not at eof else -1;
		 */
		virtual int peek() = 0;
		


		/** Read the next byte from a file.
		 *
		 * \return For success return the next byte in the file as an int.
		 * If an error occurs or end of file is reached return -1.
		 */
		virtual int read() = 0;


		/** Read the next byte from a file into param d
		 *
		 * \return For success return true. If an error occurs or end of file is reached return false.
		 */
		virtual bool read(uint8_t& d)
		{
			int v = read();
			if(v < 0)
				return false;
			d = (uint8_t) v;
			return true;
		}


		// Reads the next byte from file 
		// returns 0 if fails
		virtual char readchar()
		{
			int v = read();
			if(v == -1)
				return 0x00;
			return (char) v;
		}


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
		virtual uint32_t read(uint8_t* buf, uint32_t len) {
			uint8_t c;
			for(uint32_t i = 0; i < len; i++)
			{
				if(read(c))
					buf[i] = c;
				else
					return i;
			}
			return len;
		}
		

		/** Write a byte to a file. Required by the Arduino Print class.
		 * \param[in] b the byte to be written.
		 * Use getWriteError to check for errors.
		 * \return true for success and false for failure.
		 */
		virtual bool write(uint8_t b) = 0;
		

		/** Write data to an open file.
		 *
		 * \note Data is moved to the cache but may not be written to the
		 * storage device until sync() is called.
		 *
		 * \param[in] buffer Pointer to the location of the data to be written.
		 *
		 * \param[in] size Number of bytes to write.
		 *
		 * \return For success write() returns the number of bytes written, always
		 * \a size.
		 */
		virtual uint32_t write(const uint8_t* buffer, uint32_t len) = 0;
		

		uint32_t write(const char* buffer, uint32_t len) {
			return write((uint8_t*) buffer, len);
		}
		

		uint32_t write(const String& s) {
			return write(s.c_str(), s.length());
		}
		
};
