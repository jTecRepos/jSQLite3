#pragma once

#ifdef PLATFORM_TEENSY

#include "jFileSystem.h"
#include "jFile.h"

#include "jFileTeensy.h"

#include <LittleFS.h>

#ifndef JFS_TEENSY_LITTLEFS_SIZE
	#define JFS_TEENSY_LITTLEFS_SIZE (1 << 15)
#endif JFS_TEENSY_LITTLEFS_SIZE


class jFileSystemTeensyClass_LittleFS : public jFileSystemClass
{
protected:

	LittleFS_Program _fs; // filesystem

public:
	jFileSystemTeensyClass_LittleFS()
	 : jFileSystemClass("littlefs")
	{

	}

	bool begin();

	void end();

	void remount();

	bool format();


	jFileTeensy* open(const String& path, jFILE_MODE mode = FILE_FLAG_RW_APPEND);

	// remove file
	bool remove(const String& path);

	// rename file (same as move file)
	bool rename(const String& path, const String& newpath);

	uint32_t get_filesize(const String& path);

	bool exists(const String& path);

	//String listFilesString(const String& path);

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
	File _open(const String& path, jFILE_MODE mode = FILE_FLAG_RW_APPEND);
};


#endif // PLATFORM_TEENSY