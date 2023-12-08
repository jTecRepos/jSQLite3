#pragma once 

#include "jDefines.h"

#include "jVec.h"


/*******************************************
 *             Help Functions              *
 * ****************************************/


template <class T>
const T& jMin(const T& a, const T& b) {
	return (b < a) ? b : a;
}

template <class T>
const T& jMax(const T& a, const T& b) {
	return (b < a) ? a : b;
}



bool isAlpha(char c);

bool isNumber(char c);

bool isHexNumber(char c);

bool isSymbol(char c);

bool isPrintable(char c);

bool isNumber(const String& s);

bool isHexNumber(const String& s);

bool isFloatNumber(const String& s);



uint32_t str2uint32(const String& s);
int32_t str2int32(const String& s);
uint32_t str2int_hex(const String& s);
float str2float(const String& s);
double str2double(const String& s);



int32_t get_random_int(int32_t min, int32_t max);



bool memcpycmp(void *buffer, const void *new_data, size_t size);
bool rangesIntersect(unsigned int offset0, unsigned int length0, unsigned int offset1, unsigned int length1);


/*******************************************
 *        DEBUG to String Functions        *
 * ****************************************/


String sprintBits(uint8_t myByte);

void jhexdump(const void *mem, uint32_t len, uint8_t cols = 16);
void jhexdump2(const void *mem, uint32_t len, uint8_t cols = 16);

String debugtoString_bytes_mostlyAscii(const char* data, uint16_t len);
String debugtoString_bytes(const char* data, uint16_t len);

// converts escape to readable character
String DebugPrint_String(const String& s);

// print an byte array like "[[x0A, x24, x47, x50, x47, x53, ...]"
String DebugPrint_ByteArray(const uint8_t* data, uint16_t len);
String DebugPrint_ByteArray(const String& data);



String Bytes_2_HexString(const uint8_t* data, uint16_t len);


/*******************************************
 *           Print  Functions              *
 * ****************************************/


// Converts Seconds to readable timestamp string
// returns like 01:05:59
String getTimeString_HHMMSS(uint32_t s);

// Converts Seconds to readable timestamp string (supports negative values)
// returns like -01:05:59.123
String getTimeString_HHMMSSCCC(float s);

// Converts Seconds to readable timestamp string
// returns like 01:05:59
String getTimeString_HHMMSS(uint8_t hour, uint8_t min, uint8_t sec);

// Converts Seconds to readable timestamp string
// returns like 01:05:59.123
String getTimeString_HHMMSSCCC(uint8_t hour, uint8_t min, uint8_t sec, uint16_t millis);

// Converts Seconds to readable timestamp string
// returns like 30.03.2020
String getDateString(uint8_t day, uint8_t month, uint16_t year);

// get Time String like 05:11 for 5min 11sec
String getTimeString_MMSS(uint32_t s);


// get Time String like 05:11 for 5h 11min
String getTimeString_HHMM(uint32_t s);


// get Time String like 05:11 for 5h 11min
String getTimeString_HHMM2(uint32_t s);


String getFileSizeString(int32_t bytes);




/*******************************************
 *             Geo Function                *
 * ****************************************/


// distance between two geo points in meter
float geo_distance(double lat1, double long1, double lat2, double long2);
float geo_course_from_to(double lat1, double long1, double lat2, double long2);
float geo_bearing(double lat1, double lon1, double lat2, double lon2);

float convert_gps_head_to_yaw(float gps_heading);
void update_geo_position_by_nav_posi_delta(double& geo_posi_lat, double& geo_posi_lon, double pos_delta_x, double pos_delta_y);
void project_latlon_2_plane(float lat, float lon, float lat0, float lon0, float& x, float& y);
void project_plane_2_latlon(float x, float y, double lat0, double lon0, double& lat, double& lon);



/*******************************************
 *              CHECKSUMs                  *
 * ****************************************/

uint32_t calc_CRC32(uint8_t* data, int16_t len);
uint16_t calc_CRC16(uint8_t* data, size_t length);


/*******************************************
 *         Signal Processing               *
 * ****************************************/


bool is_valid(float v);
bool is_valid(double x);
bool is_valid(jVec3f x);
bool is_valid(Mat3x3 x);

#define isValidNum(x) is_valid(x)

#define deg2rad(a_deg) ((a_deg) * FACTOR_deg_2_rad)
#define rad2deg(a_rad) ((a_rad) * FACTOR_rad_2_deg)

#define heading_2_yaw(head_deg) ((-head_deg) * FACTOR_deg_2_rad)
#define yaw_2_heading(yaw) ((-yaw) * FACTOR_rad_2_deg)


// calculate angle distance between to angles in rad or deg
float angle_difference(float a1, float a2, bool isDegree = false);


void closest_point_on_line(float x1, float y1, float x2, float y2, float x3, float y3, float& px, float& py);


float vec2d_dist(float x1, float y1, float x2, float y2);
double vec2d_dist(double x1, double y1, double x2, double y2);


void vec2d_interpolate(float x0, float y0, float x1, float y1, float factor, float& x_out, float& y_out);
void vec2d_interpolate(double x0, double y0, double x1, double y1, double factor, double& x_out, double& y_out);


/*******************************************
 *         lowpass and filter               *
 * ****************************************/


float get_lowpass_coeff(float dt, float f_cutoff);

// Version 1: does update param y
float lowpass_ms(float x, uint16_t dt_ms, float f_cutoff, float& y);
float lowpass(float x, float dt, float f_cutoff, float& y);

// Version 2: does not update y but returns y_new
float lowpass2(float x, float dt, float f_cutoff, float y);

void lowpass(jVec3f x, float dt, float f_cutoff, jVec3f& y);

int32_t avg_filter(int32_t x, uint16_t& N, int64_t& sum);
void avg_filter2(int32_t x, uint16_t& N, int32_t& y);
float avg_filter(float x, uint16_t& N, float& sum);

