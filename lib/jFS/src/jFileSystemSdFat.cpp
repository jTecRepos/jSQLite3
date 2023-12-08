#include "jFileSystemSdFat.h"

#include "jFS_config.h"

#if JFS_ENABLE_SDFAT == 1

#if JFS_DEBUG==1
#define LogX Log
#define LogXD LogD
#else
#define LogX(...)
#define LogXD(...)
#endif


#include "SdFat.h"

#include <jSystem.h>


/*
	src: https://diyprojects.io/esp32-get-started-spiff-library-read-write-modify-files/
	option is a string specifying the access mode. It can be

	"r" read, read only
	"r +" read and write. The pointer is positioned at the start of the file
	"w" write, write. The existing content is deleted. The file is created if it does not exist
	"w +" opens the file for reading and writing. The file is created if it does not exist, otherwise it is truncated. The pointer is positioned at the start of the file
	"a" append, opens a file adding data. The file is created if it does not exist. The pointer is positioned at the end of the file if it already exists
	"a +" append, opens a file adding data. The file is created if it does not exist. The pointer is positioned at the start of the file for reading and at the end of the file for writing (appending)
*/



// Read and Write with Position at end of file. appends startinf from end of file
#define _FILE_FLAG_RW_APPEND (O_RDWR | O_CREAT | O_APPEND)

// Read and Write with Position at start file. overrides starting from position 0
// Can be replaced by APPEND and move to first position (seek(0))
#define _FILE_FLAG_RW_START (O_RDWR | O_CREAT)
#define _FILE_FLAG_RW_OVERRIDE (O_RDWR | O_TRUNC)

// For read file only - needed for Filetransfer over Wifi. 
// in General this might work better then w+ when reading a whole file from start to end
#define _FILE_FLAG_R (O_RDONLY)

// Size to log 10 byte lines at 25 kHz for more than ten minutes.
#define LOG_FILE_SIZE 10*25000*600  // 150,000,000 bytes.


static const char* FILENAME_ROOT = "/";

/***************************************************************************************************************/

MUTEX_CREATE(JFS_SDFAT_LOCK)


/*********************************** jFileSystemSdFatBaseClass *************************************************/



void jFileSystemSdFatBaseClass::end()
{
	LogX("jfs.%s.state: end", get_tag());

	MUTEX_LOCK(JFS_SDFAT_LOCK);
	sd.end();
	MUTEX_UNLOCK(JFS_SDFAT_LOCK);
	
	_is_mounted = false;
}

jFileSdFat* jFileSystemSdFatBaseClass::open(const String& path, jFILE_MODE mode)
{
	LogX("jfs.%s: state=open, path='%s', mode=%d", get_tag(), path.c_str(), mode);
	
	

	// Create empty file if file not exisits (fix with file functions not working if file not exists)
	if(!exists(path))
	{
		// open not existent file in read mode is not possible
		if(mode == jFILE_MODE::FILE_FLAG_R)
			return new jFileSdFat(); // return closed null file


		//LogX(">>> Creating new File %s", path.c_str());
		LogX("jfs.%s.open: create new file", get_tag());

MUTEX_LOCK(JFS_SDFAT_LOCK);	
		// create a file and write nothing to it
		FsFile file = _open(path, FILE_FLAG_RW_APPEND);
		uint8_t d[1];
		file.write(d, 0);
		file.flush();
		file.close();
MUTEX_UNLOCK(JFS_SDFAT_LOCK);

	}
	
	MUTEX_LOCK(JFS_SDFAT_LOCK);
	jFileSdFat* res = new jFileSdFat(_open(path, mode), mode);
	MUTEX_UNLOCK(JFS_SDFAT_LOCK);

	return res;
}

