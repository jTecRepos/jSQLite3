#pragma once

//#ifdef SOC_SDMMC_HOST_SUPPORTED
#if defined(VARIANTE_ESP32) || defined(VARIANTE_ESP32S3)

#include "jFileSystem.h"
#include "jFileSystemESP.h"
#include "jFile.h"
#include "jFileESP.h"


class jFileSystemESPClass_MMC : public jFileSystemESPBaseClass
{
protected:

	bool bitmode1 = false;

public:

	jFileSystemESPClass_MMC(bool bitmode1);

#ifdef VARIANTE_ESP32S3
	
	// for 1bit Mode (variable pins only supported by ESP32S3)
	jFileSystemESPClass_MMC(int8_t pin_clk, int8_t pin_cmd, int8_t pin_d0);

	// for 4bit Mode (variable pins only supported by ESP32S3)
	jFileSystemESPClass_MMC(int8_t pin_clk, int8_t pin_cmd, int8_t pin_d0, int8_t pin_d1, int8_t pin_d2, int8_t pin_d3);

#endif // VARIANTE_ESP32S3

	bool begin();

	void end();

	
	uint64_t size() override;

	uint64_t used() override;


	void printStats();


protected:

};


#endif // PLATFORM_ESP32