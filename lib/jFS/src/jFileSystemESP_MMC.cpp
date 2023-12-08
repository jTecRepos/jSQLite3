#include "jFileSystemESP_MMC.h"

//#ifdef SOC_SDMMC_HOST_SUPPORTED
#if defined(VARIANTE_ESP32) || defined(VARIANTE_ESP32S3)


#if JFS_DEBUG==1
#define LogX Log
#define LogXD LogD
#else
#define LogX(...)
#define LogXD(...)
#endif


#include <jSystem.h>

// https://docs.espressif.com/projects/esp-idf/en/latest/esp32s3/api-reference/peripherals/sdmmc_host.html
#include "SD_MMC.h"


#ifdef VARIANTE_ESP32
// SD - MMC
#define PIN_SD_CLK 14 // SCK
#define PIN_SD_CMD 15 // MOSI
#define PIN_SD_DAT0 2 // MISO
#define PIN_SD_DAT1 4
#define PIN_SD_DAT2 12
#define PIN_SD_DAT3 13
#endif

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

jFileSystemESPClass_MMC::jFileSystemESPClass_MMC(bool bitmode1)
 : jFileSystemESPBaseClass("sd_mmc", &SD_MMC), bitmode1(bitmode1)
{

}


#ifdef VARIANTE_ESP32S3

jFileSystemESPClass_MMC::jFileSystemESPClass_MMC(int8_t pin_clk, int8_t pin_cmd, int8_t pin_d0)
 : jFileSystemESPBaseClass("sd_mmc", &SD_MMC)
{
	bitmode1 = true;
	SD_MMC.setPins(pin_clk, pin_cmd, pin_d0);
}

jFileSystemESPClass_MMC::jFileSystemESPClass_MMC(int8_t pin_clk, int8_t pin_cmd, int8_t pin_d0, int8_t pin_d1, int8_t pin_d2, int8_t pin_d3)
 : jFileSystemESPBaseClass("sd_mmc", &SD_MMC)
{
	bitmode1 = false;
	SD_MMC.setPins(pin_clk, pin_cmd, pin_d0, pin_d1, pin_d2, pin_d3);
}

#endif // VARIANTE_ESP32S3



bool jFileSystemESPClass_MMC::begin()
{
	LogX("jfs.%s: begin", get_tag());

	bool b = false;

#ifdef VARIANTE_ESP32
	//pinMode(PIN_SD_CLK, PULLUP);   // 14
	//pinMode(PIN_SD_CMD, PULLUP);   // 15
	//pinMode(PIN_SD_DAT0, PULLUP);  // 2
	//pinMode(PIN_SD_DAT1, PULLUP);  // 4
	pinMode(PIN_SD_DAT2, PULLUP);  // 12
	//pinMode(PIN_SD_DAT3, PULLUP);  // 13
#endif
	
	//b = SD_MMC.begin("/cdcard", bitmode1);
	b = SD_MMC.begin("/cdcard", bitmode1, false);
	//b = SD_MMC.begin("/cdcard", bitmode1, false, SDMMC_FREQ_DEFAULT);
	//b = SD_MMC.begin("/cdcard", bitmode1, false, SDMMC_FREQ_HIGHSPEED);
	//b = SD_MMC.begin();

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


void jFileSystemESPClass_MMC::end()
{
	LogX("jfs.%s: end", get_tag());
	((fs::SDMMCFS*) _fs)->end();
}



uint64_t jFileSystemESPClass_MMC::size() {
	//return ((fs::SDMMCFS*) _fs)->cardSize();
	return ((fs::SDMMCFS*) _fs)->totalBytes();
}


uint64_t jFileSystemESPClass_MMC::used() {
	return ((fs::SDMMCFS*) _fs)->usedBytes();
}


void jFileSystemESPClass_MMC::printStats()
{
	uint8_t cardType = ((fs::SDMMCFS*) _fs)->cardType();
 
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
 
	 
	uint64_t cardSize = ((fs::SDMMCFS*) _fs)->cardSize();
	int cardSizeInMB = cardSize/(1024 * 1024);
 
	uint64_t bytesAvailable = ((fs::SDMMCFS*) _fs)->totalBytes(); 
	int bytesAvailableInMB = bytesAvailable/(1024 * 1024);

	uint64_t bytesUsed = ((fs::SDMMCFS*) _fs)->usedBytes();
	int bytesUsedInMB = bytesUsed/(1024 * 1024);
 
	LogX("jfs.%s: size_MB=%d, avail_MB=%d, used_MB=%d, perc=%d", get_tag(),
		cardSizeInMB, bytesAvailableInMB, bytesUsedInMB, bytesUsedInMB * 100 / bytesAvailableInMB);
}


#endif // PLATFORM_ESP32