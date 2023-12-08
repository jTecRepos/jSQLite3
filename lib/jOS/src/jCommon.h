#pragma once

#include "jDefinesArchitecture.h"

#if defined(PLATFORM_RP2040) && !defined(__cplusplus)


/* Standard C library includes */
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>

typedef enum {
	LOW     = 0,
	HIGH    = 1,
	CHANGE  = 2,
	FALLING = 3,
	RISING  = 4,
} PinStatus;

typedef enum {
	INPUT           = 0x0,
	OUTPUT          = 0x1,
	INPUT_PULLUP    = 0x2,
	INPUT_PULLDOWN  = 0x3,
} PinMode;

#include "pgmspace.h"

//#define PROGMEM

#else

#include "Arduino.h" // Arduino support

#ifdef abs
#undef abs
#endif 
#ifdef max
#undef max
#endif 
#ifdef min
#undef min
#endif 

#ifndef bit
#define bit(b) (1UL << (b))    // Taken directly from Arduino.h
#endif

#ifdef PLATFORM_AVR
typedef uint32_t time_t;

#define jdelay delay

#endif // PLATFORM_AVR


#ifdef PLATFORM_ESP8266
//#include "core_version.h"

#if defined(__cplusplus)

#include <vector>
#define Vector std::vector

#endif


#define INPUT_PULLDOWN INPUT // pulldown not supported

#define jdelay delay

#endif // PLATFORM_ESP8266


#ifdef PLATFORM_ESP32
#include "core_version.h"
#ifdef VARIANTE_ESP32C3
#include "pgmspace.h"
#endif 

#if defined(__cplusplus)

#include <vector>
#define Vector std::vector

#endif
#define jdelay delay

#endif // PLATFORM_ESP32


#ifdef PLATFORM_STM32
//#include <avr/pgmspace.h>

#if defined(__cplusplus)

#include <vector>
#define Vector std::vector

#endif

#define jdelay delay

#define assert(...)

#endif // PLATFORM_STM32


#ifdef PLATFORM_SAMD

#if defined(__cplusplus)

#include <vector>
#define Vector std::vector

#endif

#define jdelay delay

#define assert(...)

#endif // PLATFORM_SAMD


#ifdef PLATFORM_RP2040

#ifdef VARIANTE_RP2040ALT


#undef PIN_SERIAL2_TX
#undef PIN_SERIAL2_RX

#undef PIN_SERIAL1_TX
#undef PIN_SERIAL1_RX

#undef PIN_SPI0_MISO
#undef PIN_SPI0_MOSI
#undef PIN_SPI0_SCK
#undef PIN_SPI0_SS


#endif 

#if defined(__cplusplus)

#include <vector>
#define Vector std::vector

#endif

#ifdef ENABLE_MULTITASK
//#define jdelay(ms) (_jdelay(ms))
#define jdelay delay
#else
#define jdelay delay
#endif 


#endif // PLATFORM_RP2040


#ifdef PLATFORM_TEENSY

#if defined(__cplusplus)

#include <vector>
#define Vector std::vector

#ifdef ENABLE_MULTITASK
#include <TeensyThreads.h>
#define jdelay(ms) (threads.delay(ms))
#else
#define jdelay delay
#endif 

#else
#define jdelay delay
#endif

#define assert(...)

#endif // PLATFORM_TEENSY









#if (ESP8266 || ESP32)
	#define ISR_PREFIX ICACHE_RAM_ATTR
#else
	#define ISR_PREFIX
#endif


#endif


#ifndef RTC_NOINIT_ATTR
	#define RTC_NOINIT_ATTR
#endif

#ifndef RTC_DATA_ATTR
	#define RTC_DATA_ATTR
#endif
