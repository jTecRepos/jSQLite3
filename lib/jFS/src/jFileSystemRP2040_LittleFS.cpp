#include "jFileSystemRP2040_LittleFS.h"

#ifdef PLATFORM_RP2040

#if JFS_DEBUG==1
#define LogX Log
#define LogXD LogD
#else
#define LogX(...)
#define LogXD(...)
#endif


#include <jSystem.h>

#include "FS.h"

//#define _FS LittleFS


// Read and Write with Position at end of file. appends startinf from end of file
#define _FILE_FLAG_RW_APPEND "a+"

// Read and Write with Position at start file. overrides starting from position 0
// Can be replaced by APPEND and move to first position (seek(0))
#define _FILE_FLAG_RW_START "r+"
#define _FILE_FLAG_RW_OVERRIDE "w+"

// For read file only - needed for Filetransfer over Wifi. 
// in General this might work better then w+ when reading a whole file from start to end
#define _FILE_FLAG_R "r"



static const char* FILENAME_ROOT = "/";


static String fix_path(const String& path)
{
	//if (path.length() > 0 && path[0] != '/')
	if (!path.startsWith(FILENAME_ROOT))
		return FILENAME_ROOT + path;
	else
		return path;
}


bool jFileSystemRP2040Class_LittleFS::begin()
{
	LogX("jfs.%s: begin", get_tag());	
	
	// try to mount
	bool b = _fs->begin();
	
	if (!b){
		LogX("jfs.%s: state='mount failed - formating'");

		if(_fs->format())
		{
			LogX("jfs.%s: state=formated");
		}
		else
		{
			is_mounted = false;
			return false;
		}
	}

	printStats();
	
	LogX("jfs.%s: state=mounted", get_tag());
	
	
	listDir("/");
	//LogX("%s.config_txt: exists=%d", get_tag(), exists("/config.txt"));

	is_mounted = true;
	return true;
}


void jFileSystemRP2040Class_LittleFS::end()
{
	LogX("jfs.%s: end", get_tag());
	_fs->end();
	is_mounted = false;
}


bool jFileSystemRP2040Class_LittleFS::format() {
	LogX("jfs.%s: formating", get_tag());
	return _fs->format();
}


jFileRP2040* jFileSystemRP2040Class_LittleFS::open(const String& path, jFILE_MODE mode)
{
	// Create empty file if file not exisits (fix with file functions not working if file not exists)
	if(!exists(path) && mode != FILE_FLAG_R)
	{
		fs::File file = _open(path, FILE_FLAG_RW_APPEND);
		uint8_t d[1];
		file.write(d, 0);
		file.flush();
		file.close();
	}

	return new jFileRP2040(_open(path, mode), mode);
}


fs::File jFileSystemRP2040Class_LittleFS::_open(const String& path, jFILE_MODE mode)
{	
	String _mode = _FILE_FLAG_RW_APPEND;
	switch(mode) {
		case FILE_FLAG_RW_APPEND: _mode = _FILE_FLAG_RW_APPEND; break;
		case FILE_FLAG_RW: _mode  = _FILE_FLAG_RW_START; break;
		case FILE_FLAG_RW_OVERRIDE: _mode = _FILE_FLAG_RW_OVERRIDE; break;
		case FILE_FLAG_R: _mode = _FILE_FLAG_R; break;
	}

	return _fs->open(fix_path(path), _mode.c_str());
}


bool jFileSystemRP2040Class_LittleFS::remove(const String& path)
{
	return _fs->remove(fix_path(path));
}


bool jFileSystemRP2040Class_LittleFS::rename(const String& path, const String& newpath)
{
	return _fs->rename(fix_path(path), fix_path(newpath));
}


uint32_t jFileSystemRP2040Class_LittleFS::get_filesize(const String& path) 
{
	if(exists(path))
	{
		fs::File f = _open(path, FILE_FLAG_R);
		uint32_t s = f.size();
		f.close();
		return s;
	}
	else
		return 0;
}


