#pragma once


#include "jFile.h"

#define JFILE_SECBUFFERED_DEBUG 0

class jFile_SecBuffered : public jFile 
{
	protected:	

		const static int SECTOR_BUFFER_COUNT = 5;
		const static int SECTOR_BUFFER_SIZE = 512;
		
		struct Sector_Buffer {
			uint8_t *data = nullptr; // the data of the sector
			uint32_t address = 0xFFFFFFFF; // Offset in file
			int id = -1; // unique id of this buffer - counts up - older buffer have lower indexs
			bool changed = false; // sector content changed flag
		};


		int sec_buff_counter = 0;
		
		Sector_Buffer sec_buffer[SECTOR_BUFFER_COUNT];

		
	
		jFile* file; // the wrapped file


		/*********************************************************/


		void print_sec_buff_status(const char* tag);
		


		/*********************************************************/

	public:

		jFile_SecBuffered(jFile* file);

		~jFile_SecBuffered()
		{
			// TODO what about free(file) ?
		}

		jFile* getBaseFile() {
			return file;
		}

		bool isOpen() override {
			return file->isOpen();
		}

		bool close();


		bool getWriteError() { 
			return file->getWriteError();
		}

		uint64_t getLastWrite() {
			return file->getLastWrite();
		}
		
		
		bool isDirectory() {
			return file->isDirectory();
		}
		
		
		String name() {
			return file->name();
		}
		
		void flush();
		
		uint32_t size() {
			return file->size();
		}

		uint32_t position() {
			return file->position();
		}
		
		bool seek(uint32_t pos) {
			return file->seek(pos);
		}
		
		uint32_t available() {
			return file->available();
		}
		
		int peek() {
			return file->peek();
		}
		
		int read() {
			uint8_t b;
			if(file->read(&b, 1) == 1)
				return b;
			return -1;
		}

		
		uint32_t read(uint8_t* buf, uint32_t len) override;
			
		
		
		bool write(uint8_t b) {
			return write(&b, 1);
		}
		
		
		uint32_t write(const uint8_t* buffer, uint32_t len);

};

