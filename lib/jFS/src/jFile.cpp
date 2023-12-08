#include "jFile.h"

#if JFS_DEBUG==1
#define LogX Log
#else
#define LogX(...)
#endif



jFile::operator bool()
{
	return isOpen();
}

/*
#include "jFS.h"


// Opens file. if file does not exist, empty file is created
jFile::jFile(String filename, jFILE_MODE mode)
{
	// Create empty file if file not exisits (fix with file functions not working if file not exists)
	if(!jFS_ESP.exists(filename))
	{
#ifdef DEBUG_STORAGE
		LogX(">> Creating new File %s", filename.c_str());
#endif 
		file = jFS_ESP.open(filename, FILE_FLAG_RW_APPEND);
		uint8_t d[1];
		file.write(d, 0);
		file.flush();
		file.close();
	}

	file_mode = mode;
	
	file = jFS_ESP.open(filename, mode);

}
*/