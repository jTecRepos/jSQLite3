#pragma once

#include "jDefines.h"


//get current timeout duration of WDT in [ms]
// return 0 if not enabled
uint16_t jWatchdog_getTimeout();

void jWatchdog_enable(uint32_t timeout_ms);

void jWatchdog_disable();

void jWatchdog_reset();

// delay function - allows longer waits without triggering wdt timeout
void jWatchdog_delay(uint32_t ms);