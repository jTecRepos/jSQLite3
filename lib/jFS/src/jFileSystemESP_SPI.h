#pragma once

#ifdef PLATFORM_ESP32

#include "jFileSystem.h"
#include "jFileSystemESP.h"
#include "jFile.h"
#include "jFileESP.h"


class jFileSystemESPClass_SPI : public jFileSystemESPBaseClass
{
protected:

	int pin_cs;

public:
	jFileSystemESPClass_SPI(int _pin_cs);

	bool begin();

	void end();

	
	uint64_t size() override;

	uint64_t used() override;


	void printStats();


protected:

};


#endif // PLATFORM_ESP32