#include "jHelp.h"

#include "jSystem.h"




/*******************************************
 *         Help Functions                  *
 * ****************************************/




bool isAlpha(char c)
{
	return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

bool isAlpha(String s)
{
	for (int i = 0; i < s.length(); i++)
		if (!isAlpha(s[i]))
			return false;
	return true;
}

bool isNumber(char c)
{
	return (c >= '0' && c <= '9');
}

bool isHexNumber(char c)
{
	return (c >= '0' && c <= 'F');
}

bool isSymbol(char c)
{
	return (c >= '!' && c <= '/') || (c >= ':' && c <= '@') || (c >= '[' && c < 'a') || (c >= '{' && c <= '~');
}

bool isPrintable(char c)
{
	return (c >= ' ' && c <= '~');
}


// returns true for all Numbers (also negative)
bool isNumber(const String& s)
{
	for (int i = 0; i < s.length(); i++)
		if (!(isNumber(s[i]) || (i == 0 && s[i] == '-' && s.length() > 1)))
			return false;
	return true;
}

// returns true for hex Numbers
bool isHexNumber(const String& s)
{
	for (int i = 0; i < s.length(); i++)
		if (!(isHexNumber(s[i]) || (i == 0 && s[i] == 'x' && s.length() > 1)))
			return false;
	return true;
}


bool isFloatNumber(const String& s) {
	/*
	std::string string = std::string(s.c_str());
	std::string::const_iterator it = string.begin();
	bool decimalPoint = false;
	int minSize = 0;
	if (string.size() > 0 && (string[0] == '-' || string[0] == '+')) {
		it++;
		minSize++;
	}
	
	while (it != string.end()) {
		
		if (*it == '.') {
			if (!decimalPoint) 
				decimalPoint = true;
			else 
				break;
		}
		else if (!std::isdigit(*it) && ((*it != 'f') || it + 1 != string.end() || !decimalPoint))
			break;
		++it;
	}
	return string.size() > minSize && it == string.end();
	*/

	if(s == "NaN" || s == "nan" || s == "NAN")
		return true;

	
	const char* it = s.c_str(); 
	bool decimalPoint = false;
	int minSize = 0;
	
	// check if start with '+' or '-'
	if (s.length() > 0 && (s[0] == '-' || s[0] == '+')) {
		it++;
		minSize++;
	}
	// itterate over string
	while (*it) {
		
		if (*it == '.') {
			if (!decimalPoint) 
				decimalPoint = true;
			else 
				break; // if two decimalPoint found -> not a float string
		}
		else if (!isNumber(*it) && ((*it != 'f') || *(it + 1) || !decimalPoint)) // if not a number or f at the end of the string or decimal point -> not a float string
			break;
		++it;
	}
	return s.length() > minSize && !*it; // check if end reached -> if yes is valid float
}



uint32_t str2uint32(const String& s)
{
	return strtoul(s.c_str(), NULL, 0);
}

int32_t str2int32(const String& s)
{
	return strtol(s.c_str(), NULL, 0); // https://cplusplus.com/reference/cstdlib/strtol/
}

uint32_t str2int_hex(const String& s)
{
	if(s.length() > 1 && s[0] == 'x')
		return strtoul(s.c_str() + 1, NULL, 16); // skip 'x'
	else
		return strtoul(s.c_str(), NULL, 16);
}

float str2float(const String& s)
{
	if(s == "NaN" || s == "nan" || s == "NAN")
		return NAN;

#if defined(PLATFORM_AVR) || defined(VARIANTE_NATIVE_ANDROID)
	return atof(s.c_str());
#else
	return atoff(s.c_str());
#endif 
}

double str2double(const String& s)
{
	if(s == "NaN" || s == "nan" || s == "NAN")
		return NAN;

	return atof(s.c_str());
}


int32_t get_random_int(int32_t min, int32_t max)
{
	return rand() % (max - min + 1) + min;
}


// returns true if buffer was changed by new data
bool memcpycmp(void *buffer, const void *new_data, size_t size) {
	// Check if the buffer content differs from the new data
	bool res = memcmp(buffer, new_data, size) != 0;
	
	if (res) {
		// Copy new data into buffer only if different
		memcpy(buffer, new_data, size);
	}
	return res;
}


bool rangesIntersect(unsigned int offset0, unsigned int length0, unsigned int offset1, unsigned int length1) {
	unsigned int end0 = offset0 + length0;
	unsigned int end1 = offset1 + length1;

	// Check if range 0 intersects with range 1
	bool intersects = (offset0 < end1) && (offset1 < end0);
	return intersects;
}



/*******************************************
 *        DEBUG to String Functions        *
 * ****************************************/



String sprintBits(uint8_t myByte) {
	String s = "";
	for (uint8_t mask = 0x80; mask; mask >>= 1) {
		if (mask & myByte)
			s += '1';
		else
			s += '0';

		if(mask == 0x10)
			s += " ";
	}
	return s;
}


void jhexdump(const void *mem, uint32_t len, uint8_t cols) 
{
	const uint8_t* src = (const uint8_t*) mem;
	LogD("[HEXDUMP] Address: 0x%08X len: 0x%X (%d)", (ptrdiff_t)src, len, len);
	LogD("");
	LogD("[ Address  ]   offset    -- -- -- -- -- -- -- CONTENT  -- -- -- -- -- -- ");
	//   "[0x3FCE2A9C] 0x00000000:  j  L  o  g  g  e  r  E  S  P    08 00 00 00 00 " // Example Line
	
	String out = "";
	out.reserve(75); //TODO optimize this with precaculated length of line and static char[] buffer
	for(uint32_t i = 0; i < len; i++) 
	{
		if(i % cols == 0) {
			if(i > 0)
				LogD(out);

			out = format("[0x%08X] 0x%08X: ", (ptrdiff_t)src, i);
		}

		out += format("%02X ", *src);
		src++;
	}
	LogD(out);
}


void jhexdump2(const void *mem, uint32_t len, uint8_t cols) 
{
	const uint8_t* src = (const uint8_t*) mem;
	LogD("[HEXDUMP] Address: 0x%08X len: 0x%X (%d)", (ptrdiff_t)src, len, len);
	LogD("");
	LogD("[ Address  ]   offset    -- -- -- -- -- -- -- CONTENT  -- -- -- -- -- -- ");
	//   "[0x3FCE2A9C] 0x00000000:  j  L  o  g  g  e  r  E  S  P    08 00 00 00 00 " // Example Line
	
	String out = "";
	out.reserve(75); //TODO optimize this with precaculated length of line and static char[] buffer
	for(uint32_t i = 0; i < len; i++) 
	{
		if(i % cols == 0) {
			if(i > 0)
				LogD(out);

			out = format("[0x%08X] 0x%08X: ", (ptrdiff_t)src, i);
		}

		uint8_t c = *src;
		if(c >= ' ' && c <= '~')
			out += format(" %c ", c);
		else if(c == '\n')
			out += format("%s ", "\\n");
		else if(c == '\t')
			out += format("%s ", "\\t");
		else if(c == '\r')
			out += format("%s ", "\\r");
		else
			out += format("%02X ", *src);
		src++;
	}
	LogD(out);
}


String debugtoString_bytes_mostlyAscii(const char* data, uint16_t len)
{
	String out = "";

	out += "[";
	for (int i = 0; i < len; i++)
	{
		char c = data[i];
		if (c == 0)
			out +="_";
		else if (c == '\n')
			out +="\\n";
		else if (c == '\r')
			out +="\\r";
		else if (c == '\t')
			out +="\\t";
		else if(isPrintable(c))
			out += format("%c", c);
		else
			out += format("\%02X", c);
	}
	
	out +="]";
	
	return out;
}


String debugtoString_bytes(const char* data, uint16_t len)
{
	String out = "";

	out += "[";
	for (int i = 0; i < len; i++)
	{
		char c = data[i];
		if (c == '\n')
			out +="\\n";
		else if (c == '\r')
			out +="\\r";
		else if (c == '\t')
			out +="\\t";
		else if(isPrintable(c))
			out += format("%c", c);
		else
			out += format("\%02X", c);
	}
	
	out +="]";
	
	return out;
}


String DebugPrint_String(const String& s)
{
	String res = "";
	res.reserve(s.length());

	for(int i = 0; i < s.length(); i++)
	{
		char c = s[i];
		if(c == '\n')
			res += "\\n";
		else if (c == '\t')
			res += "\\t";
		else if (c == '\r')
			res += "\\r";
		//else if (c > 128)
		//	res += format("\\x%02X", (uint8_t) c);
		
		else if(isPrintable(c))
			res += c;
		else
			res += format("\\x%02X", (uint8_t) c);

	}

	return res;
}


String DebugPrint_ByteArray(const uint8_t* data, uint16_t len)
{
	String s = "[";
	s.reserve(len * 5);

	for(int i = 0; i < len; i++)
		s += format(i + 1 < len ? "x%02X, " : "x%02X]", data[i]);

	return s;
}

String DebugPrint_ByteArray(const String& data)
{
	return DebugPrint_ByteArray((const uint8_t*) data.c_str(), data.length());
}


String Bytes_2_HexString(const uint8_t* data, uint16_t len)
{
	String res = "";
	res.reserve(len * 2);

	for(int i = 0; i < len; i++)
	{
		uint8_t v = data[i];

		char c0 = ((0xF0 & v) >> 4);
		char c1 = (0x0F & v);
		res += (char) (c0 < 10 ? c0 + '0' : c0 - 10 + 'A');
		res += (char) (c1 < 10 ? c1 + '0' : c1 - 10 + 'A');
	}

	return res;
}






/*******************************************
 *           Print  Functions              *
 * ****************************************/




String getTimeString_HHMMSS(uint32_t s)
{
	return getTimeString_HHMMSS(
		(uint8_t)((s / 3600) % 60), 
		(uint8_t)((s / 60) % 60), 
		(uint8_t)(s % 60)
	);
}


String getTimeString_HHMMSSCCC(float s)
{
	float abs_s = ABS(s);
	return ((s < 0) ? "-" : "") + getTimeString_HHMMSSCCC(
		(uint8_t)(((int) abs_s / 3600) % 60), 
		(uint8_t)((int) ((int) abs_s / 60) % 60), 
		(uint8_t)((int) abs_s % 60), 
		((uint16_t)(abs_s * 1000.f) % 1000)
	);
}



String getTimeString_HHMMSS(uint8_t hour, uint8_t min, uint8_t sec)
{
	return format("%02d:%02d:%02d", hour, min, sec);
}

String getTimeString_HHMMSSCCC(uint8_t hour, uint8_t min, uint8_t sec, uint16_t millis)
{
	return format("%02d:%02d:%02d.%03d", hour, min, sec, millis);
}


String getDateString(uint8_t day, uint8_t month, uint16_t year)
{
	return format("%02d.%02d.%04d", day, month, year);
}

String getTimeString_MMSS(uint32_t s)
{
	return format("%02d:%02d", 
		(s / FACTOR_min_2_s) % 100, 
		(s) % 60
	);
}


String getTimeString_HHMM(uint32_t s)
{
	return format("%02d:%02d", 
		(s / FACTOR_hour_2_s) % 100, 
		(s / FACTOR_min_2_s) % 60
	);
}

String getTimeString_HHMM2(uint32_t s)
{
	return format("%01dh%02dm", 
		(s / FACTOR_hour_2_s) % 100, 
		(s / FACTOR_min_2_s) % 60
	);
}




String getFileSizeString(int32_t bytes) {
	String fsize = "";
	if (bytes < 1024)				 			fsize = format2("%d B", bytes);
	else if (bytes < (1024l * 1024l))	  		fsize = format2("%.2f KB", bytes / 1024.0f);
	else if (bytes < (1024l * 1024l * 1024l)) 	fsize = format2("%.2f MB", bytes / (1024.0f * 1024.0f));
	else							  			fsize = format2("%.2f GB", bytes / (1024.0f * 1024.0f * 1024.0f));
	return fsize;
}


/*******************************************
 *             Geo Function                *
 * ****************************************/


float geo_distance(double lat1, double long1, double lat2, double long2)
{
	// returns distance in meters between two positions, both specified
	// as signed decimal-degrees latitude and longitude. Uses great-circle
	// distance computation for hypothetical sphere of radius 6372795 meters.
	// Because Earth is no exact sphere, rounding errors may be up to 0.5%.
	// Courtesy of Maarten Lamers
	double delta = deg2rad(long1 - long2);
	double sdlong = sin(delta);
	double cdlong = cos(delta);
	lat1 = deg2rad(lat1);
	lat2 = deg2rad(lat2);
	double slat1 = sin(lat1);
	double clat1 = cos(lat1);
	double slat2 = sin(lat2);
	double clat2 = cos(lat2);
	delta = (clat1 * slat2) - (slat1 * clat2 * cdlong);
	delta = sq(delta);
	delta += sq(clat2 * sdlong);
	delta = sqrt(delta);
	double denom = (slat1 * slat2) + (clat1 * clat2 * cdlong);
	delta = atan2(delta, denom);
	return delta * 6372795;
}


float geo_course_from_to(double lat1, double long1, double lat2, double long2)
{
	// returns course in degrees (North=0, West=270) from position 1 to position 2,
	// both specified as signed decimal-degrees latitude and longitude.
	// Because Earth is no exact sphere, calculated course may be off by a tiny fraction.
	// Courtesy of Maarten Lamers
	double dlon = deg2rad(long2 - long1);
	lat1 = deg2rad(lat1);
	lat2 = deg2rad(lat2);
	double a1 = sin(dlon) * cos(lat2);
	double a2 = sin(lat1) * cos(lat2) * cos(dlon);
	a2 = cos(lat1) * sin(lat2) - a2;
	a2 = atan2(a1, a2);
	if (a2 < 0.0)
	{
		a2 += TWO_PI;
	}
	return rad2deg(a2);
}


float geo_bearing(double lat1, double lon1, double lat2, double lon2)
{
	//posi_from = (lat1, lon1)
	//posi_to = (lat2, lon2)
	//returns heading/bearing in deg
	//North = 0 deg
	//East = +90 deg
	//South = 180 deg
	//West = 270 deg

	double teta1 = deg2rad(lat1);
	double teta2 = deg2rad(lat2);

	double delta2 = deg2rad(lon2-lon1);

	//==================Heading Formula Calculation================//
	
	double y = sin(delta2) * cos(teta2);
	double x = cos(teta1)*sin(teta2) - sin(teta1)*cos(teta2)*cos(delta2);
	float brng = atan2(y, x);
	brng = rad2deg(brng); //here: -180 to +180
	//brng = (int(brng + 360) % 360);
	
	//convert to [0 to 360]
	brng = brng + 360.0f;
	brng -= ((int)(brng / 360.0f)) * 360.0f; //modulo 360deg
	return brng;
}




float convert_gps_head_to_yaw(float gps_heading)
{
	gps_heading = 360 - gps_heading; // convert geo heading to coordinate correct heading (rotation from north (pos axis-x) to west (pos axis-y) is positive)
	gps_heading = gps_heading > 180 ? (gps_heading - 360) : gps_heading; // to Zero-Centered: from (0° - 360°) to (-180° - 180°)
	gps_heading *= FACTOR_deg_2_rad; // deg -> rad
	return gps_heading;
}


#define PARAM_EARTH_RADIUS 6365684 // [m]
#define FACTOR_EARTH_METER_P_DEG (FACTOR_deg_2_rad * PARAM_EARTH_RADIUS)

/*
Moves a geo location by given pos_delta in [m]
:param geo_posi_lat, geo_posi_lon: (lat, lon) [deg]
:param pos_delta_x, pos_delta_y: (dist_north, dist_east) [m]
*/
void update_geo_position_by_nav_posi_delta(double& geo_posi_lat, double& geo_posi_lon, double pos_delta_x, double pos_delta_y)
{
	// source: https://stackoverflow.com/questions/7477003/calculating-new-longitude-latitude-from-old-n-meters
	const double dlat = FACTOR_EARTH_METER_P_DEG; // [m/deg] Distance between latitude Degree lines
	double dlon = FACTOR_EARTH_METER_P_DEG * cos(geo_posi_lat  * FACTOR_deg_2_rad); // [m/deg] Distance between longitude Degree lines
	

	geo_posi_lat += (pos_delta_x / dlat);
	geo_posi_lon += (-pos_delta_y / dlon);
}


/*
Projects geo location to plane. plane is tangential to eath and touches at geo_pos0
:param geo_pos: (lat, lon)
:param geo_pos0: (lat0, lon0)
:return: (x, y) on plane +X = North, +Y = West
*/
void project_latlon_2_plane(float lat, float lon, float lat0, float lon0, float& x, float& y) 
{
	// Version 1: Calculate by projecting to Plane using tangens
	/*

	#angle delta (deg to rad)
	dy_angle = (lat - lat0) * (pi / 180)
	dx_angle = (lon - lon0) * (pi / 180)
	
	#project to plane
	y = tan(dy_angle) * PARAM_EARTH_RADIUS
	x = tan(dx_angle * cos(lat  * pi/180)) * r //Längengrad Radius richtung pole -> 0. aquator = earth radius

	*/

	// Version 2: Calculate Distance of moving 1deg in North and West direction. => results in distance on sphere/globe
	const float dlat = FACTOR_EARTH_METER_P_DEG; // [m/deg] Distance between latitude Degree lines
	float dlon = FACTOR_EARTH_METER_P_DEG * cos(lat0  * FACTOR_deg_2_rad); // [m/deg] Distance between longitude Degree lines
	

	x = (lat - lat0) * dlat;
	y = -(lon - lon0) * dlon;
}

/*
Projects position on plane (x, y) to geo location using given geo_pos0 as origin of plane
:param pos_xy: (x, y) - position to convert
:param geo_pos0: (lat0, lon0) - origin of plane in geo frame
:return: (lat, lon) - resulting geo position
*/
void project_plane_2_latlon(float x, float y, double lat0, double lon0, double& lat, double& lon) 
{

	// Version 2: Calculate Distance of moving 1deg in North and West direction. => results in distance on sphere/globe
	const float dlat = FACTOR_EARTH_METER_P_DEG; //[m/deg] Distance between latitude Degree lines
	float dlon = FACTOR_EARTH_METER_P_DEG * cos(lat0  * FACTOR_deg_2_rad); //[m/deg] Distance between longitude Degree lines

	lat = lat0 + x / dlat;
	lon = lon0 + (-y) / dlon;
}


/*******************************************
 *              CHECKSUMs                  *
 * ****************************************/


uint32_t calc_CRC32(uint8_t* data, int16_t len)
{	

	const uint32_t crc_table[16] = {
		0x00000000, 0x1db71064, 0x3b6e20c8, 0x26d930ac,
		0x76dc4190, 0x6b6b51f4, 0x4db26158, 0x5005713c,
		0xedb88320, 0xf00f9344, 0xd6d6a3e8, 0xcb61b38c,
		0x9b64c2b0, 0x86d3d2d4, 0xa00ae278, 0xbdbdf21c
	};

	uint32_t crc = ~0L;

	for( unsigned int index = 0 ; index < len  ; ++index ) {
		crc = crc_table[( crc ^ data[index] ) & 0x0f] ^ (crc >> 4);
		crc = crc_table[( crc ^ ( data[index] >> 4 )) & 0x0f] ^ (crc >> 4);
		crc = ~crc;
	}
	return crc;
}


#define CRC16_POLYNOMIAL 0x8005

uint16_t calc_CRC16(uint8_t* data, size_t length) {
    uint16_t crc = 0xFFFF;
    
    for (size_t i = 0; i < length; ++i) {
        crc ^= (uint16_t)data[i];
        
        for (uint8_t j = 0; j < 8; ++j) {
            if (crc & 0x0001) {
                crc = (crc >> 1) ^ CRC16_POLYNOMIAL;
            } else {
                crc >>= 1;
            }
        }
    }
    
    return crc;
}

/*******************************************
 *         Signal Processing               *
 * ****************************************/


bool is_valid(float x)
{
	//return !__isnanf(v);
	return !(isnan(x) 
	//|| x == infinityf()
	|| isinf(x)
	);
}

bool is_valid(double x)
{
	return !(isnan(x)
	|| isinf(x)
	);
}

bool is_valid(jVec3f x)
{
	return is_valid(x.x) && is_valid(x.y) && is_valid(x.z);
}

bool is_valid(Mat3x3 x)
{
	return (
		is_valid(x.xx) && is_valid(x.xy) && is_valid(x.xz)
		&& is_valid(x.yx) && is_valid(x.yy) && is_valid(x.yz)
		&& is_valid(x.zx) && is_valid(x.zy) && is_valid(x.zz)
	);
}


float angle_difference(float a1, float a2, bool isDegree) {
	float half = isDegree ? 180 : PI;
	float d = a1 - a2;

	if (d > half) {
		return d - (2 * half);
	}
	if (d < -half) {
		return d + (2 * half);
	}
	return d;
}


void closest_point_on_line(float x1, float y1, float x2, float y2, float x3, float y3, float& px, float& py) {

	// Direction vector of the line
	float dx = x2 - x1, dy = y2 - y1;

	// Vector from P1 to P3
	float v_x = x3 - x1, v_y = y3 - y1;

	// Projection of v onto d
	float dot_product = v_x * dx + v_y * dy;
	float len_squared = dx * dx + dy * dy;

	// Avoid division by zero
	if (len_squared == 0) {
		px = x1;
		py = y1;
	}
	else {

		float ratio = dot_product / len_squared;

		// Closest point coordinates
		px = x1 + ratio * dx;
		py = y1 + ratio * dy;

	}
}


float vec2d_dist(float x1, float y1, float x2, float y2) {
	return (float) sqrt(pow(x1 - x2, 2) + pow(y1 - y2, 2));
}


double vec2d_dist(double x1, double y1, double x2, double y2) {
	return (double) sqrt(pow(x1 - x2, 2) + pow(y1 - y2, 2));
}


void vec2d_interpolate(float x0, float y0, float x1, float y1, float factor, float& x_out, float& y_out) {
	x_out = x0 + (x1 - x0) * factor;
	y_out = y0 + (y1 - y0) * factor;
}

void vec2d_interpolate(double x0, double y0, double x1, double y1, double factor, double& x_out, double& y_out) {
	x_out = x0 + (x1 - x0) * factor;
	y_out = y0 + (y1 - y0) * factor;
}


/*******************************************
 *         lowpass and filter               *
 * ****************************************/


/*
calculates lowpass coefficient
:param dt: time delta between samples [s]
:param f_cutoff: cutoff frequency
:return: alpha for func: y_new = a * x + (1 - a) * y_old
*/
float get_lowpass_coeff(float dt, float f_cutoff)
{
	float RC = 1.0f / (TWO_PI * f_cutoff); // RC = Time constant of PT1 Glied
	return dt / (RC + dt);
}


float lowpass_ms(float x, uint16_t dt_ms, float f_cutoff, float& y)
{
	return lowpass(x, dt_ms * FACTOR_ms_2_s, f_cutoff, y);
}

// Return RC low-pass filter output samples, given input samples,
// time interval dt in [s], and time constant RC
// returns smoothed value
float lowpass(float x, float dt, float f_cutoff, float& y)
{
	// if f_cutoff = 0
	// or y is not valid
	// ->  override y with x
	if (f_cutoff == 0 || !is_valid(y))
	{
		y = x;
	}
	else if(!is_valid(x)) // if new value is invalid -> invalidate y
	{
		y = NAN;
	}
	else
	{
		float RC = 1.0f / (TWO_PI * f_cutoff);
		float a = dt / (RC + dt);
		y = a * x + (1.0f - a) * y;
	}

	/*
	if (isnan(y))
	{
		//SerialOut.println("!ISNAN!<-------------------");
		y = x;
	}
	*/

	return y;
}

// Return RC low-pass filter output samples, given input samples,
// time interval dt [s], and time constant RC
// returns smoothed value
float lowpass2(float x, float dt, float f_cutoff, float y)
{
	// if f_cutoff = 0
	// or y is not valid
	// ->  override y with x
	if (f_cutoff == 0 || isnan(y))
	{
		y = x;
	}
	else if(isnan(x)) // if new value is invalid -> invalidate y
	{
		y = NAN;
	}
	else
	{
		float RC = 1.0f / (TWO_PI * f_cutoff);
		float a = dt / (RC + dt);
		y = a * x + (1.0f - a) * y;
	}

	/*
	if (isnan(y))
	{
		//SerialOut.println("!ISNAN!<-------------------");
		y = x;
	}
	*/

	return y;
}

void lowpass(jVec3f x, float dt, float f_cutoff, jVec3f& y)
{
	lowpass(x.x, dt, f_cutoff, y.x);
	lowpass(x.y, dt, f_cutoff, y.y);
	lowpass(x.z, dt, f_cutoff, y.z);
}

int32_t avg_filter(int32_t x, uint16_t& N, int64_t& sum)
{
	if (N == 0)
		sum = 0;

	sum += x;
	N++;

	return sum / N;
}

void avg_filter2(int32_t x, uint16_t& N, int32_t& y)
{
	if (N == 0)
		y = x;
	else
		y = (int32_t)(((float)y) + ((float)x / (N))) / ((float)(N + 1) / (float)(N));
	N++;
}


float avg_filter(float x, uint16_t& N, float& sum)
{
	if (N == 0)
		sum = 0;

	sum += x;
	N++;
	
	return sum / N;
}

