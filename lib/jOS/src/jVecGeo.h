#pragma once

#include "jDefines.h"
#include "jVec.h"


/*******************************************
 *                jVec2Geo                  *
 * ****************************************/



struct jVec2Geo {
	double lat, lon; 

	jVec2Geo()
	: lat(NAN), lon(NAN)
	{}
	
	jVec2Geo(double lat, double lon)
	: lat(lat), lon(lon)
	{}

	// calculates distance between geo points WITHOUT altitude consideration
	float distance2D(const jVec2Geo& _p);

	
	String print(const String& tag, uint8_t prec = 10);

};


/*******************************************
 *                jVec3Geo                  *
 * ****************************************/



class jVec3Geo {

 public:

	double lat, lon;
	float alt;

	jVec3Geo()
	: lat(NAN), lon(NAN), alt(NAN)
	{}
	
	jVec3Geo(double lat, double lon, float alt)
	: lat(lat), lon(lon), alt(alt)
	{}

	
	bool isValid(bool require_alt=false);
	
	// calculates distance between geo points WITHOUT altitude consideration
	const float distance2D(const jVec3Geo& _p);

	// calculates distance between geo points WITH altitude consideration
	float distance3D(const jVec3Geo& _p);

	// Move geo positon by given position delta in [m]
	void move(const jVec3f& posi_delta);

	// convert this geo position to a nav position relative to the given nav_origin
	jVec3f toNav(jVec3Geo nav_origin);

	// convert a given Position in nav frame to geo frame using this as nav_origin
	jVec3Geo fromNav(jVec3f p_nav);

	
	String print(const String& tag);

	
	static jVec3Geo vec3geo_interpolate(jVec3Geo p0, jVec3Geo p1, float factor) {

		return jVec3Geo(
			p0.lat + (p1.lat - p0.lat) * factor,
			p0.lon + (p1.lon - p0.lon) * factor,
			p0.alt + (p1.alt - p0.alt) * factor
		);
	}



};