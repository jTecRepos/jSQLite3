#pragma once

#ifdef PLATFORM_RP2040

#include "jFileSystem.h"
#include "jFile.h"

#include "jFileRP2040.h"

#include <LittleFS.h>


class jFileSystemRP2040Class_LittleFS : public jFileSystemClass
{
protected:

	FS* _fs; // pointer of filesystem

public:
	jFileSystemRP2040Class_LittleFS()
	 : jFileSystemClass("littlefs"), _fs(&LittleFS)
	{

	}

	bool begin();

	void end();

	void remount();

	bool format();


	jFileRP2040* open(const String& path, jFILE_MODE mode = FILE_FLAG_RW_APPEND);

	// remove file
	bool remove(const String& path);

	// rename file (same as move file)
	bool rename(const String& path, const String& newpath);

	uint32_t get_filesize(const String& path);

	bool exists(const String& path);

	Vector<File> getAllFiles(const String& path);

	Vector<String> getAllFilenames(const String& path = "", const String& ext = "");

	bool isMounted();

	//checks if SD Card in Error State (minimum of one file must be present on SD)
	bool isErrorState();

	void printStats();

	void listDir(const char* dirname, String postfix="", int max_depth=0);

private:

	Vector<File> getAllFilesInFolder(const char* dirname);
	Vector<String> getAllFileNamesInFolder(String dirname);

	bool is_mounted = false;

	// basic class to open a file
	fs::File _open(const String& path, jFILE_MODE mode = FILE_FLAG_RW_APPEND);
};


#endif // PLATFORM_RP2040