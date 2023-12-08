#include "jFileRP2040.h"


#ifdef PLATFORM_RP2040

#if JFS_DEBUG==1
#define LogX Log
#else
#define LogX(...)
#endif


#include "jSystem.h"
#include "jTimeHelp.h"

//MUTEX_EXTERN(LOCK_SD)





#endif // PLATFORM_RP2040