#pragma once

#include "jCommon.h"

#ifndef ULONG_MAX
	#define ULONG_MAX 0xFFFFFFFF
#endif

#ifndef MIN
	#define MIN(a,b) ((a)<(b)?(a):(b))
#endif
#ifndef MAX
	#define MAX(a,b) ((a)>(b)?(a):(b))
#endif


#define LIMIT(LOWER, X, UPPER) ((X)<(LOWER)?(LOWER):((X)>(UPPER)?(UPPER):(X)))
#define MEDIAN3(a,b,c) (MAX(MIN(a,b), MIN(MAX(a,b),c)))
#define MAP(value, FROM_MIN, FROM_MAX, TO_LOW, TO_HIGH) ((LIMIT(FROM_MIN, value, FROM_MAX) - FROM_MIN) * (TO_HIGH - TO_LOW) / (FROM_MAX - FROM_MIN) + TO_LOW)
#define ABS(value) ((value) < 0 ? -(value) : (value))

// calculates the amount of chunks that are needed for the given length.
// example: LENGTH_IN_CHUNKS(30, 8) => 4
#define LENGTH_IN_CHUNKS(element_len, chunk_len) (((element_len) / (chunk_len)) + (((element_len) % (chunk_len) == 0) ? 0 : 1)) 


// set and reset a bit inside a byte[]
#define BYTE_ARRAY_BIT_SET(byte_array, bitnum) byte_array[bitnum / 8] = byte_array[bitnum / 8] | (1 << (bitnum % 8))
#define BYTE_ARRAY_BIT_RESET(byte_array, bitnum) byte_array[bitnum / 8] = byte_array[bitnum / 8] & ~(1 << (bitnum % 8))
#define BYTE_ARRAY_BIT_READ(byte_array, bitnum) ((byte_array[bitnum / 8] & (1 << (bitnum % 8))) != 0)

#define FLIP_UINT16_LSB_MSB(X2) ( (((X2) & 0xFF) << 8) | (((X2) >> 8) & 0xFF) )
#define JOIN_UINT16_LSB_MSB(U8L, U8H) (((uint16_t) (U8L)) | ((uint16_t)(U8H)) << 8)


// this might be ajusted when system supports INT64 timestamps
#define _MICROS_MAX 0xFFFFFFFF
#define _MILLIS_MAX 0xFFFFFFFF

// calculate micros delta - overflow save
//#define MICROS_DT(t_from, t_to) t_to - t_from
#define MICROS_DT(t_from, t_to) ((t_from > t_to) ? (_MICROS_MAX - t_from) + t_to : t_to - t_from)

// calculate millis delta - overflow save
//#define MILLIS_DT(t_from, t_to) t_to - t_from
#define MILLIS_DT(t_from, t_to) ((t_from > t_to) ? (_MILLIS_MAX - t_from) + t_to : t_to - t_from)
#define MILLIS_SINCE(t) MILLIS_DT(t, millis())

// convert millis timestamp to epoch timestamp in milliseconds
#define MILLIS_2_MILLIS_EPOCH(t_millis) (millis_epoch() - ((int64_t) MILLIS_DT(t_millis, millis())))



// put this at start of a loop that is regularly called. limits execution to given looptime
#define LIMIT_LOOP_RATE(looptime_ms) static uint32_t _last_time_loop = 0; \
	if (millis() - _last_time_loop < (looptime_ms)) \
		return; \
	_last_time_loop = millis();

#define LIMIT_LOOP_RATE_DT(looptime_ms, dt_variable) static uint32_t _last_time_loop = 0; \
	if (millis() - _last_time_loop < (looptime_ms)) \
		return; \
	uint32_t dt_variable = MILLIS_DT(_last_time_loop, millis()); \
	_last_time_loop = millis();


//TODO implement EXECUTE_EVERY_RND with 
// static uint32_t _last_time init as millis() - ms + ((uint32_t) (rnd() * ms))

