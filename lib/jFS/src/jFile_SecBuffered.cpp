#include "jFile_SecBuffered.h"

#include <jSystem.h>
#include <jHelp.h>

#if JFILE_SECBUFFERED_DEBUG==1
#define LogX Log
#define LogXD LogD
#else
#define LogX(...)
#define LogXD(...)
#endif



jFile_SecBuffered::jFile_SecBuffered(jFile* file)
: file(file)
{
	
	// clear sec buffer
	for(uint8_t sec_buff_i = 0; sec_buff_i < SECTOR_BUFFER_COUNT; sec_buff_i++) {
		sec_buffer[sec_buff_i].data = nullptr;
		sec_buffer[sec_buff_i].address = 0xFFFFFFFF;
		sec_buffer[sec_buff_i].id = -1;
		sec_buffer[sec_buff_i].changed = false;
	}

}


void jFile_SecBuffered::print_sec_buff_status(const char* tag) {
	for(uint8_t sec_buff_i = 0; sec_buff_i < jFile_SecBuffered::SECTOR_BUFFER_COUNT; sec_buff_i++) {
		LogX("%s.sec_buff.%d: addr=%d, id=%d, data=%d, changed=%d", tag, 
			sec_buff_i, 
			sec_buffer[sec_buff_i].address, 
			sec_buffer[sec_buff_i].id, 
			sec_buffer[sec_buff_i].data,
			sec_buffer[sec_buff_i].changed
		);

	}
}


bool jFile_SecBuffered::close()
{
	
	// free sec buffer
	for(uint8_t sec_buff_i = 0; sec_buff_i < SECTOR_BUFFER_COUNT; sec_buff_i++) {
		if(sec_buffer[sec_buff_i].data != nullptr)
		{
			free(sec_buffer[sec_buff_i].data);
		}
	}

	return file->close();
}


void jFile_SecBuffered::flush() {
	
	int posi_backup = file->position();
	
	// check buffer
	for(uint8_t sec_buff_i = 0; sec_buff_i < SECTOR_BUFFER_COUNT; sec_buff_i++) {
		if(sec_buffer[sec_buff_i].changed)
		{
			
			
			// writeout buffer
			file->seek(sec_buffer[sec_buff_i].address);
			file->write(sec_buffer[sec_buff_i].data, SECTOR_BUFFER_SIZE);
			
			file->seek(posi_backup); // return to prev position

			// reset changed flag
			sec_buffer[sec_buff_i].changed = false;
		}
	}
	
	file->flush();
}


