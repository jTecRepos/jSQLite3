#pragma once

#include "jFS_config.h"

#if JFS_ENABLE_SDFAT == 1


#include "jFileSystem.h"
#include "jFile.h"
#include "jFileSdFat.h"


#include <SdFat.h>


class jFileSystemSdFatBaseClass : public jFileSystemClass
{
protected:

	SdFs sd;

	bool _is_mounted = false;

public:

	jFileSystemSdFatBaseClass(String tag = "sd")
	: jFileSystemClass(tag)
	{

	}

	// implemented in child classes
	//bool begin();

	void end();
	
	bool isMounted();

	// checks if SD Card in Error State (minimum of one file must be present on SD)
	bool isErrorState();


	Vector<String> getAllFilenames(const String& path = "", const String& ext = "");
	
	/******************* file functions *******************************/

	virtual jFileSdFat* open(const String& path, jFILE_MODE mode = FILE_FLAG_RW_APPEND);

	// remove file
	bool remove(const String& path);

	// rename file (same as move file)
	bool rename(const String& path, const String& newpath);

	uint32_t get_filesize(const String& path);

	bool exists(const String& path);

	// truncate a given file to the given size
	bool truncate(const String& path, uint32_t size) override; 

		
	/*********************** util / debug funcs **************************/

	uint64_t size() override;

	uint64_t used() override;
	
	void printStats();

	void listDir(const char* dirname) override {
		listDir(dirname, "", -1);
	}
	
	void listDir(const char* dirname, String postfix, int max_depth);

protected:
	

	// basic class to open a file
	virtual FsFile _open(const String& path, jFILE_MODE mode = FILE_FLAG_RW_APPEND);
};


/***************************** SD Card per SPI Version *******************************/

#if SPI_DRIVER_SELECT == 0	|| SPI_DRIVER_SELECT == 1

class jFileSystemSdFatSPIClass : public jFileSystemSdFatBaseClass
{
protected:

	int pin_cs = 0;

public:

	jFileSystemSdFatSPIClass(int _pin_cs);

	bool begin();
};

#endif // 


/***************************** SD Card per external SPI Version *******************************/

#if SPI_DRIVER_SELECT == 3

#include <jSPI.h>

class jFileSystemSdFatSPIClass : public jFileSystemSdFatBaseClass
{
protected:

	jSPI* spi;

	int pin_cs = 0;

public:

	jFileSystemSdFatSPIClass(int _pin_cs, jSPI* _spi);

	bool begin();
};

#endif // SPI_DRIVER_SELECT


#ifdef PLATFORM_TEENSY

class jFileSystemSdFatTeensySDIOClass : public jFileSystemSdFatBaseClass
{
protected:


public:

	jFileSystemSdFatTeensySDIOClass();

	bool begin();

};


#endif // PLATFORM_TEENSY

#endif // JFS_ENABLE_SDFAT