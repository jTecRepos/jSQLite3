#include "jFileSystem.h"

#if JFS_DEBUG==1
#define LogX Log
#define LogXD LogD
#else
#define LogX(...)
#define LogXD(...)
#endif


#include <jSystem.h>
#include <jWatchdog.h>
#include <jHelp.h>

/*******************************  **************************************/

bool jFileSystemClass::copy(const String& path, const String& newpath)
{
	if(!exists(path))
		return false;

	if(exists(newpath))
		return false;

	bool res = true;


	jFile* f_src = open(path, jFILE_MODE::FILE_FLAG_R);
	jFile* f_dst = open(newpath, jFILE_MODE::FILE_FLAG_RW_OVERRIDE);

	// TODO what to do here ?
	//if(!f_src->isOpen() || !f_dst->isOpen())
	//	return false;

	uint32_t size = f_src->size();
	uint32_t size_new = 0;
	uint8_t buff[512];
	uint32_t last_time_wdt = millis();
	while (size_new < size && f_src->available())
	{
		int len = MIN(512, size - size_new);
		int rlen = f_src->read(buff, len);

		if(rlen == 0)
		{
			LogXD("jfs.%s.copy: err=read error");
			res = false;
			break;
		}

		int wlen = f_dst->write(buff, rlen);
		
		if(rlen == 0)
		{
			LogXD("jfs.%s.copy: err=write error, rlen=%d, wlen=%d", rlen, wlen);
			res = false;
			break;
		}

		size_new += wlen;
		
		EXECUTE_EVERY_NAME(last_time_wdt, 500)
			LogXD(">>> copy wdt reset");
			jWatchdog_reset();
		EXECUTE_EVERY_END

	}

	LogXD("jfs.%s.copy: size=%d, size_new=%d", get_tag(), size, size_new);

	f_src->close();
	f_dst->close();

	delete(f_src);
	delete(f_dst);

	return res;
}


bool jFileSystemClass::truncate(const String& path, uint32_t size) 
{
	if(!exists(path))
		return false;

	bool res = true;

	String ftmp = get_tmp_filename();

	LogXD("jfs.%s.truncate: ftmp=%s", get_tag(), ftmp.c_str());

	rename(path, ftmp);

	jFile* f_old = open(ftmp, jFILE_MODE::FILE_FLAG_R);
	jFile* f_new = open(path, jFILE_MODE::FILE_FLAG_RW_OVERRIDE);

	// TODO what to do here ?
	//if(!f_old->isOpen() || !f_new->isOpen())
	//	return false;

	uint32_t size_new = 0;
	uint8_t buff[512];
	uint32_t last_time_wdt = millis();
	while (size_new < size && f_old->available())
	{
		int len = MIN(512, size - size_new);
		int rlen = f_old->read(buff, len);

		if(rlen == 0)
		{
			LogXD("jfs.%s.truncate: err=read error");
			res = false;
			break;
		}

		int wlen = f_new->write(buff, rlen);
		
		if(rlen == 0)
		{
			LogXD("jfs.%s.copy: err=write error, rlen=%d, wlen=%d", rlen, wlen);
			res = false;
			break;
		}

		size_new += rlen;

		EXECUTE_EVERY_NAME(last_time_wdt, 500)
			jWatchdog_reset();
		EXECUTE_EVERY_END

	}

	LogXD("jfs.%s.truncate: size=%d, size_new=%d", get_tag(), size, size_new);

	f_old->close();
	f_new->close();

	delete(f_old);
	delete(f_new);

	remove(ftmp);

	return res;
}

String jFileSystemClass::get_tmp_filename() 
{
	//TODO deadlock in case of all temp files are present -> not likely
	while(true) {
		int x = get_random_int(0, 999);
		String fname = format("TMP%02d.BIN", x);
		if(!exists(fname))
			return fname;
	}
}


/******************************* get next file **************************************/

String jFileSystemClass::get_next_file(const String& fileformat, int& index, int max_index)
{
	String filename = "";
	for(index = 0; index < max_index; index++)
	{
		filename = format(fileformat.c_str(), index);
		if(!this->exists(filename))
			return filename;
		
		// loop over 1000 files may take a while
		if(index % 25 == 0)
			jWatchdog_reset();
	}
	return "";
}

String jFileSystemClass::get_last_file(const String& fileformat, int max_index)
{
	String filename = "";
	for(int i = 0; i < max_index; i++)
	{
		filename = format(fileformat.c_str(), i);
		
		if(!this->exists(filename)) // search for first file that does not exists
		{
			if(i > 0) // return last file (which should exist)
				return format(fileformat.c_str(), i - 1);
			else // no file exists
				return "";
		}
		
		// loop over 1000 files may take a while
		if(i % 25 == 0)
			jWatchdog_reset();
	}
	return "";
}


/*
//TODO implement
void jFileSystemClass::listDir(fs::FS& fs, const char* dirname, String postfix, int max_depth) {
	LogX("fs.%s.lis: dir='%s'", get_tag(), dirname);

	File root = fs.open(dirname, _FILE_FLAG_R);
	if (!root) {
		LogX("fs.%s.lis: err=failed to open directory", get_tag());
		return;
	}
	if (!root.isDirectory()) {
		LogX("fs.%s.lis: err=not a directory", get_tag());
		return;
	}


	File file = root.openNextFile();
	while (file) {

		if (file.isDirectory()) 
		{
			LogX("fs.%s.lis: dir='%s%s'", get_tag(), postfix.c_str(), file.name());

			String postfix_new = postfix;
			if(postfix_new.length() > 0 || !postfix_new.endsWith("."))
				postfix_new += ".";
			postfix_new += file.name();
			
			if(max_depth > 0)
				listDir(fs, file.fullName(), postfix_new, max_depth - 1);
		}
		else {
			LogX("fs.%s.lis: file='%s%s', size=%d", get_tag(), postfix.c_str(), file.name(), file.size());
		}
		file = root.openNextFile();
	}
}
*/