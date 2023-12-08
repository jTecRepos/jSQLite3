#pragma once

#ifdef PLATFORM_ESP32

#include "jFileSystem.h"
#include "jFileSystemESP.h"
#include "jFile.h"
#include "jFileESP.h"


class jFileSystemESPClass_SPIFFS : public jFileSystemESPBaseClass
{
protected:


public:
	jFileSystemESPClass_SPIFFS();

	bool begin()
	{
		return begin(true);
	}

	bool begin(bool formatiffail);

	void end();

	bool isMounted() override;

	
	uint64_t size() override;

	uint64_t used() override;


	void printStats();


protected:

	bool is_mounted = false;

};


#endif // PLATFORM_ESP32