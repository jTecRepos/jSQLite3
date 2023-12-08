#include "jFileSystemESP.h"

#ifdef PLATFORM_ESP32


#if JFS_DEBUG==1
#define LogX Log
#define LogXD LogD
#else
#define LogX(...)
#define LogXD(...)
#endif



#include <jSystem.h>

static const char* FILENAME_ROOT = "/";


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

// Read and Write with Position at end of file. appends starting from end of file
#define _FILE_FLAG_RW_APPEND "a+"

// Read and Write with Position at start file. overrides starting from position 0
// Can be replaced by APPEND and move to first position (seek(0))
#define _FILE_FLAG_RW "r+"
#define _FILE_FLAG_RW_OVERRIDE "w+"

// For read file only 
// in General this might work better than w+ when reading a whole file from start to end
#define _FILE_FLAG_R "r"



static String fix_path(const String& path)
{
	//if (path.length() > 0 && path[0] != '/')
	if (!path.startsWith(FILENAME_ROOT))
		return FILENAME_ROOT + path;
	else
		return path;
}


Vector<String> jFileSystemESPBaseClass::getAllFileNamesInFolder(String dirname, String ext) {
	Vector<String> res;

	File root = _fs->open(dirname);

	if (!root)
		return res;

	if (!root.isDirectory()) 
		return res;

	File file;
	while (true) {
		file = root.openNextFile();
		if (!file)
		{
			//LogX(">break");
			break;
		}

		if (!file.isDirectory()) {
			String fname = file.name();
			fname = fname.substring(dirname.length());
			
			if(ext.length() > 0 && !fname.endsWith(ext)) // skip if ext filter is given
				continue;

			res.push_back(fname);
		}
		
		file.close();
	}
	return res;
}


Vector<File> jFileSystemESPBaseClass::getAllFilesInFolder(const char* dirname) {
	Vector<File> res;

	File root = _fs->open(dirname);
	if (!root) {
		//LogX("Failed to open directory");
		return res;
	}
	if (!root.isDirectory()) {
		//LogX("Not a directory");
		return res;
	}

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


void jFileSystemESPBaseClass::listDir(const char* dirname, uint8_t max_depth) 
{
	LogX("jfs.%s.lis: dir=%s", get_tag(), dirname);

	// TODO sync with RP2040 little FS implementation

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
		if (file.isDirectory()) {
			LogX("jfs.%s.lis: dir='%s'", get_tag(), file.name());
			if (max_depth > 0) {
				listDir(file.name(), max_depth - 1);
			}
		}
		else {
			LogX("jfs.%s.lis: file='%s', size=%d", get_tag(), file.name(), file.size());
		}
		file = root.openNextFile();
	}
}


/***************************************************************************************/




void jFileSystemESPBaseClass::remount()
{
	LogX("jfs.%s: remount", get_tag());
	end();
	begin();
}


jFileESP* jFileSystemESPBaseClass::open(const String& path, jFILE_MODE mode)
{
	LogX("jfs.%s: state=open, path='%s', mode=%d", get_tag(), path.c_str(), mode);

	// Create empty file if file not exisits (fix with file functions not working if file not exists)
	if(!exists(path) && mode != FILE_FLAG_R)
	{

		//LogX(">>> Creating new File %s", path.c_str());
		LogX("jfs.%s.open: create new file", get_tag());

		fs::File file = _open(path, FILE_FLAG_RW_APPEND);
		uint8_t d[1];
		file.write(d, 0);
		file.flush();
		file.close();
	}

	return new jFileESP(_open(path, mode), mode);
}

fs::File jFileSystemESPBaseClass::_open(const String& path, jFILE_MODE mode)
{	
	String _mode = _FILE_FLAG_RW_APPEND;
	switch(mode) {
		case FILE_FLAG_RW_APPEND: _mode = _FILE_FLAG_RW_APPEND; break;
		case FILE_FLAG_RW: _mode  = _FILE_FLAG_RW; break;
		case FILE_FLAG_RW_OVERRIDE: _mode = _FILE_FLAG_RW_OVERRIDE; break;
		case FILE_FLAG_R: _mode = _FILE_FLAG_R; break;
	}

	return _fs->open(fix_path(path), _mode.c_str());
}

bool jFileSystemESPBaseClass::remove(const String& path)
{
	return _fs->remove(fix_path(path));
}

bool jFileSystemESPBaseClass::rename(const String& path, const String& newpath)
{
	return _fs->rename(fix_path(path), fix_path(newpath));
}

uint32_t jFileSystemESPBaseClass::get_filesize(const String& path) 
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

bool jFileSystemESPBaseClass::exists(const String& path)
{
	return _fs->exists(fix_path(path));
}

bool jFileSystemESPBaseClass::isMounted()
{
	File root = _fs->open("/");
	return !!root;
}

bool jFileSystemESPBaseClass::isErrorState()
{
	File root = _fs->open("/");
	if (!root || !root.isDirectory())
		return true;
	File file = root.openNextFile(); // test open first file
	return !file;
}



Vector<File> jFileSystemESPBaseClass::getAllFiles(const String& path)
{
	return getAllFilesInFolder(path.c_str());
}


Vector<String> jFileSystemESPBaseClass::getAllFilenames(const String& path, const String& ext)
{
	String _path = path;
	if(_path.length() == 0)
		_path = FILENAME_ROOT;

	return getAllFileNamesInFolder(_path.c_str(), ext);
}


#endif // PLATFORM_ESP32