// Version creates a local variable
#define EXECUTE_EVERY(ms) { \
	static uint32_t _last_time = 0; \
	if (millis() - _last_time > (ms)) { \
		_last_time = millis();

// Version takes the name of an existing variable 
#define EXECUTE_EVERY_NAME(last_time_variable, ms) { \
	if (millis() - last_time_variable > (ms)) { \
		last_time_variable = millis();


// This Version keeps trys to keep the avg period
/*
#define EXECUTE_EVERY2(ms) { \
	static uint32_t _last_time = 0; \
	if (millis() - _last_time > (ms)) { \
		if(millis() - _last_time > 2 * ms) _last_time = millis(); \
		else _last_time += (ms); \
*/
#define EXECUTE_EVERY2(ms) { \
	static uint32_t _last_time = 0; \
	if (millis() - _last_time > (ms)) { \
		_last_time = MAX(millis() - (ms), _last_time + (ms));

/*
#define EXECUTE_EVERY2_NAME(last_time_variable, ms) { \
	if (millis() - last_time_variable > (ms)) { \
		if(millis() - last_time_variable > 2 * ms) last_time_variable = millis(); \
		else last_time_variable += (ms); \
*/
#define EXECUTE_EVERY2_NAME(last_time_variable, ms) { \
	if (millis() - last_time_variable > (ms)) { \
		last_time_variable = MAX(millis() - (ms), last_time_variable + (ms));

#define EXECUTE_EVERY_END }}
#define EXECUTE_EVERY_RESET _last_time = millis();

// tracks microseconds time between STARTX and ENDX. result in param dt
#define TRACK_DT_MICROS_START0 uint32_t _tm0s = micros();
#define TRACK_DT_MICROS_END0(dt) uint32_t _tm0e = micros(); int32_t dt = (_tm0s > _tm0e) ? (_MICROS_MAX - _tm0s) + _tm0e : _tm0e - _tm0s;

#define TRACK_DT_MICROS_START1 uint32_t _tm1s = micros();
#define TRACK_DT_MICROS_END1(dt) uint32_t _tm1e = micros(); int32_t dt = (_tm1s > _tm1e) ? (_MICROS_MAX - _tm1s) + _tm1e : _tm1e - _tm1s;

#define TRACK_DT_MICROS_START2 uint32_t _tm2s = micros();
#define TRACK_DT_MICROS_END2(dt) uint32_t _tm2e = micros(); int32_t dt = (_tm2s > _tm2e) ? (_MICROS_MAX - _tm2s) + _tm2e : _tm2e - _tm2s;

// tracks miliseconds time between STARTX and ENDX. result in param dt //TODO make overflow save
#define TRACK_DT_MILLIS_START0 uint32_t _t0s = millis();
#define TRACK_DT_MILLIS_END0(dt) uint32_t _t0e = millis(); int32_t dt = _t0e - _t0s;

#define TRACK_DT_MILLIS_START1 uint32_t _t1s = millis();
#define TRACK_DT_MILLIS_END1(dt) uint32_t _t1e = millis(); int32_t dt = _t1e - _t1s;

#define TRACK_DT_MILLIS_START2 uint32_t _t2s = millis();
#define TRACK_DT_MILLIS_END2(dt) uint32_t _t2e = millis(); int32_t dt = _t2e - _t2s;

#define TRACK_EXECUTE_DT_MS(dt_ms) static uint32_t _texlastms = millis(); \
	int32_t dt_ms = millis() - _texlastms; \
	_texlastms = millis();

#define TRACK_EXECUTE_DT_MS_FLOAT(dt_ms) static uint32_t _texlastms = micros(); \
	float dt_ms = (micros() - _texlastms) * FACTOR_us_2_ms; \
	_texlastms = micros();

#define TRACK_EXECUTE_DT_S(dt_s) static uint32_t _texlasts = millis(); \
	float dt_s = (millis() - _texlasts) * FACTOR_ms_2_s; \
	_texlasts = millis();

#define const_g 9.81



#ifdef PI
	#undef PI
#endif 
#ifdef TWO_PI
	#undef TWO_PI
#endif 

//TODO rename to CONST_XXX
#define PI (3.1415926535897932384626433832795f)
//#define HALF_PI	 1.5707963267948966192313216916398
#define TWO_PI (6.283185307179586476925286766559f)
//#define DEG_TO_RAD  0.017453292519943295769236907684886
//#define RAD_TO_DEG  57.295779513082320876798154814105
//#define EULER	   2.718281828459045235360287471352
#define CONST_G 9.81f


// FACTOR TIME
#define FACTOR_ns_2_ms (1e-6f)
#define FACTOR_ns_2_s (1e-9f)
#define FACTOR_s_2_h (3600)
#define FACTOR_s_2_us (1000 * 1000)
#define FACTOR_min_2_s (60)
#define FACTOR_hour_2_s (3600)
#define FACTOR_day_2_s (24 * 3600)
#define FACTOR_week_2_s (7 * 24 * 3600)
#define FACTOR_s_2_ms (1000)
#define FACTOR_s_2_min (1.0f / 60.0f)
#define FACTOR_s_2_hour (1.0f / 3600.0f)
#define FACTOR_min_2_ms (1000 * 60)
#define FACTOR_hour_2_ms (1000 * 3600)
#define FACTOR_ms_2_s (1e-3f)
#define FACTOR_ms_2_us (1000)
#define FACTOR_us_2_ms (1e-3f)
#define FACTOR_ms_2_ns (1000 * 1000)
#define FACTOR_ms_2_min (1.0f / FACTOR_min_2_ms)
#define FACTOR_us_2_s (1.0 / (1e+6))

// FACTOR GEOMETRIC
#define FACTOR_mm_2_m (1e-3f)
#define FACTOR_cm_2_m (1e-2f)
#define FACTOR_mps_2_kmh (3.6f)
#define FACTOR_kmh_2_mps (1.0f/FACTOR_mps_2_kmh)
#define FACTOR_g_2_mpss (9.81f)
#define FACTOR_mpss_2_g (1.0f/FACTOR_g_2_mpss)
#define FACTOR_m_2_km (1e-3f)
#define FACTOR_deg_2_rad (0.017453292519943295769236907684886)
#define FACTOR_rad_2_deg (57.295779513082320876798154814105)

// FACTOR Electric
#define FACTOR_Ah_2_mAh (1000)
#define FACTOR_mAh_2_Ah (1e-3f)
#define FACTOR_A_2_mA (1000)
#define FACTOR_mA_2_A (1e-3f)
#define FACTOR_mAh_2_As (3.6f)
#define FACTOR_As_2_mAh (1.0f / FACTOR_mAh_2_As)
#define FACTOR_As_2_Ah (1.0f / (3.6f * 1000.0f))
#define FACTOR_mW_2_W (1e-3f)
#define FACTOR_Wh_2_J (3600.0f) // Watt Seconds to Joules
#define FACTOR_Wh_2_Ws (3600.0f)
#define FACTOR_J_2_Wh (1.0f / 3600.0f) // Joules to Watt Seconds
#define FACTOR_Ws_2_Wh (1.0f / 3600.0f)

// FACTOR Mechanical
#define FACTOR_Pa_2_bar (1e-5f)
#define FACTOR_bar_2_Pa (100000)
#define FACTOR_mbar_2_Pa (100)
#define FACTOR_Pa_2_mbar (1.0f / 100.0f)
#define FACTOR_atm_2_Pa (101325)
#define FACTOR_psi_2_Pa (6894.76f)

// File/Disk Sizes

// from/to Bytes
#define FACTOR_kB_2_B (1024)
#define FACTOR_B_2_kB (1.0f / FACTOR_kB_2_B)
#define FACTOR_MB_2_B (1024 * 1024)
#define FACTOR_B_2_MB (1.0f / FACTOR_MB_2_B)
#define FACTOR_GB_2_B (1024 * 1024 * 1024)
#define FACTOR_B_2_GB (1.0f / FACTOR_GB_2_B)
#define FACTOR_TB_2_B (1024 * 1024 * 1024 * 1024)
#define FACTOR_B_2_TB (1.0f / FACTOR_TB_2_B)

// from/to KiloBytes
#define FACTOR_MB_2_kB (1024)
#define FACTOR_kB_2_MB (1.0f / FACTOR_MB_2_kB)
#define FACTOR_GB_2_kB (1024 * 1024)
#define FACTOR_kB_2_GB (1.0f / FACTOR_GB_2_kB)
#define FACTOR_TB_2_kB (1024 * 1024 * 1024)
#define FACTOR_kB_2_TB (1.0f / FACTOR_TB_2_kB)

// from/to MegaBytes
#define FACTOR_GB_2_MB (1024)
#define FACTOR_MB_2_GB (1.0f / FACTOR_GB_2_MB)
#define FACTOR_TB_2_MB (1024 * 1024)
#define FACTOR_MB_2_TB (1.0f / FACTOR_TB_2_MB)

// from/to GigaBytes
#define FACTOR_TB_2_GB (1024)
#define FACTOR_GB_2_TB (1.0f / FACTOR_TB_2_GB)








/**************** Generall enums *******************/


enum SEND_STATUS {
	NONE, // nothing to send
	DATA_PENDING, // currently trying to send data
	FAILED, // data send failed
	DONE // data send was succesfully done
};
