#include "jFileTeensy.h"


#ifdef PLATFORM_TEENSY

#if JFS_DEBUG==1
#define LogX Log
#else
#define LogX(...)
#endif


#include "jSystem.h"
#include "jTimeHelp.h"

//MUTEX_EXTERN(LOCK_SD)





#endif // PLATFORM_TEENSY