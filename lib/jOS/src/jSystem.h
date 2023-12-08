#pragma once

#include "jDefines.h"

#include "jMultiTask.h"


extern void LogD(const String& msg);
extern void LogD(const char* format, ...) __attribute__((format(printf, 1, 2)));

// Log to EventLog not monitor
extern void LogR(const String& msg);
extern void LogR(const char* format, ...) __attribute__((format(printf, 1, 2)));

// Log to EventLog and Debug out
extern void Log(const String& msg);
extern void Log(const char* format, ...) __attribute__((format(printf, 1, 2)));

// Log to Monitor, Eventlog and Debug out
extern void LogM(const String& msg);
extern void LogM(const char* format, ...) __attribute__((format(printf, 1, 2)));


// get Current time as UNIX Timestamp in [s]
// number of seconds elapsed since 00:00 hours, Jan 1, 1970 UTC (i.e., a unix timestamp).
uint32_t getUNIXTimestamp();

extern uint64_t millis_epoch();


// returns 0 for Core0 (main core) and 1 for Core1 (second core)
uint8_t getCoreID();


// returns system voltage in [V] like 3.3V or 5V
float getSysVoltage();

#ifdef PLATFORM_ESP32

#define CPU_FREQ_240MHZ_WIFI_BT 240
#define CPU_FREQ_160MHZ_WIFI_BT 160
#define CPU_FREQ_80MHZ_WIFI_BT 80
#define CPU_FREQ_40MHZ 40
#define CPU_FREQ_26MHZ 26
#define CPU_FREQ_20MHZ 20
#define CPU_FREQ_13MHZ 13
#define CPU_FREQ_10MHZ 10

#endif // PLATFORM_ESP32

uint32_t getCPU_Freq();

void setCPU_Freq(uint32_t frq_mhz);


uint32_t jsystem_get_free_heap();


void jSystemRestart();


void _jdelay(uint32_t ms);

void jyield();



/******************************************
 *             GPIO FUNCTIONS             *
 * ***************************************/


// waits for pin to go to given level
// after timeout returns 0xFFFF
uint32_t expectPulse(uint8_t _pin, bool level, uint32_t timeout_us=1000);



/******************************************
 *          LOW LEVEL FUNCTIONS           *
 * ***************************************/

#include <stdarg.h>

// monolit function
String format(const char* format, ...) __attribute__((format(printf, 1, 2)));

// using s_vsnprintf
String format2(const char* format, ...) __attribute__((format(printf, 1, 2)));


String s_vsnprintf(const char* format, va_list arg);


#if defined(PLATFORM_AVR) || defined(PLATFORM_SAMD) || defined(PLATFORM_TEENSY) || defined(PLATFORM_ESP8266) || defined(PLATFORM_STM32)
String StringFromByteArray(const uint8_t* Data, uint16_t length);
String StringFromByteArray(const char* Data, uint16_t length);
#else
#define StringFromByteArray(Data, length) String(Data, length)
#endif 
