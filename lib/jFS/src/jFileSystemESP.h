#pragma once

#ifdef PLATFORM_ESP32

#include "jFileSystem.h"
#include "jFile.h"
#include "jFileESP.h"


#include "FS.h"


class jFileSystemESPBaseClass : public jFileSystemClass
{
protected:

	FS* _fs; // pointer of filesystem

public:
	jFileSystemESPBaseClass(String tag, FS* fs)
	: jFileSystemClass(tag), _fs(fs)
	{

	}

	virtual void remount();

	
	virtual bool isMounted();

	virtual bool isErrorState();

	
	Vector<String> getAllFilenames(const String& path = "", const String& ext = "");

	
	/******************* file functions *******************************/
	
	virtual jFileESP* open(const String& path, jFILE_MODE mode = FILE_FLAG_RW_APPEND);

	// remove file
	bool remove(const String& path);

	// rename file (same as move file)
	bool rename(const String& path, const String& newpath);

	uint32_t get_filesize(const String& path);

	bool exists(const String& path);

		
	/*********************** util / debug funcs **************************/

	
	

	void listDir(const char* dirname) override {
		listDir(dirname, -1);
	}
	
	void listDir(const char* dirname, uint8_t max_depth);

protected: 

	Vector<File> getAllFiles(const String& path);


	Vector<String> getAllFileNamesInFolder(String dirname, String ext);
	Vector<File> getAllFilesInFolder(const char* dirname);

	// basic class to open a file
	virtual fs::File _open(const String& path, jFILE_MODE mode = FILE_FLAG_RW_APPEND);
};


#endif // PLATFORM_ESP32