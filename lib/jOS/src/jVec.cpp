#include "jVec.h"

#include <jSystem.h>


String jVec3f::print(const String& tag, uint8_t prec)
{
	String format_str = format("%%s: x=%%.%df, y=%%.%df, z=%%.%df", prec, prec, prec);
	return format(format_str.c_str(), tag.c_str(), this->x, this->y, this->z);
}

// Not efficient
void jVec3f::rotate(float roll, float pitch, float yaw)
{
	Mat3x3 R = Mat3x3(roll, pitch, yaw);
	jVec3f y = R * *this;
	this->x = y.x;
	this->y = y.y;
	this->z = y.z;
}