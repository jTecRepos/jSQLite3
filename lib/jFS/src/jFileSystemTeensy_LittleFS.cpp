#include "jFileSystemTeensy_LittleFS.h"

#ifdef PLATFORM_TEENSY

#if JFS_DEBUG==1
#define LogX Log
#else
#define LogX(...)
#endif


#include <jSystem.h>

#include "FS.h"


// Read and Write with Position at end of file. appends startinf from end of file
#define _FILE_FLAG_RW_APPEND FILE_WRITE

// Read and Write with Position at start file. overrides starting from position 0
// Can be replaced by APPEND and move to first position (seek(0))
#define _FILE_FLAG_RW_START FILE_WRITE
#define _FILE_FLAG_RW_OVERRIDE FILE_WRITE_BEGIN

// For read file only - needed for Filetransfer over Wifi. 
// in General this might work better then w+ when reading a whole file from start to end
#define _FILE_FLAG_R FILE_READ



static const char* FILENAME_ROOT = "/";


static String fix_path(const String& path)
{
	//if (path.length() > 0 && path[0] != '/')
	if (!path.startsWith(FILENAME_ROOT))
		return FILENAME_ROOT + path;
	else
		return path;
}


bool jFileSystemTeensyClass_LittleFS::begin()
{
	LogX("jfs.%s: begin", get_tag());	
	
	// try to mount
	bool b = _fs.begin(JFS_TEENSY_LITTLEFS_SIZE);
	
	if (!b){
		LogX("jfs.%s: state='mount failed - formating'");

		if(_fs.format())
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
	//LogX("jfs.%s.config_txt: exists=%d", get_tag(), exists("/config.txt"));

	is_mounted = true;
	return true;
}


void jFileSystemTeensyClass_LittleFS::end()
{
	LogX("jfs.%s: end", get_tag());
	//_fs.end();
	is_mounted = false;
}


bool jFileSystemTeensyClass_LittleFS::format() {
	LogX("jfs.%s: formating", get_tag());
	return _fs.format();
}


jFileTeensy* jFileSystemTeensyClass_LittleFS::open(const String& path, jFILE_MODE mode)
{
	// Create empty file if file not exisits (fix with file functions not working if file not exists)
	if(!exists(path) && mode != FILE_FLAG_R)
	{
		File file = _open(path, FILE_FLAG_RW_APPEND);
		uint8_t d[1];
		file.write(d, 0);
		file.flush();
		file.close();
	}

	return new jFileTeensy(_open(path, mode), mode);
}


File jFileSystemTeensyClass_LittleFS::_open(const String& path, jFILE_MODE mode)
{	
	uint8_t _mode = _FILE_FLAG_RW_APPEND;
	switch(mode) {
		case FILE_FLAG_RW_APPEND: _mode = _FILE_FLAG_RW_APPEND; break;
		case FILE_FLAG_RW: _mode  = _FILE_FLAG_RW_START; break;
		case FILE_FLAG_RW_OVERRIDE: _mode = _FILE_FLAG_RW_OVERRIDE; break;
		case FILE_FLAG_R: _mode = _FILE_FLAG_R; break;
	}

	return _fs.open(fix_path(path).c_str(), _mode);
}


bool jFileSystemTeensyClass_LittleFS::remove(const String& path)
{
	return _fs.remove(fix_path(path).c_str());
}


bool jFileSystemTeensyClass_LittleFS::rename(const String& path, const String& newpath)
{
	return _fs.rename(fix_path(path).c_str(), fix_path(newpath).c_str());
}


uint32_t jFileSystemTeensyClass_LittleFS::get_filesize(const String& path) 
{
	if(exists(path))
	{
		File f = _open(path, FILE_FLAG_R);
		uint32_t s = f.size();
		f.close();
		return s;
	}
	else
		return 0;
}


bool jFileSystemTeensyClass_LittleFS::exists(const String& path)
{
	return _fs.exists(fix_path(path).c_str());
}


bool jFileSystemTeensyClass_LittleFS::isMounted()
{
	return is_mounted;
	//File root = _fs.open("/");
	//return !!root;
}


bool jFileSystemTeensyClass_LittleFS::isErrorState()
{
	File root = _fs.open("/", _FILE_FLAG_R);
	if (!root || !root.isDirectory())
		return true;
	File file = root.openNextFile(); // test open first file
	return !file;
}



void jFileSystemTeensyClass_LittleFS::printStats()
{
	
}


void jFileSystemTeensyClass_LittleFS::listDir(const char* dirname, String postfix, int max_depth) {
	LogX("jfs.%s.lis: dir='%s'", get_tag(), dirname);

	File root = _fs.open(dirname, _FILE_FLAG_R);
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
				listDir(file.name(), postfix_new, max_depth - 1);
		}
		else {
			LogX("jfs.%s.lis: file='%s%s', size=%d", get_tag(), postfix.c_str(), file.name(), file.size());
		}
		file = root.openNextFile();
	}
}


Vector<File> jFileSystemTeensyClass_LittleFS::getAllFilesInFolder(const char* dirname) {
	Vector<File> res;

	File root = _fs.open(dirname, _FILE_FLAG_R);
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


Vector<String> jFileSystemTeensyClass_LittleFS::getAllFileNamesInFolder(String dirname) {
	Vector<String> res;

	File root = _fs.open(dirname.c_str(), _FILE_FLAG_R);
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


Vector<File> jFileSystemTeensyClass_LittleFS::getAllFiles(const String& path)
{
	return getAllFilesInFolder(path.c_str());
}


Vector<String> jFileSystemTeensyClass_LittleFS::getAllFilenames(const String& path, const String& ext)
{
	String _path = path;
	if(_path.length() == 0)
		_path = FILENAME_ROOT;

	return getAllFileNamesInFolder(_path.c_str());
}

void jFileSystemTeensyClass_LittleFS::remount()
{
	LogX("jfs.%s: remount", get_tag());
	
	end();
	begin();
}



#endif // PLATFORM_TEENSY