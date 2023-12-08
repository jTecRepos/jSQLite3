#include "jSystem.h"


uint32_t getUNIXTimestamp() {
	return uint32_t ( millis_epoch() / 1000LLU );
}

/*
uint64_t millis_epoch() {
	// TODO make this sub second accurate
	return ((uint64_t) getUNIXTimestamp()) * 1000LLU;
}
*/


#ifdef PLATFORM_AVR
#include <avr/wdt.h>
#endif // PLATFORM_AVR

// returns 0 for Core0 (main core) and 1 for Core1 (second core)
uint8_t getCoreID()
{
#ifdef PLATFORM_RP2040
#ifdef VARIANTE_RP2040ALT
	return get_core_num();
#else
	uint32_t reg = *((uint32_t*) SIO_BASE);
	return (uint8_t) reg;
#endif 
	
#endif // PLATFORM_RP2040
#ifdef PLATFORM_ESP32
	// xTaskGetCurrentTaskHandle(); -> maybe usefull for multi task 
	return xPortGetCoreID();
#endif // PLATFORM_ESP32
#ifdef PLATFORM_ESP32_C3
	return 0; // ESP32-C3 only has a single core
#endif // PLATFORM_ESP32
	return 0;
}

float getSysVoltage()
{
#ifdef PLATFORM_AVR
	return 5.0f;
#else 
	return 3.3f;
#endif
}


#ifdef PLATFORM_RP2040
//extern "C" {
//#include "stdlib.h"
//};
static inline bool set_sys_clock_khz(uint32_t freq_khz, bool required);
#endif // PLATFORM_RP2040

void setCPU_Freq(uint32_t frq_mhz)
{
#ifdef PLATFORM_ESP32
	setCpuFrequencyMhz(frq_mhz); 
#endif // PLATFORM_ESP32

#ifdef PLATFORM_RP2040
	//set_sys_clock_khz(60000, true); //60 Mhz
	set_sys_clock_khz(frq_mhz * 1000, false); //60 Mhz
#endif // PLATFORM_RP2040
}


uint32_t getCPU_Freq()
{
#if defined(PLATFORM_ESP32)
	//return getCpuFrequencyMhz(); // Get CPU clock
	return ESP.getCpuFreqMHz();
#endif // PLATFORM_ESP32
#if defined(PLATFORM_ESP8266)
	return ESP.getCpuFreqMHz();
#endif // PLATFORM_ESP32
#ifdef PLATFORM_STM32
	return F_CPU;
#endif // PLATFORM_STM32
#ifdef PLATFORM_SAMD
	return F_CPU;
#endif // PLATFORM_SAMD
#ifdef PLATFORM_AVR
	return F_CPU;
#endif // PLATFORM_AVR
	
	return 0;
}

#ifdef PLATFORM_TEENSY

// src: //https://forum.pjrc.com/threads/33443-How-to-display-free-ram
extern unsigned long _heap_start;
extern unsigned long _heap_end;
extern char *__brkval;
#endif // PLATFORM_TEENSY

//TODO test this for arm free ram
/*
extern "C" char* sbrk(int incr);
static int FreeRam() {
	char top;
	return &top - reinterpret_cast<char*>(sbrk(0));
}
*/

uint32_t jsystem_get_free_heap()
{
#ifdef PLATFORM_ESP32
	//return ESP.getFreeHeap();
	return heap_caps_get_free_size(MALLOC_CAP_8BIT);
#endif // PLATFORM_ESP32

#ifdef PLATFORM_ESP8266
	return ESP.getFreeHeap();
#endif // PLATFORM_ESP8266

#ifdef PLATFORM_STM32
	//TODO https://github.com/stm32duino/STM32Examples/blob/main/examples/Benchmarking/MemoryAllocationStatistics/MemoryAllocationStatistics.ino
	//return System.freeMemory();
#endif // PLATFORM_STM32

#ifdef PLATFORM_TEENSY
	
#ifdef ARDUINO_TEENSY4X
	return (char *)&_heap_end - __brkval;
#else

	// //https://forum.pjrc.com/threads/33443-How-to-display-free-ram
	uint32_t stackTop;
	uint32_t heapTop;

	// current position of the stack.
	stackTop = (uint32_t) &stackTop;

	// current position of heap.
	void* hTop = malloc(1);
	heapTop = (uint32_t) hTop;
	free(hTop);

	// The difference is (approximately) the free, available ram.
	return stackTop - heapTop;

#endif 
#endif // PLATFORM_TEENSY
	//return FreeRam();
	return 0;
}