bool jFileSystemRP2040Class_LittleFS::exists(const String& path)
{
	return _fs->exists(fix_path(path));
}


bool jFileSystemRP2040Class_LittleFS::isMounted()
{
	return is_mounted;
	//File root = _fs->open("/");
	//return !!root;
}


bool jFileSystemRP2040Class_LittleFS::isErrorState()
{
	File root = _fs->open("/", _FILE_FLAG_R);
	if (!root || !root.isDirectory())
		return true;
	File file = root.openNextFile(); // test open first file
	return !file;
}



void jFileSystemRP2040Class_LittleFS::printStats()
{
	/*
	uint32_t bytesAvailable = ((fs::SPIFFSFS*) _fs)->totalBytes(); 	
	uint32_t bytesUsed = ((fs::SPIFFSFS*) _fs)->usedBytes(); 
 
	LogX("jfs.%s: avail=%d, used=%d, perc=%d", get_tag(), 
		bytesAvailable, bytesUsed, bytesUsed / (bytesAvailable / 100)
	);
	*/
}



void jFileSystemRP2040Class_LittleFS::listDir(const char* dirname, String postfix, int max_depth) {
	LogX("jfs.%s.lis: dir='%s'", get_tag(), dirname);

	File root = _fs->open(dirname, _FILE_FLAG_R);
	if (!root) {
		LogX("jfs.%s.lis: err=failed to open directory", get_tag());
		return;
	}
	if (!root.isDirectory()) {
		LogX("jfs.%s.lis: err=not a directory", get_tag());
		return;
	}


	File file = root.openNextFile();
	while (file) {

		if (file.isDirectory()) 
		{
			LogX("jfs.%s.lis: dir='%s%s'", get_tag(), postfix.c_str(), file.name());

			String postfix_new = postfix;
			if(postfix_new.length() > 0 || !postfix_new.endsWith("."))
				postfix_new += ".";
			postfix_new += file.name();
			
			if(max_depth > 0)
				listDir(file.fullName(), postfix_new, max_depth - 1);
		}
		else {
			LogX("jfs.%s.lis: file='%s%s', size=%d", get_tag(), postfix.c_str(), file.name(), file.size());
		}
		file = root.openNextFile();
	}
}


Vector<File> jFileSystemRP2040Class_LittleFS::getAllFilesInFolder(const char* dirname) {
	Vector<File> res;

	File root = _fs->open(String(dirname), _FILE_FLAG_R);
	if (!root)
		return res;
	
	if (!root.isDirectory())
		return res;
	

	File file;
	while (true) {
		file = root.openNextFile();
		if (!file)
			break;

		if (!file.isDirectory())
			res.push_back(file);
	}
	return res;
}


Vector<String> jFileSystemRP2040Class_LittleFS::getAllFileNamesInFolder(String dirname) {
	Vector<String> res;

	File root = _fs->open(dirname, _FILE_FLAG_R);
	if (!root) 
		return res;
	
	if (!root.isDirectory()) 
		return res;
	

	File file;
	while (true) {
		file = root.openNextFile();
		if (!file)
			break;

		if (!file.isDirectory()) {
			String fname = file.name();
			fname = fname.substring(dirname.length());
			//LogX(fname);
			res.push_back(fname);
		}
		
		file.close();
	}
	return res;
}


Vector<File> jFileSystemRP2040Class_LittleFS::getAllFiles(const String& path)
{
	return getAllFilesInFolder(path.c_str());
}


Vector<String> jFileSystemRP2040Class_LittleFS::getAllFilenames(const String& path, const String& ext)
{
	String _path = path;
	if(_path.length() == 0)
		_path = FILENAME_ROOT;

	return getAllFileNamesInFolder(_path.c_str());
}

void jFileSystemRP2040Class_LittleFS::remount()
{
	LogX("jfs.%s: remount", get_tag());
	
	end();
	begin();
}



#endif // PLATFORM_RP2040