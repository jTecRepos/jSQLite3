#include "jSerial_RP2040.h"

#ifdef PLATFORM_RP2040

jSerialHard::~jSerialHard()
{
#ifdef VARIANTE_RP2040MBED
	if(_ser_created)
	{
		delete ser;
	}
#endif 
}

#endif // PLATFORM_RP2040
