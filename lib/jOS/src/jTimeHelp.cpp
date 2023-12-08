#include "jTimeHelp.h"

#include "jSystem.h"

#define SECONDS_FROM_1970_TO_2000 946684800


// Utilities from JeeLabs/Ladyada

////////////////////////////////////////////////////////////////////////////////
// utility code, some of this could be exposed in the jDateTime API if needed

// DS3231 is smart enough to know this, but keeping it for now so I don't have
// to rewrite their code. -ADW
static const uint8_t daysInMonth [] PROGMEM = { 31,28,31,30,31,30,31,31,30,31,30,31 };

// number of days since 2000/01/01, valid for 2001..2099
static uint16_t date2days(uint16_t y, uint8_t m, uint8_t d) {
	if (y >= 2000)
		y -= 2000;
	uint16_t days = d;
	for (uint8_t i = 1; i < m; ++i)
		days += pgm_read_byte(daysInMonth + i - 1);
	if (m > 2 && y % 4 == 0)
		++days;
	return days + 365 * y + (y + 3) / 4 - 1;
}

static long time2long(uint16_t days, uint8_t h, uint8_t m, uint8_t s) {
	return ((days * 24L + h) * 60 + m) * 60 + s;
}

/*******************************************************************************
 * TO GET ALL DATE/TIME INFORMATION AT ONCE AND AVOID THE CHANCE OF ROLLOVER
 * jDateTime implementation spliced in here from Jean-Claude Wippler's (JeeLabs)
 * RTClib, as modified by Limor Fried (Ladyada); source code at:
 * https://github.com/adafruit/RTClib
 ******************************************************************************/

////////////////////////////////////////////////////////////////////////////////
// jDateTime implementation - ignores time zones and DST changes
// NOTE: also ignores leap seconds, see http://en.wikipedia.org/wiki/Leap_second

jDateTime::jDateTime (uint64_t epoch_time) {
	epoch_time -= SECONDS_FROM_1970_TO_2000; // bring to 2000 timestamp from 1970

	ss = epoch_time % 60;
	epoch_time /= 60;
	mm = epoch_time % 60;
	epoch_time /= 60;
	hh = epoch_time % 24;
	uint16_t days = epoch_time / 24;
	uint8_t leap;
	for (yOff = 0; ; ++yOff) {
		leap = yOff % 4 == 0;
		if (days < 365 + leap)
			break;
		days -= 365 + leap;
	}
	for (m = 1; ; ++m) {
		uint8_t daysPerMonth = pgm_read_byte(daysInMonth + m - 1);
		if (leap && m == 2)
			++daysPerMonth;
		if (days < daysPerMonth)
			break;
		days -= daysPerMonth;
	}
	d = days + 1;
}

jDateTime::jDateTime (uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t min, uint8_t sec) {
	if (year >= 2000)
		year -= 2000;
	yOff = year;
	m = month;
	d = day;
	hh = hour;
	mm = min;
	ss = sec;
}

static uint8_t conv2d(const char* p) {
	uint8_t v = 0;
	if ('0' <= *p && *p <= '9')
		v = *p - '0';
	return 10 * v + *++p - '0';
}

// UNIX time: IS CORRECT ONLY WHEN SET TO UTC!!!
uint64_t jDateTime::unixtime(void) const {
	uint64_t t;
	uint16_t days = date2days(yOff, m, d);
	t = time2long(days, hh, mm, ss);
	t += SECONDS_FROM_1970_TO_2000;  // seconds from 1970 to 2000

	return t;
}



String jDateTime::print_date() const
{
	return format("%02d.%02d.%02d", 
		this->day(), this->month(), this->year() % 100
	);
}

String jDateTime::print_timedate() const
{
	return format("%02d:%02d:%02d %02d.%02d.%02d", 
		this->hour(), this->minute(), this->second(),
		this->day(), this->month(), this->year() % 100
	);
}

String jDateTime::print_time() const
{
	return format("%02d:%02d:%02d", 
		this->hour(), this->minute(), this->second()
	);
}



// Slightly modified from JeeLabs / Ladyada
// Get all date/time at once to avoid rollover (e.g., minute/second don't match)
static uint8_t bcd2bin (uint8_t val) { return val - 6 * (val >> 4); }
static uint8_t bin2bcd (uint8_t val) { return val + 6 * (val / 10); }





/*******************************************
*             Time Help Functions         *
* ****************************************/



// calculate epoch time from time and date
// Y: year like 2020 or 20
// M: month like 1 for January
uint64_t calc_epoch_time(uint16_t Y, uint8_t M, uint8_t D, uint8_t h, uint8_t m, uint8_t s) {
	/*
#ifdef PLATFORM_ESP32
	time_t time_epoch;
	struct tm t;
	
	t.tm_year = (Y > 1000) ? (Y % 100) + 100 : Y; // Years since 1900
	t.tm_mon = M - 1;	// Month, 0 - jan
	t.tm_mday = D;		// Day of the month
	t.tm_hour = h;
	t.tm_min = m;
	t.tm_sec = s;
	time_epoch = mktime(&t);
	return time_epoch;
#else
	*/
	jDateTime dt = jDateTime(Y, M, D, h, m, s);
	return dt.unixtime();

//#endif // PLATFORM_ESP32

	//return 0;
}
