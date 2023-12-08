#include "jTracker.h"

#include <jSystem.h>


/********************* jTrackerTraffic ***********************/

String jTrackerTraffic::print(const String& tag)
{
	uint16_t dt; uint32_t c; uint32_t rate;
	epoch(dt, c, rate);
	return format("%s: dt=%d, c=%d, rate=%d, total=%d", tag.c_str(), dt, c, rate, count_total);
}


/****************** jTrackerAvg ***********************/


String jTrackerTrafficCost::print(const String& tag) {
	uint16_t dt; uint32_t c; uint32_t rate;
	epoch(dt, c, rate);

	return format("%s: dt=%d, c=%d, rate=%d, total=%d, costph=%f", tag.c_str(), dt, c, rate, count_total, getCost_perHour());
}


/****************** jTrackerAvg ***********************/


String jTrackerAvg::print(const String& tag)
{
	float avg;
	epoch(avg);

	return format("%s: avg=%.2f, min=%d, max=%d, count=%d", tag.c_str(), 
		avg, 
		value_min, 
		value_max,
		last_count
	);
}

String jTrackerAvg::print(const String& tag, float display_factor)
{
	float avg;
	epoch(avg);

	return format("%s: avg=%.2f, min=%.2f, max=%.2f, count=%d", tag.c_str(), 
		avg * display_factor, 
		value_min * display_factor, 
		value_max * display_factor,
		last_count
	);
}



/*********************** jTrackerTransfer ****************************/


String jTrackerTransfer::print(const String& tag)
{
	uint16_t dt; uint32_t c; uint32_t rate;
	epoch(dt, c, rate);
	return format("%s: perc=%d, dt=%d, c=%d, rate=%d, total_c=%d, total_t=%d, total_rate=%d, max_dt=%.1f, max_len=%d", tag.c_str(), 
		target_count > 0 ? (100 * count_total / target_count) : 100,
		dt, 
		count, 
		rate, 
		count_total, 
		duration_total / FACTOR_ms_2_us, 
		get_total_rate(), 
		max_duration * FACTOR_us_2_ms, 
		max_count
	);
}


String jTrackerTransfer::print_total(const String& tag)
{
	return format("%s.total: perc=%d, rate=%d, len=%d, dt_ms=%d, took=%.1f, max_dt=%.1f, max_len=%d", tag.c_str(), 
		target_count > 0 ? (100 * count_total / target_count) : 100,
		get_total_rate(), 
		count_total, 
		MILLIS_DT(time_transfer_start, time_transfer_end), 
		duration_total * FACTOR_us_2_s, 
		max_duration * FACTOR_us_2_ms, 
		max_count
	);
}
