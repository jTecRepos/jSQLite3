#pragma once 

#include "jDefines.h"


#define UNIXTIME_DEFAULT 946688400LLU // Sat Jan 01 2000 01:00:00 GMT+0000

#define UNIXTIME_2018_START 1514764800LLU //[s] 01.01.2018 00:00:00 GMT+0000
#define UNIXTIME_2019_START 1546300800LLU //[s] 01.01.2019 00:00:00 GMT+0000
#define UNIXTIME_2020_START 1577836800LLU //[s] 01.01.2020 00:00:00 GMT+0000
#define UNIXTIME_2021_START 1609459200LLU //[s] 01.01.2021 00:00:00 GMT+0000
#define UNIXTIME_2022_START 1640995200LLU //[s] 01.01.2022 00:00:00 GMT+0000
#define UNIXTIME_2023_START 1672531200LLU //[s] 01.01.2023 00:00:00 GMT+0000
#define UNIXTIME_2024_START 1704067200LLU //[s] 01.01.2024 00:00:00 GMT+0000
#define UNIXTIME_2025_START 1735689600LLU //[s] 01.01.2025 00:00:00 GMT+0000
#define UNIXTIME_2026_START 1767225600LLU //[s] 01.01.2026 00:00:00 GMT+0000





// jDateTime (get everything at once) from JeeLabs / Adafruit
// Simple general-purpose date/time class (no TZ / DST / leap second handling!)
class jDateTime {
public:
	jDateTime (uint64_t epoch_time =0); // unix timestamp 
	jDateTime (
		uint16_t year, // years since 2000 or total year like 2022
		uint8_t month, // starting from 1 (januar)
		uint8_t day,
		uint8_t hour =0, 
		uint8_t min =0,
		uint8_t sec =0
	);

	//jDateTime (const char* date, const char* time);
	
	uint16_t year() const		{ return 2000 + yOff; }
	uint8_t month() const		{ return m; }
	uint8_t day() const			{ return d; }
	uint8_t hour() const		{ return hh; }
	uint8_t minute() const		{ return mm; }
	uint8_t second() const		{ return ss; }
	//uint8_t dayOfTheWeek() const;

	// 32-bit times as seconds since 1/1/2000
	//long secondstime() const;
	
	// 32-bit times as seconds since 1/1/1970
	// THE ABOVE COMMENT IS CORRECT FOR LOCAL TIME; TO USE THIS COMMAND TO
	// OBTAIN TRUE UNIX TIME SINCE EPOCH, YOU MUST CALL THIS COMMAND AFTER
	// SETTING YOUR CLOCK TO UTC
	uint64_t unixtime(void) const;

	String print_date() const;
	
	String print_timedate() const;
	
	String print_time() const;

protected:
	uint8_t yOff, m, d, hh, mm, ss;
};




/*******************************************
	*             Time Help Functions         *
	* ****************************************/

uint64_t calc_epoch_time(uint16_t Y, uint8_t M, uint8_t D, uint8_t h, uint8_t m, uint8_t s);