// TODO consider making this a static local function outside of class
FsFile jFileSystemSdFatBaseClass::_open(const String& path, jFILE_MODE mode)
{
	int _mode = _FILE_FLAG_RW_APPEND;
	switch(mode) {
		case FILE_FLAG_RW_APPEND: _mode = _FILE_FLAG_RW_APPEND; break;
		case FILE_FLAG_RW: _mode  = _FILE_FLAG_RW_START; break;
		case FILE_FLAG_RW_OVERRIDE: _mode = _FILE_FLAG_RW_OVERRIDE; break;
		case FILE_FLAG_R: _mode = _FILE_FLAG_R; break;

		case FILE_FLAG_RW_PREALOC: _mode = O_RDWR | O_CREAT | O_TRUNC; break;
	}
	//return sd.open(path, _mode);
	
	if(mode == FILE_FLAG_RW_PREALOC)
	{
		FsFile file = sd.open(path, _mode);
		// File must be pre-allocated to avoid huge
		// delays searching for free clusters.
		if (!file.preAllocate(LOG_FILE_SIZE)) {
			LogX("jfs.%s.open: err=preAllocate failed", get_tag());
			//file.close();
			//return;
		}
		return file;
	}
	else
		return sd.open(path, _mode);
}




bool jFileSystemSdFatBaseClass::remove(const String& path)
{
	MUTEX_LOCK(JFS_SDFAT_LOCK);
	bool res = sd.remove(path);
	MUTEX_UNLOCK(JFS_SDFAT_LOCK);
	return res;
}

bool jFileSystemSdFatBaseClass::rename(const String& path, const String& newpath)
{
	MUTEX_LOCK(JFS_SDFAT_LOCK);
	bool res = sd.rename(path, newpath);
	MUTEX_UNLOCK(JFS_SDFAT_LOCK);
	return res;
}

uint32_t jFileSystemSdFatBaseClass::get_filesize(const String& path) 
{
	if(exists(path))
	{
		MUTEX_LOCK(JFS_SDFAT_LOCK);
		
		FsFile f = _open(path, FILE_FLAG_R);
		uint32_t s = f.size();
		f.close();
		
		MUTEX_UNLOCK(JFS_SDFAT_LOCK);
		return s;
	}
	else
		return 0;
}

bool jFileSystemSdFatBaseClass::exists(const String& path)
{
	MUTEX_LOCK(JFS_SDFAT_LOCK);
	bool res = sd.exists(path);
	MUTEX_UNLOCK(JFS_SDFAT_LOCK);
	return res;
}


bool jFileSystemSdFatBaseClass::truncate(const String& path, uint32_t size) 
{
	if(!exists(path))
		return false;

	bool err = false;

	MUTEX_LOCK(JFS_SDFAT_LOCK);
	
	FsFile f = _open(path, jFILE_MODE::FILE_FLAG_RW);
	if(f.isOpen())
	{
		err |= !f.truncate(size);
		err |= !f.close();
	}
	else {
		err = true;
	}

	MUTEX_UNLOCK(JFS_SDFAT_LOCK);	

	return !err;
}


bool jFileSystemSdFatBaseClass::isMounted()
{
	//return sd.card()->errorCode() == 0;
	return _is_mounted;
}

bool jFileSystemSdFatBaseClass::isErrorState()
{
	MUTEX_LOCK(JFS_SDFAT_LOCK);
	bool res = sd.card()->errorCode() != 0;   
	MUTEX_UNLOCK(JFS_SDFAT_LOCK);
	return res;
}




uint64_t jFileSystemSdFatBaseClass::size() {
	MUTEX_LOCK(JFS_SDFAT_LOCK);
	uint64_t res = ((uint64_t) sd.clusterCount()) * sd.bytesPerCluster();
	MUTEX_UNLOCK(JFS_SDFAT_LOCK);
	return res;
}


uint64_t jFileSystemSdFatBaseClass::used() {
	MUTEX_LOCK(JFS_SDFAT_LOCK);
	uint64_t res = ((uint64_t) sd.clusterCount() - sd.freeClusterCount()) * sd.bytesPerCluster();
	MUTEX_UNLOCK(JFS_SDFAT_LOCK);
	return res;
}


