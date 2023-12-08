#pragma once


#ifndef ENABLE_JOS_DEBUG
    #define ENABLE_JOS_DEBUG 0
#endif // ENABLE_JOS_DEBUG


/*



Teensy 

// src: https://forum.pjrc.com/threads/68048-Predefined-Macros

ARDUINO_ARCH_AVR - Avr Architecture / (all Teensy < 3.0)
TEENSYDUINO=Version - Is a Teensy + gives TD version
KINETISL - Teensy LC
KINETISK - All Teensy 3.x
__MK20DX128__ - Teensy 3.0
__MK20DX256__ - Teensy 3.1, 3.2
__MK64FX512__ - Teensy 3.5
__MK66FX1M0__ - Teensy 3.6
__IMXRT1062__ - all Teensy 4.x + Teensy 4 MM
ARDUINO_TEENSY2 - Teensy 2
ARDUINO_TEENSY30 - Teensy 3.0
[...]
ARDUINO_TEENSY40 - Teensy 4.0
ARDUINO_TEENSY41 - Teensy 4.1
ARDUINO_TEENSYMM - Teensy 4 MM



// TEENSYDUINO has a port of Dean Camera's ATOMIC_BLOCK macros for AVR to ARM Cortex M3.
#define HAS_ATOMIC_BLOCK (defined(ARDUINO_ARCH_AVR) || defined(TEENSYDUINO))

// Whether we are running on either the ESP8266 or the ESP32.
#define ARCH_ESPRESSIF (defined(ARDUINO_ARCH_ESP8266) || defined(ARDUINO_ARCH_ESP32))

// Whether we are actually running on FreeRTOS.
#define IS_FREE_RTOS defined(ARDUINO_ARCH_ESP32)

// Define macro designating whether we're running on a reasonable
// fast CPU and so should slow down sampling from GPIO.
#define FAST_CPU \
	( \
	ARCH_ESPRESSIF || \
	defined(ARDUINO_ARCH_SAM)	 || defined(ARDUINO_ARCH_SAMD) || \
	defined(ARDUINO_ARCH_STM32)   || defined(TEENSYDUINO) \
	)

// https://arduino.github.io/arduino-cli/0.19/platform-specification/#hardware-folders-structure

#define PLATFORM_ESP32 defined(ARDUINO_ARCH_ESP32)
#define PLATFORM_STM32 defined(ARDUINO_ARCH_STM32)
#define PLATFORM_SAMD defined(ARDUINO_ARCH_SAMD)
#define PLATFORM_AVR defined(ARDUINO_ARCH_AVR)
#define PLATFORM_RP2040 defined(ARDUINO_ARCH_RP2040)


defined(__AVR_ATtiny25__) || defined(__AVR_ATtiny45__) || defined(__AVR_ATtiny85__) || \
	  defined(__AVR_ATtiny24__) || defined(__AVR_ATtiny44__) || defined(__AVR_ATtiny84__)
#elif defined(ARDUINO_AVR_ATTINYX5) || defined(ARDUINO_AVR_ATTINYX4)
#elif defined(__AVR_ATmega328P__)
#elif defined(__AVR_ATmega2560__) || defined(__AVR_ATmega1280__)
#elif defined(NRF52) || defined(NRF5)
*/

/* ESP Defines
__XTENSA__ : Xtensa processor type (ESP32 ESP32S3)
ESP32 : all esp32
ESP31B
ESP8266
*/

/*

//#ifdef ESP32
#ifdef PLATFORM_ESP32
//#warning "Process core is ESP32"

#include "Arduino.h" // Arduino support

#if CONFIG_IDF_TARGET_ESP32 // ESP32/PICO-D4
#define VARIANTE_ESP32
#elif CONFIG_IDF_TARGET_ESP32S2
#define VARIANTE_ESP32S2
#elif CONFIG_IDF_TARGET_ESP32S3
#define VARIANTE_ESP32S3
#elif CONFIG_IDF_TARGET_ESP32C3
#define VARIANTE_ESP32C3
#endif

//#if NUM_DIGITAL_PINS == 48
//#define VARIANTE_ESP32S3
//#endif // CONFIG_IDF_TARGET_ESP32S3


#endif
*/


#ifdef __IMXRT1062__
#define ARDUINO_TEENSY4X
#endif