#pragma once

#ifdef PLATFORM_TEENSY

#include "jFile.h"

#include <FS.h>


class jFileTeensy : public jFile 
{
	protected:
		File file;
		jFILE_MODE file_mode;
		uint32_t time_last_write = 0;

	public:

		jFileTeensy() {
			
		}

		~jFileTeensy()
		{
			
		}

		jFileTeensy(File _file, jFILE_MODE mode) 
		: file(_file), file_mode(mode)
		{

		}

		File getBaseFile() {
			return file;
		}

		bool isOpen() {
			return !!file;
		}

		bool close()
		{
			//if(file_mode == FILE_FLAG_RW_PREALOC)
			//	file.truncate();
			file.close();
			return true;
		}

		bool getWriteError() { 
			return file.getWriteError();
		}

		
		uint64_t getLastWrite() {
			//DateTimeFields tm;
			//if(file.getModifyTime(tm))
			//	return tm // TODO convert to epoch time
			return -1;
		}

		uint32_t available() {

			return file.available();
		}

		bool isBusy() override {
			return false;
		}
		
		/** Ensure that any bytes written to the file are saved to the SD card. */
		void flush() {
			file.flush();
		}
		
		/** This function reports if the current file is a directory or not.
			* \return true if the file is a directory.
			*/
		bool isDirectory() {
			return file.isDirectory();
		}
		
		/** No longer implemented due to Long File Names.
			*
			* Use getName(char* name, uint32_t size).
			* \return a pointer to replacement suggestion.
			*/
		String name() {
			String name = String(file.name());
			return name;
		}
		
		/** Return the next available byte without consuming it.
			*
			* \return The byte if no error and not at eof else -1;
			*/
		int peek() {
			return file.peek();
		}
		
		/** \return the current file position. */
		uint32_t position() {
			return file.position();
		}
		
		/** Read the next byte from a file.
			*
			* \return For success return the next byte in the file as an int.
			* If an error occurs or end of file is reached return -1.
			*/
		int read() {
			return file.read();
		}

		
		/** Read data from a file starting at the current position.
			*
			* \param[out] buf Pointer to the location that will receive the data.
			*
			* \param[in] count Maximum number of bytes to read.
			*
			* \return For success read() returns the number of bytes read.
			* A value less than \a count, including zero, will be returned
			* if end of file is reached.
			* If an error occurs, read() returns -1.  Possible errors include
			* read() called before a file has been opened, corrupt file system
			* or an I/O error occurred.
			*/
		uint32_t read(uint8_t* buf, uint32_t count) override {
			return file.read(buf, count);
		}
		
		/**
			* Seek to a new position in the file, which must be between
			* 0 and the size of the file (inclusive).
			*
			* \param[in] pos the new file position.
			* \return true for success or false for failure.
			*/
		bool seek(uint32_t pos) {
			return file.seek(pos);
		}
		
		/** \return the file's size. */
		uint32_t size() {
			return file.size();
		}
		
		/** Write a byte to a file. Required by the Arduino Print class.
			* \param[in] b the byte to be written.
			* Use getWriteError to check for errors.
			* \return 1 for success and 0 for failure.
			*/
		bool write(uint8_t b) {			
			
			if(file.write(b))
			{
				time_last_write = millis();
				return true;
			}
			return false;
		}
		
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
		uint32_t write(const uint8_t* buffer, uint32_t size) {
			uint32_t wlen = file.write(buffer, size);
			if(wlen == size)
				time_last_write = millis();
			return wlen;
		}
		
		uint32_t write(const char* buffer, uint32_t size) {
			uint32_t wlen = write((uint8_t*) buffer, size);
			if(wlen == size)
				time_last_write = millis();
			return wlen;
		}
};

#endif // PLATFORM_TEENSY