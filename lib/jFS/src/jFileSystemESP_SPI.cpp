#include "jFileSystemESP_SPI.h"

#ifdef PLATFORM_ESP32

#if JFS_DEBUG==1
#define LogX Log
#define LogXD LogD
#else
#define LogX(...)
#define LogXD(...)
#endif



#include <jSystem.h>

#include "SD.h"
#include <SPI.h>

// for alt spi
//SPIClass spi = SPIClass(HSPI);


//#define SPI_SD_SPEED 80E6 // 80Mhz
#define SPI_SD_SPEED 20E6 // 20Mhz



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


jFileSystemESPClass_SPI::jFileSystemESPClass_SPI(int _pin_cs)
: jFileSystemESPBaseClass("sd", &SD), pin_cs(_pin_cs)
{

}


bool jFileSystemESPClass_SPI::begin()
{
	LogX("jfs.%s: begin", get_tag());

	bool b = false;

	//SPIClass spi = SPIClass(HSPI);
	
	// Alt SPI
	//spi.begin(PIN_SD_CLK /* SCK */, PIN_SD_DAT0 /* MISO */, PIN_SD_CMD /* MOSI */, PIN_SD_CD /* SS */);
	//b = SD.begin(PIN_SD_CD, spi, SPI_SD_SPEED);
	b = SD.begin(pin_cs, SPI, SPI_SD_SPEED);


	if (!b) {
		LogX("jfs.%s.err: card mount failed", get_tag());
		return false;
	}

	LogX("jfs.%s: state=mounted", get_tag());

#if JFS_DEBUG==1
	printStats();	
	listDir(FILENAME_ROOT, 1);
	LogX("jfs.%s.cfg: exists=%d", get_tag(), exists("/config.txt"));
#endif // 

	return true;
}


void jFileSystemESPClass_SPI::end()
{
	LogX("jfs.%s: end", get_tag());
	((fs::SDFS*) _fs)->end();
}



uint64_t jFileSystemESPClass_SPI::size() {
	//return ((fs::SDFS*) _fs)->cardSize();
	return ((fs::SDFS*) _fs)->totalBytes();
}


uint64_t jFileSystemESPClass_SPI::used() {
	return ((fs::SDFS*) _fs)->usedBytes();
}


void jFileSystemESPClass_SPI::printStats()
{
	uint8_t cardType = ((fs::SDFS*) _fs)->cardType();

	
	if(cardType == CARD_NONE){
		LogX("jfs.%s.err: no card found", get_tag());
		return;
	}
 
	String s = ""; 
	if(cardType == CARD_MMC) s += "MMC";
	else if(cardType == CARD_SD) s += "SDSC";
	else if(cardType == CARD_SDHC) s += "SDHC";
	else s += "UNKNOWN";
	LogX("jfs.%s.card: type=%s", get_tag(), s.c_str());
 
	uint64_t cardSize = ((fs::SDFS*) _fs)->cardSize();
	int cardSizeInMB = cardSize/(1024 * 1024);
 
	uint64_t bytesAvailable = ((fs::SDFS*) _fs)->totalBytes(); 
	int bytesAvailableInMB = bytesAvailable/(1024 * 1024);

	uint64_t bytesUsed = ((fs::SDFS*) _fs)->usedBytes();
	int bytesUsedInMB = bytesUsed/(1024 * 1024);
 
	LogX("jfs.%s: size_MB=%d, avail_MB=%d, used_MB=%d, perc=%d", get_tag(),
		cardSizeInMB, bytesAvailableInMB, bytesUsedInMB, bytesUsedInMB * 100 / bytesAvailableInMB);
}



#endif // PLATFORM_ESP32