#include "jVecGeo.h"

#include "jSystem.h"
#include "jHelp.h"


/*******************************************
 *                jVec2Geo                  *
 * ****************************************/


float jVec2Geo::distance2D(const jVec2Geo& _p)
{
	return geo_distance(this->lat, this->lon, _p.lat, _p.lon);
}


String jVec2Geo::print(const String& tag, uint8_t prec)
{
	String format_str = format("%%s: lat=%%.%df, lon=%%.%df", prec, prec);
	return format(format_str.c_str(), tag.c_str(), this->lat, this->lon);
}


/*******************************************
 *                jVec3Geo                  *
 * ****************************************/


bool jVec3Geo::isValid(bool require_alt)
{
	return isValidNum(this->lat) 
		&& isValidNum(this->lon) 
		&& (!require_alt || isValidNum(this->alt));
}

const float jVec3Geo::distance2D(const jVec3Geo& _p)
{
	return geo_distance(this->lat, this->lon, _p.lat, _p.lon);
}


float jVec3Geo::distance3D(const jVec3Geo& _p)
{
	float dist_2d = geo_distance(this->lat, this->lon, _p.lat, _p.lon);
	float dist_alt = this->alt - _p.alt;
	return sqrt(dist_2d*dist_2d + dist_alt*dist_alt); 
}


void jVec3Geo::move(const jVec3f& posi_delta) {
	update_geo_position_by_nav_posi_delta(this->lat, this->lon, posi_delta.x, posi_delta.y);
	this->alt += posi_delta.z;
}

jVec3f jVec3Geo::toNav(jVec3Geo nav_origin) {
	jVec3f p_nav;
	project_latlon_2_plane(this->lat, this->lon, nav_origin.lat, nav_origin.lon, p_nav.x, p_nav.y);
	p_nav.z = this->alt - nav_origin.alt;
	return p_nav;
}

jVec3Geo jVec3Geo::fromNav(jVec3f p_nav) {
	jVec3Geo res;
	project_plane_2_latlon(p_nav.x, p_nav.y, this->lat, this->lon, res.lat, res.lon);
	res.alt = p_nav.z + this->alt;
	return res;
}


String jVec3Geo::print(const String& tag)
{
	return format("%s: lat=%.10f, lon=%.10f, alt=%.2f", tag.c_str(), this->lat, this->lon, this->alt);
}
