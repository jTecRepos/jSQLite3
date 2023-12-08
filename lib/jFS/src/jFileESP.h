#pragma once

#ifdef PLATFORM_ESP32

#include "jFile.h"

#include "FS.h"


class jFileESP : public jFile 
{
	protected:
		fs::File file;

	public:

		jFileESP() {
		}

		~jFileESP()
		{
			
		}

		//jFileESP(String filename);

		//jFileESP(String filename, jFILE_MODE mode);

		jFileESP(File _file, jFILE_MODE mode) 
		: jFile(mode), file(_file)
		{

		}

		File* getBaseFile() {
			return &file;
		}

		bool isOpen() override {
			return !!file;
		}

		bool close()
		{
			file.close();
			return true;
		}

		bool getWriteError() { 
			return file.getWriteError();
		}

		uint64_t getLastWrite() {
			return file.getLastWrite();
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
			return file.name();
		}
		
		/** Ensure that any bytes written to the file are saved to the SD card. */
		void flush() {
			file.flush();
		}
		
		/** \return the file's size. */
		uint32_t size() {
			return file.size();
		}
		
		/** \return the current file position. */
		uint32_t position() {
			return file.position();
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
		
		uint32_t available() {
			return file.available();
		}
		
		/** Return the next available byte without consuming it.
		 *
		 * \return The byte if no error and not at eof else -1;
		 */
		int peek() {
			return file.peek();
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
		 * \param[in] len Maximum number of bytes to read.
		 *
		 * \return For success read() returns the number of bytes read.
		 * A value less than \a len, including zero, will be returned
		 * if end of file is reached.
		 * If an error occurs, read() returns -1.  Possible errors include
		 * read() called before a file has been opened, corrupt file system
		 * or an I/O error occurred.
		 */
		uint32_t read(uint8_t* buf, uint32_t len) override {
			return file.read(buf, len);
		}
		
		/** Write a byte to a file. Required by the Arduino Print class.
		 * \param[in] b the byte to be written.
		 * Use getWriteError to check for errors.
		 * \return true for success and false for failure.
		 */
		bool write(uint8_t b) {
			return file.write(b);
		}
		
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
		uint32_t write(const uint8_t* buffer, uint32_t len) {
			return file.write(buffer, len);
		}
		
		uint32_t write(const char* buffer, uint32_t len) {
			return write((uint8_t*) buffer, len);
		}
		
		
};

#endif // PLATFORM_ESP32