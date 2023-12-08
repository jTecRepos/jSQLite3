#pragma once

#include <jCommon.h>

#include "jFile.h"


enum STORAGE_TYPE {
	SD_FS, // sd card (using default library)
	SPI_FFS // flash file system
};



class jFileSystemClass
{
protected:

	String tag = "fs";

public:
	jFileSystemClass(const String& tag)
	{
		this->tag = tag;
	}

	const char* get_tag() { return tag.c_str(); }

	// begin / mount filesystem
	virtual bool begin() = 0;

	// end / unmount filesystem
	virtual void end() = 0;

	// remount filesystem
	virtual void remount() {
		end();
		begin();
	}

	
	// returns true if filesystem is mounted and can be accessed
	virtual bool isMounted() = 0;

	// checks if SD Card in Error State (minimum of one file must be present on SD)
	virtual bool isErrorState() = 0;

	
	// returns a list of filenames of files in given path. only filenames are returned not complete path
	// ext: optional param to filter for fiven file extension
	virtual Vector<String> getAllFilenames(const String& path = "", const String& ext = "") = 0;


	/******************* file functions *******************************/

	// open a file from the filesystem
	virtual jFile* open(const String& path, jFILE_MODE mode = FILE_FLAG_RW_APPEND) = 0;

	// remove file
	virtual bool remove(const String& path) = 0;

	// rename file (same as move file)
	virtual bool rename(const String& path, const String& newpath) = 0;
	
	// copy a file to a given path
	virtual bool copy(const String& path, const String& newpath);

	// get filesize of a given file by path
	virtual uint32_t get_filesize(const String& path) = 0;

	// check if a the given path exists (file or dir)
	virtual bool exists(const String& path) = 0;

	// truncate a given file to the given size
	virtual bool truncate(const String& path, uint32_t size);

	
	/*********************** util / debug funcs **************************/

	// volumd size in bytes
	virtual uint64_t size() {
		return 0;
	}

	// volumd space filled in bytes
	virtual uint64_t used() {
		return 0;
	}
	
	
	virtual void printStats() = 0;

	virtual void listDir(const char* dirname) {
		// optional implement in childs
	}

	String get_tmp_filename();
	

	// looks for next file in numbered series that does not yet exists
	// fileformat like "LOG%02d.TXT" 
	// index: start index - returns selected index as reference
	// returns empty string of no file was found
	String get_next_file(const String& fileformat, int& index, int max_index=1000);
	
	// looks for last/latests file in numbered series that does exists
	// fileformat like "LOG%02d.TXT" 
	// returns empty string of no file was found
	String get_last_file(const String& fileformat, int max_index=1000);

};
