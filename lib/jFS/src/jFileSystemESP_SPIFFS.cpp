#include "jFileSystemESP_SPIFFS.h"

#ifdef PLATFORM_ESP32

#if JFS_DEBUG==1
#define LogX Log
#define LogXD LogD
#else
#define LogX(...)
#define LogXD(...)
#endif


#include <jSystem.h>
#include <jWatchdog.h>

#include "FS.h"
#include "SPIFFS.h"

static const char* FILENAME_ROOT = "/";

static String fix_path(const String& path)
{
	//if (path.length() > 0 && path[0] != '/')
	if (!path.startsWith(FILENAME_ROOT))
		return FILENAME_ROOT + path;
	else
		return path;
}


/***************************************************************************************/

jFileSystemESPClass_SPIFFS::jFileSystemESPClass_SPIFFS()
	: jFileSystemESPBaseClass("spiffs", &SPIFFS)
{

}


bool jFileSystemESPClass_SPIFFS::begin(bool formatiffail)
{
	LogX("jfs.%s: state=begin", get_tag());
	
	uint16_t wdt_t = jWatchdog_getTimeout();
	if(wdt_t)
		jWatchdog_enable(90 * FACTOR_s_2_ms); // set to 90s

	// might take up to a minute to format
	is_mounted = SPIFFS.begin(formatiffail);
	
	if(wdt_t)
		jWatchdog_enable(wdt_t);

	if (!is_mounted) {
		LogX("jfs.%s.err: card mount failed", get_tag());
		return false;
	}
	
	
	LogX("jfs.%s: state=mounted", get_tag());

#if JFS_DEBUG==1
	printStats();	
	listDir(FILENAME_ROOT, 1);
	LogX("jfs.%s.cfg: exists=%d", get_tag(), exists("/config.txt"));
#endif // 

	is_mounted = true;
	return true;
}


void jFileSystemESPClass_SPIFFS::end()
{
	LogX("jfs.%s:: end", get_tag());
	((fs::SPIFFSFS*) _fs)->end();
	is_mounted = false;
}




bool jFileSystemESPClass_SPIFFS::isMounted()
{
	return is_mounted;
}


uint64_t jFileSystemESPClass_SPIFFS::size() {
	//return ((fs::SPIFFSFS*) _fs)->cardSize();
	return ((fs::SPIFFSFS*) _fs)->totalBytes();
}


uint64_t jFileSystemESPClass_SPIFFS::used() {
	return ((fs::SPIFFSFS*) _fs)->usedBytes();
}


void jFileSystemESPClass_SPIFFS::printStats()
{
	uint32_t bytesAvailable = ((fs::SPIFFSFS*) _fs)->totalBytes(); 	
	uint32_t bytesUsed = ((fs::SPIFFSFS*) _fs)->usedBytes(); 
 
	LogX("jfs.%s: avail=%d, used=%d, perc=%d", get_tag(), 
		bytesAvailable, bytesUsed, bytesUsed / (bytesAvailable / 100)
	);
}






#endif // PLATFORM_ESP32