#include "jFileSdFat.h"

#if JFS_ENABLE_SDFAT == 1

#if JFS_DEBUG==1
#define LogX Log
#else
#define LogX(...)
#endif


#include <jSystem.h>
#include <jTimeHelp.h>

MUTEX_EXTERN(JFS_SDFAT_LOCK)

/*************************************************************************/


bool jFileSdFat::close()
{
	MUTEX_LOCK(JFS_SDFAT_LOCK);
	if(file_mode == FILE_FLAG_RW_PREALOC)
		file.truncate();

	bool res = file.close();
	MUTEX_UNLOCK(JFS_SDFAT_LOCK);
	return res;
}

bool jFileSdFat::getWriteError() { 
	MUTEX_LOCK(JFS_SDFAT_LOCK);
	bool res = file.getWriteError();
	MUTEX_UNLOCK(JFS_SDFAT_LOCK);
	return res;
}


uint64_t jFileSdFat::getLastWrite() 
{
	uint64_t res = time_last_write;
	
	MUTEX_LOCK(JFS_SDFAT_LOCK);

	uint16_t date, time;
	if (file.getModifyDateTime(&date, &time)) {
		uint16_t Y = FS_YEAR(date);
		uint8_t M = FS_MONTH(date);
		uint8_t D = FS_DAY(date);
		uint8_t h = FS_HOUR(time);
		uint8_t m = FS_MINUTE(time);
		uint8_t s = FS_SECOND(time);
		
		// file.EventLog20.TXT: date=2022.04.17, time=11:27:04
		//LogD("file.%s: date=%02d.%02d.%d, time=%d:%02d:%02d", this->name().c_str(), 
		//	Y, M, D, h, m, s
		//); 

		//jDateTime dt = jDateTime(Y, M, D, h, m, s);

		res = calc_epoch_time(Y, M, D, h, m, s);

	}

	MUTEX_UNLOCK(JFS_SDFAT_LOCK);

	return res;

}


uint32_t jFileSdFat::available() 
{
	uint32_t res = 0;
	MUTEX_LOCK(JFS_SDFAT_LOCK);

#ifdef PLATFORM_ESP8266
	res = file.available();
#else
	res = file.available64();
#endif 
	
	MUTEX_UNLOCK(JFS_SDFAT_LOCK);
	return res;
}

bool jFileSdFat::isBusy() 
{
	bool res = false;
	MUTEX_LOCK(JFS_SDFAT_LOCK);

#ifndef PLATFORM_ESP8266
	res = file.isBusy();
#endif 

	MUTEX_UNLOCK(JFS_SDFAT_LOCK);

	return res;
}


void jFileSdFat::flush() 
{
	MUTEX_LOCK(JFS_SDFAT_LOCK);
	file.flush();
	MUTEX_UNLOCK(JFS_SDFAT_LOCK);
}


bool jFileSdFat::isDirectory() 
{
	MUTEX_LOCK(JFS_SDFAT_LOCK);
	bool res = file.isDirectory();
	MUTEX_UNLOCK(JFS_SDFAT_LOCK);
	return res;
}


String jFileSdFat::name() 
{
	MUTEX_LOCK(JFS_SDFAT_LOCK);
	
	char buff[JFILE_MAX_NAME_LENGTH];
	file.getName(buff, JFILE_MAX_NAME_LENGTH);
	String name = String(buff);
	
	MUTEX_UNLOCK(JFS_SDFAT_LOCK);
	
	return name;
}


int jFileSdFat::peek() 
{
	MUTEX_LOCK(JFS_SDFAT_LOCK);
	int res = file.peek();
	MUTEX_UNLOCK(JFS_SDFAT_LOCK);
	return res;
}


uint32_t jFileSdFat::position() 
{
	MUTEX_LOCK(JFS_SDFAT_LOCK);
	uint32_t res = file.position();
	MUTEX_UNLOCK(JFS_SDFAT_LOCK);
	return res;
}


int jFileSdFat::read() 
{
	MUTEX_LOCK(JFS_SDFAT_LOCK);
	int res = file.read();
	MUTEX_UNLOCK(JFS_SDFAT_LOCK);
	return res;
}


uint32_t jFileSdFat::read(uint8_t* buf, uint32_t len) 
{
	MUTEX_LOCK(JFS_SDFAT_LOCK);
	uint32_t res = file.read(buf, len);
	MUTEX_UNLOCK(JFS_SDFAT_LOCK);
	return res;
}


bool jFileSdFat::seek(uint32_t pos) 
{
	MUTEX_LOCK(JFS_SDFAT_LOCK);
	bool res = file.seek(pos);
	MUTEX_UNLOCK(JFS_SDFAT_LOCK);
	return res;
}


uint32_t jFileSdFat::size() 
{
	MUTEX_LOCK(JFS_SDFAT_LOCK);
	uint32_t res = file.size();
	MUTEX_UNLOCK(JFS_SDFAT_LOCK);
	return res;
}


bool jFileSdFat::write(uint8_t b) 
{
	bool res = false;
	MUTEX_LOCK(JFS_SDFAT_LOCK);
	
	if(file.write(b))
	{
		time_last_write = millis();
		res = true;
	}
	
	MUTEX_UNLOCK(JFS_SDFAT_LOCK);
	return res;
}


uint32_t jFileSdFat::write(const uint8_t* buffer, uint32_t len) 
{
	MUTEX_LOCK(JFS_SDFAT_LOCK);
	
	uint32_t wlen = file.write(buffer, len);
	if(wlen == len)
		time_last_write = millis();

	MUTEX_UNLOCK(JFS_SDFAT_LOCK);
	return wlen;
}

#endif // 