void jFileSystemSdFatBaseClass::printStats()
{
	MUTEX_LOCK(JFS_SDFAT_LOCK);

	if (sd.vol()->fatType() == 0) {
		LogX("jfs.%s.err: no valid FAT16/FAT32 partition", get_tag());
	}
	else if(sd.card()->sectorCount())
	{
		LogX("jfs.%s.err: determine card size failed", get_tag());
	}
	else
	{
		
		uint32_t size = sd.card()->sectorCount();
		uint32_t sizeMB = (0.000512 * size) + 0.5;

		LogX("jfs.%s: card_size=%d, type=FAT%d, cluster_size=%d", get_tag(),
			sizeMB,
			sd.vol()->fatType(),
			sd.vol()->bytesPerCluster()
		);

		int perc = sd.clusterCount() / (sd.freeClusterCount() / 100);
		LogX("jfs.%s: perc=%d", get_tag(), perc);

			
		//Log("Files found (date time size name):");
		//sd.ls(LS_R | LS_DATE | LS_SIZE);
		
		if ((sizeMB > 1100 && sd.vol()->sectorsPerCluster() < 64) || (sizeMB < 2200 && sd.vol()->fatType() == 32)) {
			LogXD("This card should be reformatted for best performance.");
			LogXD("Use a cluster size of 32 KB for cards larger than 1 GB.");
			LogXD("Only cards larger than 2 GB should be formatted FAT32.");
		}
	}

	MUTEX_UNLOCK(JFS_SDFAT_LOCK);
	
}


void jFileSystemSdFatBaseClass::listDir(const char* dirname, String postfix, int max_depth) 
{
	LogX("fs.%s.lis: dir='%s'", get_tag(), dirname);

	if(postfix.length() == 0)
		MUTEX_LOCK(JFS_SDFAT_LOCK);

	// open root directory
	FsFile root;
	if (!root.open(dirname))
	{
		LogX("jfs.%s.err: open root (%s) failed", get_tag(), dirname);
	}
	else 
	{
		// if not directory -> can not be listed
		if (!root.isDirectory()) 
		{
			LogX("jfs.%s.lis: err=not a directory", get_tag());
		}
		else // is directory:
		{

			// go over all files in root
			FsFile file;
			char fname[JFILE_MAX_NAME_LENGTH];
			while (file.openNext(&root, O_RDONLY)) 
			{

				file.getName(fname, JFILE_MAX_NAME_LENGTH);

				if (file.isHidden()) {

				}
				else if (file.isDirectory()) 
				{
					LogX("fs.%s.lis: dir='%s%s'", get_tag(), postfix.c_str(), fname);

					String postfix_new = postfix;
					if(postfix_new.length() > 0 || !postfix_new.endsWith("."))
						postfix_new += ".";
					postfix_new += String(fname);
					
					if(max_depth > 0)
						listDir(format("%s/%s", dirname, fname).c_str(), postfix_new, max_depth - 1);
				}
				else {
					LogX("jfs.%s.lis: file='%s%s', size=%d", get_tag(), postfix.c_str(), fname, file.size());
				}
				file = root.openNextFile();
			}
		}
	}

	if(postfix.length() == 0)
		MUTEX_UNLOCK(JFS_SDFAT_LOCK);
}



Vector<String> jFileSystemSdFatBaseClass::getAllFilenames(const String& path, const String& ext)
{
	Vector<String> res;

	String _path = path;
	if(_path.length() == 0)
		_path = FILENAME_ROOT;

	MUTEX_LOCK(JFS_SDFAT_LOCK);

	// open root directory
	FsFile root;
	if (!root.open(_path.c_str())) {
		LogX("jfs.%s.err: open root (%s) failed", get_tag(), _path.c_str());
	}
	else
	{
		
		// go over all files in root
		FsFile file;
		char buff[JFILE_MAX_NAME_LENGTH];
		//int rootFileCount = 0;
		while (file.openNext(&root, O_RDONLY)) 
		{
			if (file.isHidden()) {

			}
			else
			{
				file.getName(buff, JFILE_MAX_NAME_LENGTH);
				String filename = String(buff);
				
				if(ext.length() > 0 && !filename.endsWith(ext)) // skip if ext filter is given
					continue;

				res.push_back(filename);
				
				//rootFileCount++;
				//Log("%d: %s", rootFileCount, filename.c_str());
			}
			file.close();
		}

		root.close();
	}

	MUTEX_UNLOCK(JFS_SDFAT_LOCK);

	return res;
}