uint32_t jFile_SecBuffered::read(uint8_t* buf, uint32_t len) {
	
	uint32_t offset = file->position();

	LogXD("jFile_SecBuffered.read: posi=%d, len=%d", offset, len);

	// find sector buffer
	for(uint8_t sec_buff_i = 0; sec_buff_i < SECTOR_BUFFER_COUNT; sec_buff_i++) {
		
		// check if buffer is set and is correct address
		if(sec_buffer[sec_buff_i].address == offset && len == SECTOR_BUFFER_SIZE) {
			LogXD("jFile_SecBuffered.read: mode=read from sec_buffer, sec_buff_i=%d", sec_buff_i);
			memcpy(buf, sec_buffer[sec_buff_i].data, len);
			file->seek(offset + SECTOR_BUFFER_SIZE); // move cursor forward like actual read operation
			return len;
		}

		// if buffer intersects
		if(rangesIntersect(offset, len, sec_buffer[sec_buff_i].address, SECTOR_BUFFER_SIZE)) {
			LogX("jFile_SecBuffered.read: state=intersect flush, sec_buff_i=%d, addr=%d", id, sec_buff_i, sec_buffer[sec_buff_i].address);
			
			// writeout changes before if sector changed
			if(sec_buffer[sec_buff_i].changed) {

				uint32_t posi_backup = file->position();
				
				// writeout buffer content
				file->seek(sec_buffer[sec_buff_i].address);
				file->write(sec_buffer[sec_buff_i].data, SECTOR_BUFFER_SIZE);
				
				// return to previous position
				file->seek(posi_backup);
			}
		}

		//TODO check if inside a sector like some bites in sector
	}


	uint32_t rlen = file->read(buf, len);

	
	// update sector buffer
	if(len == SECTOR_BUFFER_SIZE) { // only for 512
		// lock for buffer to write to (empty or oldest)
		int8_t sec_buff_i = -1;
		for(uint8_t i = 0; i < SECTOR_BUFFER_COUNT; i++) {
			// if empty buffer
			if(sec_buffer[i].address == 0xFFFFFFFF)
			{
				if(sec_buffer[i].data == nullptr) // malloc if not already
					sec_buffer[i].data = (uint8_t *)malloc(SECTOR_BUFFER_SIZE);
				

				if(sec_buffer[i].data != nullptr) {
					sec_buff_i = i;
				}

				break;
			}

			// if buffer has lower id (is older)
			if(sec_buff_i < 0 || sec_buffer[sec_buff_i].id > sec_buffer[i].id)
			{
				sec_buff_i = i;
			}
		}

		// write to selected buffer
		if(sec_buff_i >= 0) {
			

			LogX("jFile_SecBuffered.read: mode=set sec_buffer, sec_buff_i=%d, addr=%d", sec_buff_i, offset);
			
			// writeout before override if sector changed
			if(sec_buffer[sec_buff_i].changed) {

				uint32_t posi_backup = file->position();
				
				// writeout buffer content
				file->seek(sec_buffer[sec_buff_i].address);
				file->write(sec_buffer[sec_buff_i].data, SECTOR_BUFFER_SIZE);
				
				// return to previous position
				file->seek(posi_backup);
			}


			memcpy(sec_buffer[sec_buff_i].data, buf, len);
			sec_buffer[sec_buff_i].address = offset;
			sec_buffer[sec_buff_i].id = sec_buff_counter++;
			sec_buffer[sec_buff_i].changed = false;
		}

		//print_sec_buff_status(p);
	}

	return rlen;

}



uint32_t jFile_SecBuffered::write(const uint8_t* buffer, uint32_t len) {
	uint32_t offset = file->position();

	LogXD("jFile_SecBuffered.write: posi=%d, len=%d", offset, len);

	// check sector buffers
	for(uint8_t sec_buff_i = 0; sec_buff_i < SECTOR_BUFFER_COUNT; sec_buff_i++) {
		
		// if buffer matches 
		if(sec_buffer[sec_buff_i].address == offset && SECTOR_BUFFER_SIZE == len) {
			LogXD("jFile_SecBuffered.write: step=update sec buffer, sec_buff_i=%d", sec_buff_i);
			
			//memcpy(sec_buffer[sec_buff_i].data, zBuf, SECTOR_BUFFER_SIZE);
			
			sec_buffer[sec_buff_i].changed = memcpycmp(sec_buffer[sec_buff_i].data, buffer, SECTOR_BUFFER_SIZE);
			if(!sec_buffer[sec_buff_i].changed) {
				LogXD("jFile_SecBuffered.write: step=skip write as not different");
				file->seek(offset + SECTOR_BUFFER_SIZE); // move cursor forward like the actual write operation
				return SECTOR_BUFFER_SIZE;
			}
			
		}
		// intersects with a buffer -> clear the buffer
		else if(rangesIntersect(offset, len, sec_buffer[sec_buff_i].address, SECTOR_BUFFER_SIZE)) {
			LogXD("jFile_SecBuffered.write: step=clear sec buffer, sec_buff_i=%d, addr=%d", sec_buff_i, sec_buffer[sec_buff_i].address);
			sec_buffer[sec_buff_i].address = 0xFFFFFFFF;
			sec_buffer[sec_buff_i].id = -1;
		}
		
	}
	
	return file->write(buffer, len);
}