void jSystemRestart()
{
#if defined(PLATFORM_ESP32) || defined(PLATFORM_ESP8266)
	ESP.restart();
#endif // PLATFORM_ESP32
#ifdef PLATFORM_AVR
	// https://arduino.stackexchange.com/questions/61180/avr-software-reset-without-watchdog
	wdt_enable(WDTO_15MS);for(;;);// (this way enters recovery mode)
	//or
	//  asm ("jmp 0x0");// (this way function registers not being resetted)
#endif // PLATFORM_AVR
#if defined(PLATFORM_SAMD) || defined(PLATFORM_RP2040)
	NVIC_SystemReset();
#endif // ARM BASED
}


void _jdelay(uint32_t ms)
{
	
#ifdef PLATFORM_RP2040
	busy_wait_us(ms * 1000);
#else
	delay(ms);
#endif 
}

void jyield()
{
	yield();
}


/******************************************
 *             GPIO FUNCTIONS             *
 * ***************************************/


uint32_t expectPulse(uint8_t _pin, bool level, uint32_t timeout_us) 
{
	// Check this:
	// https://www.arduino.cc/reference/de/language/functions/advanced-io/pulseinlong/
	
	uint32_t time0 = micros();
	while (digitalRead(_pin) == level) {
		if(micros() - time0 > timeout_us) {
			return 0xFFFF; // Exceeded timeout, fail.
		}
	}
	
	return micros() - time0;
}


/******************************************
 *          LOW LEVEL FUNCTIONS           *
 * ***************************************/


String format(const char* format, ...)
{
	char loc_buf[64];
	char* temp = loc_buf;
	va_list arg;
	va_list copy;
	va_start(arg, format);
	va_copy(copy, arg);
	int len = vsnprintf(temp, sizeof(loc_buf), format, copy);
	va_end(copy);
	if (len < 0) {
		va_end(arg);
		return "";
	};
	if (len >= sizeof(loc_buf)) {
		temp = (char*)malloc(len + 1);
		if (temp == NULL) {
			va_end(arg);
			return "";
		}
		len = vsnprintf(temp, len + 1, format, arg);
	}
	va_end(arg);
	
	String s = String(temp);
	
	if (temp != loc_buf) {
		free(temp);
	}
	return s;
}

String format2(const char* format, ...)
{
	va_list arg;
	va_start(arg, format);
	String s = s_vsnprintf(format, arg);
	va_end(arg);
	
	return s;
}


String s_vsnprintf(const char* format, va_list arg)
{
	char loc_buf[64];
	char* temp = loc_buf;
	
	va_list copy;
	va_copy(copy, arg);
	int len = vsnprintf(temp, sizeof(loc_buf), format, copy);
	va_end(copy);
	if (len < 0) {
		return "";
	};
	if (len >= sizeof(loc_buf)) {
		temp = (char*)malloc(len + 1);
		if (temp == NULL) {
			return "";
		}
		len = vsnprintf(temp, len + 1, format, arg);
	}
	
	String s = String(temp);

	if (temp != loc_buf) {
		free(temp);
	}
	return s;
}




#if defined(PLATFORM_AVR) || defined(PLATFORM_SAMD) || defined(PLATFORM_TEENSY) || defined(PLATFORM_ESP8266) || defined(PLATFORM_STM32)
String StringFromByteArray(const uint8_t* Data, uint16_t length) {
	String value = "";
	value.reserve(length);
	for(int i = 0; i < length; i++)
		value += (char) Data[i];

	return value;
}
String StringFromByteArray(const char* Data, uint16_t length) {
	String value = "";
	value.reserve(length);
	for(int i = 0; i < length; i++)
		value += (char) Data[i];

	return value;
}

#endif 