/********************************* jFileSystemSdFatSPIClass ****************************************/


#include <jTimeHelp.h>


#ifndef SPI_CKS_SPEED_SD
#if defined(VARIANTE_ESP32S3)
#define SPI_CKS_SPEED_SD 10 //20
#elif defined(PLATFORM_ESP32)
#define SPI_CKS_SPEED_SD 20
#elif defined(PLATFORM_RP2040)
#define SPI_CKS_SPEED_SD 31.25
#else // default
#define SPI_CKS_SPEED_SD 20
#endif 


#endif // SPI_CKS_SPEED_SD



//------------------------------------------------------------------------------
// Call back for file timestamps.  Only called for file create and sync().
void dateTime(uint16_t* date, uint16_t* time, uint8_t* ms10) {
	jDateTime now = jDateTime(getUNIXTimestamp());

	// Return date using FS_DATE macro to format fields.
	*date = FS_DATE(now.year(), now.month(), now.day());

	// Return time using FS_TIME macro to format fields.
	*time = FS_TIME(now.hour(), now.minute(), now.second());

	// Return low time bits in units of 10 ms, 0 <= ms10 <= 199.
	*ms10 = now.second() & 1 ? 100 : 0; // TODO this does not make sense
}



#if SPI_DRIVER_SELECT == 0 || SPI_DRIVER_SELECT == 1


jFileSystemSdFatSPIClass::jFileSystemSdFatSPIClass(int _pin_cs)
	: jFileSystemSdFatBaseClass("sd"), pin_cs(_pin_cs)
{

}

#endif // SPI_DRIVER_SELECT== 1 or 2

#if SPI_DRIVER_SELECT == 3  

#include <jSPI.h>

// This is a simple driver based on the the standard SPI.h library.
// You can write a driver entirely independent of SPI.h.
// It can be optimized for your board or a different SPI port can be used.
// The driver must be derived from SdSpiBaseClass.
// See: SdFat/src/SpiDriver/SdSpiBaseClass.h
class SdFatSPiClass : public SdSpiBaseClass {

public:
	SdFatSPiClass(jSPI* _spi)
	: spi(_spi)
	{

	}
	// Activate SPI hardware with correct speed and mode.
	void activate() {
		spi->beginTransaction();
	}
	// Initialize the SPI bus.
	void begin(SdSpiConfig config) {
		spi->begin();
		spi->setFrequency(config.maxSck);
	}
	// Deactivate SPI hardware.
	void deactivate() {
		spi->endTransaction();
	}
	// Receive a byte.
	uint8_t receive() {
		return spi->transfer(0XFF);
	}
	// Receive multiple bytes.
	// Replace this function if your board has multiple byte receive.
	uint8_t receive(uint8_t* buf, size_t count) {
		for (size_t i = 0; i < count; i++) {
			buf[i] = spi->transfer(0XFF);
		}
		return 0;
	}
	// Send a byte.
	void send(uint8_t data) {
		spi->transfer(data);
	}
	// Send multiple bytes.
	// Replace this function if your board has multiple byte send.
	void send(const uint8_t* buf, size_t count) {
		for (size_t i = 0; i < count; i++) {
			spi->transfer(buf[i]);
		}
	}
	// Save SPISettings for new max SCK frequency
	void setSckSpeed(uint32_t maxSck) {
		spi->setFrequency(maxSck);
	}

private:
	jSPI* spi;
};


//static SdFatSPiClass sdfat_spi; //TODO make this class member -> but therefore SdFatSPiClass definition must go to .h file

