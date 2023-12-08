
#pragma once

#include <jCommon.h>


enum class LOG_STRING_CHANNEL : uint16_t {
	ELOG = 0,
	MONITOR = 1,
	DLOG = 2,
	DEBUG = 3
};

enum class LOG_BYTES_CHANNEL : uint16_t {
	SERIAL_AT_TX = 1,
	SERIAL_AT_RX = 2,
	SERIAL_GPS_TX = 3,
	SERIAL_GPS_RX = 4,
	RTCM3 = 5,
	DEBUG = 6
};


void StoreStringLog(LOG_STRING_CHANNEL channel, const String& msg);


void StoreBytesLog(LOG_BYTES_CHANNEL channel, const uint8_t* data, uint16_t length);


void StoreBytesLog(LOG_BYTES_CHANNEL channel, const char* str_data);

