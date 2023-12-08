#include "jLogFormat.h"

/******************************************* LogfileHelp ********************************************************************/

//TODO make this new library and put all code from 
// - src/Storage/LogfileHelp 
// - src/Defines_Logfile
// there

// String Log
#define TAG_STRING_LOG 'l'
	#define TAG_STRING_LOG_ID_ELOG 0 // equivalent to LogR - text contains what would go to eventlog
	#define TAG_STRING_LOG_ID_MONITOR 1 // equivalent to LogM - text is forwarded to connected monitor channels
	#define TAG_STRING_LOG_ID_DLOG 2 // equivalent to LogD - not yet used
	#define TAG_STRING_LOG_ID_DEBUG 3 // can be freely used to test connection or log debug data


// Bytes Log
#define TAG_BYTES_LOG 'n'
	#define TAG_BYTES_LOG_ID_SERIAL_AT_TX 1
	#define TAG_BYTES_LOG_ID_SERIAL_AT_RX 2
	#define TAG_BYTES_LOG_ID_SERIAL_GPS_TX 3
	#define TAG_BYTES_LOG_ID_SERIAL_GPS_RX 4
	#define TAG_BYTES_LOG_ID_RTCM3 5 // rtcm3 datastream for rtk correction 
	#define TAG_BYTES_LOG_ID_DEBUG 6 // can be freely used to test connection or log debug data




extern void StoreString_ID(char tag, uint16_t id, const String& msg);


extern void StoreBytes_ID(char tag, uint16_t id, const uint8_t* data, uint16_t length);



/***************************************************************************************************************/





void StoreStringLog(LOG_STRING_CHANNEL channel, const String& msg)
{
	StoreString_ID(TAG_STRING_LOG, (uint16_t) channel, msg);
}

void StoreBytesLog(LOG_BYTES_CHANNEL channel, const uint8_t* data, uint16_t length)
{
	StoreBytes_ID(TAG_BYTES_LOG, (uint16_t) channel, data, length);
}


void StoreBytesLog(LOG_BYTES_CHANNEL channel, const char* str_data)
{
	String s = String(str_data);
	StoreBytes_ID(TAG_BYTES_LOG, (uint16_t) channel, (const uint8_t*) s.c_str(), s.length());
}