jFileSystemSdFatSPIClass::jFileSystemSdFatSPIClass(int _pin_cs, jSPI* _spi)
: jFileSystemSdFatBaseClass("sd"), pin_cs(_pin_cs), spi(_spi)
{
	
}

#endif // SPI_DRIVER_SELECT==3




bool jFileSystemSdFatSPIClass::begin()
{
	LogX("jfs.%s.state: begin", get_tag());

	// Set callback
	FsDateTime::setCallback(dateTime);

	MUTEX_LOCK(JFS_SDFAT_LOCK);

#if SPI_DRIVER_SELECT == 0 || SPI_DRIVER_SELECT == 1
	_is_mounted = sd.begin(pin_cs, SD_SCK_MHZ(SPI_CKS_SPEED_SD));
	//_is_mounted = sd.begin(SdSpiConfig(pin_cs, DEDICATED_SPI, SD_SCK_MHZ(SPI_CKS_SPEED_SD))); // dedicated spi holds the spi cs low to improve sd speeds (problems with switching cores)
#elif SPI_DRIVER_SELECT == 3
	
	//sdfat_spi = SdFatSPiClass(spi);
	//_is_mounted	= sd.begin(SdSpiConfig(pin_cs, DEDICATED_SPI, SD_SCK_MHZ(SPI_CKS_SPEED_SD), &sdfat_spi));
	
	// Memorycleak but best solution 
	//TODO maybe free/delete in destructor (but then needs to be class member)
	SdFatSPiClass* sdfat_spi = new SdFatSPiClass(spi);
	_is_mounted	= sd.begin(SdSpiConfig(pin_cs, DEDICATED_SPI, SD_SCK_MHZ(SPI_CKS_SPEED_SD), sdfat_spi));
#endif // 
	
	
	if (!_is_mounted)
	{
		LogX("jfs.%s.err: card mount failed", get_tag());

		if (sd.card()->errorCode()) 
		{
			LogX("jfs.%s.err: code=x%02X, data=x%02X", get_tag(),
				sd.card()->errorCode(),
				sd.card()->errorData()
			);
		}

	}
	else
	{
		LogX("jfs.%s: state=mounted", get_tag());
	}
	
	MUTEX_UNLOCK(JFS_SDFAT_LOCK);

	
#if JFS_DEBUG==1
	if(_is_mounted) {
		printStats();	
		listDir(FILENAME_ROOT);
		LogX("jfs.%s.cfg: exists=%d", get_tag(), exists("/config.txt"));
	}
#endif // JFS_DEBUG

	return _is_mounted;
}

/******************************* jFileSystemSdFatTeensySDIOClass *******************************************/

#ifdef PLATFORM_TEENSY

// Use Teensy SDIO
#define SD_CONFIG  SdioConfig(FIFO_SDIO)



jFileSystemSdFatTeensySDIOClass::jFileSystemSdFatTeensySDIOClass()
 : jFileSystemSdFatBaseClass("sd")
{

}


bool jFileSystemSdFatTeensySDIOClass::begin()
{
	LogX("jfs.%s.state: begin", get_tag());

	MUTEX_LOCK(JFS_SDFAT_LOCK);

	_is_mounted = sd.begin(SD_CONFIG);

	if (!_is_mounted)
	{
		LogX("jfs.%s.err: card mount failed", get_tag());

		if (sd.card()->errorCode()) 
		{
			LogX("jfs.%s.err: code=x%02X, data=x%02X", get_tag(), 
				sd.card()->errorCode(),
				sd.card()->errorData()
			);
		}

	}
	else
	{
		LogX("jfs.%s: state=mounted", get_tag());
	}

	MUTEX_UNLOCK(JFS_SDFAT_LOCK);


#if JFS_DEBUG==1
	if(_is_mounted) {
		printStats();	
		listDir(FILENAME_ROOT);
		LogX("jfs.%s.cfg: exists=%d", get_tag(), exists("/config.txt"));
	}
#endif // JFS_DEBUG

	return _is_mounted;
}



#endif // PLATFORM_TEENSY

#endif // 