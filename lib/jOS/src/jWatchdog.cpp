#include "jWatchdog.h"

#include "jSystem.h"

//[ms] Watchdog Timer Reset Timeout Duration
// set to 0 to disable
static uint32_t wdt_timeout = 0; 



#if defined(PLATFORM_ESP32)
#include <esp_task_wdt.h>
#endif // PLATFORM_ESP32

#if defined(PLATFORM_SAMD) || defined(PLATFORM_AVR)
// https://github.com/adafruit/Adafruit_SleepyDog
#include <Adafruit_SleepyDog.h>
#endif // PLATFORM_SAMD || PLATFORM_AVR

#ifdef PLATFORM_RP2040
// https://github.com/raspberrypi/pico-examples/blob/master/watchdog/hello_watchdog/hello_watchdog.c
extern "C" {
#include <hardware/watchdog.h>
};

//untested
//#include <mbed.h>
//mbed::Watchdog &watchdog = mbed::Watchdog::get_instance();

#endif // PLATFORM_RP2040

#ifdef PLATFORM_TEENSY

#include "Watchdog_t4.h"
static WDT_T4<WDT1> wdt;

/*
void wdt_callback() {
	LogD("jwdt.msg: wdt resets soon");
}
*/
//#include <Watchdog.h>
//static Watchdog watchdog;
#endif // PLATFORM_TEENSY

#ifdef PLATFORM_STM32
	
#endif // PLATFORM_STM32



#ifdef PLATFORM_ESP8266

// https://github.com/esp8266/Arduino/issues/1532

#include <Ticker.h>
static Ticker ticker_wdt;
static uint32_t last_time_wd_reset = 0;

void ICACHE_RAM_ATTR isr_ticker_wdt(void) {
	// check if last reset is too long ago
	if(wdt_timeout > 0 && (millis() - last_time_wd_reset) >= wdt_timeout)
	{
		// save the hit here to eeprom or to rtc memory if needed
		ESP.restart();  // normal reboot 
		//ESP.reset();  // hard reset
	}
}


#endif // PLATFORM_ESP8266








uint16_t jWatchdog_getTimeout()
{
	return wdt_timeout;
}


void jWatchdog_enable(uint32_t timeout_ms)
{
	wdt_timeout = timeout_ms;


#if defined(PLATFORM_ESP32)
	esp_task_wdt_init(timeout_ms / 1000, true); // enable panic so ESP32 restarts
	//esp_task_wdt_add(NULL); // add current thread to WDT watch
#endif // PLATFORM_ESP32

#if defined(PLATFORM_ESP8266)
	//ESP.wdtEnable(timeout_ms);

	last_time_wd_reset = millis(); // reset wdt
	if(timeout_ms > 0)
		ticker_wdt.attach_ms(LIMIT(100, timeout_ms / 3, 1000), isr_ticker_wdt);

#endif // PLATFORM_ESP32

#if defined(PLATFORM_SAMD) || defined(PLATFORM_AVR)
	Watchdog.enable(timeout_ms);
#endif // PLATFORM_SAMD || PLATFORM_AVR

#ifdef PLATFORM_RP2040
	watchdog_enable(timeout_ms, 1);
	//watchdog.start(timeout_ms); //untested
#endif // PLATFORM_RP2040

#ifdef PLATFORM_TEENSY
	
	WDT_timings_t config;
	config.lp_suspend = true;
	
	//config.trigger = 5; // in seconds, 0->128 
	//config.timeout = 5; 
	config.timeout = timeout_ms * FACTOR_ms_2_s; // in seconds, 0->128
	//config.callback = wdt_callback;
	wdt.begin(config);
	
	// only allowes full seconds
	//watchdog.enable((Watchdog::Timeout) timeout_ms);
#endif // PLATFORM_TEENSY
	
#ifdef PLATFORM_STM32
	
#endif // PLATFORM_STM32

}

void jWatchdog_disable()
{
	wdt_timeout = 0;
	
#if defined(VARIANTE_ESP32) || defined(VARIANTE_ESP32S3)
	disableCore0WDT();
	disableCore1WDT();
	disableLoopWDT();
#endif // VARIANTE_ESP32
	
#if defined(VARIANTE_ESP32C3)
	disableCore0WDT();
	disableLoopWDT();
#endif // VARIANTE_ESP32C3

#if defined(PLATFORM_ESP8266)
	//ESP.wdtDisable();

	ticker_wdt.detach();
#endif // PLATFORM_ESP32

#if defined(PLATFORM_SAMD) || defined(PLATFORM_AVR)
	Watchdog.disable();
#endif // PLATFORM_SAMD || PLATFORM_AVR

#ifdef PLATFORM_RP2040
	//watchdog.stop(); //untested
#endif // PLATFORM_RP2040

#ifdef PLATFORM_TEENSY
	//wdt.feed();
#endif // PLATFORM_TEENSY

#ifdef PLATFORM_STM32
	
#endif // PLATFORM_STM32
}


void jWatchdog_reset()
{
#if defined(PLATFORM_ESP32)
	esp_task_wdt_reset();
	//esp_err_t res = esp_task_wdt_reset();
	//LogD("wdt.reset: res=%d", res);
#endif // PLATFORM_ESP32

#if defined(PLATFORM_ESP8266)
	//ESP.wdtFeed();
	
	last_time_wd_reset = millis(); // reset wdt
#endif // PLATFORM_ESP32

#if defined(PLATFORM_SAMD) || defined(PLATFORM_AVR)
	Watchdog.reset();
#endif // PLATFORM_SAMD || PLATFORM_AVR

#ifdef PLATFORM_RP2040
	//TODO implement multicore support
	if(getCoreID() != 0)
		return;

	watchdog_update();
	//watchdog.kick(); // untested
#endif // PLATFORM_RP2040

#ifdef PLATFORM_TEENSY
	wdt.feed();
	//watchdog.reset();
#endif // PLATFORM_TEENSY

#ifdef PLATFORM_STM32
	
#endif // PLATFORM_STM32
}

void jWatchdog_delay(uint32_t ms)
{
	uint32_t t_start = millis();
	uint32_t last_time_wdt_reset = 0;
	while(millis() - t_start < ms)
	{
		EXECUTE_EVERY_NAME(last_time_wdt_reset, 25)
			jWatchdog_reset();
		EXECUTE_EVERY_END
		
		jdelay(1);
	}
}