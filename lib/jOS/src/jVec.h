#pragma once

#include "jDefines.h"

class jVec3f {

 public:
	float x, y, z;

	jVec3f()
	: x(0.0f), y(0.0f), z(0.0f)
	{}

	jVec3f(float x, float y, float z)
	{
		this->x = x;
		this->y = y;
		this->z = z;
	}
	
	jVec3f(jVec3f* _other)
	{
		this->x = _other->x;
		this->y = _other->y;
		this->z = _other->z;
	}

	jVec3f(const jVec3f& _other)
	{
		this->x = _other.x;
		this->y = _other.y;
		this->z = _other.z;
	}

	void scale(float factor)
	{
		this->x *= factor;
		this->y *= factor;
		this->z *= factor;
	}

	void scale(float factor, float offset)
	{
		this->x = this->x * factor + offset;
		this->y = this->y * factor + offset;
		this->z = this->z * factor + offset;
		
	}

	// set to (0, 0, 0)
	void zero()
	{
		this->x = 0.0f;
		this->y = 0.0f;
		this->z = 0.0f;
	}

	// Element wise absolute
	void abs()
	{
		if(this->x < 0) this->x *= -1;
		if(this->y < 0) this->y *= -1;
		if(this->z < 0) this->z *= -1;		
	}

	// Element wise maximum
	void max(const jVec3f& _v)
	{
		if(this->x < _v.x) this->x = _v.x;
		if(this->y < _v.y) this->y = _v.y;
		if(this->z < _v.z) this->z = _v.z;
	}
	
	// Element wise minimum
	void min(const jVec3f& _v)
	{
		if(this->x > _v.x) this->x = _v.x;
		if(this->y > _v.y) this->y = _v.y;
		if(this->z > _v.z) this->z = _v.z;
	}

	// Element wise 1/x
	void invert()
	{
		this->x = 1.0f / this->x;
		this->y = 1.0f / this->y;
		this->z = 1.0f / this->z;
	}

	float length()
	{
		return sqrt(
			this->x * this->x 
			+ this->y * this->y 
			+ this->z * this->z
		);
	}

	void normalize()
	{
		float len = this->length();
		this->x /= len;
		this->y /= len;
		this->z /= len;
	}

	// Rotates Vector - Not efficient
	void rotate(float roll, float pitch, float yaw);

	String print(const String& tag, uint8_t prec);


	// https://www.tutorialspoint.com/cplusplus/cpp_overloading.htm
	
	/*****************************
	 *          operator +       
	 *****************************/
	jVec3f operator+(const jVec3f& _v) {
		jVec3f res(
			this->x + _v.x,
			this->y + _v.y,
			this->z + _v.z
			);
		return res;
	}
	  
	jVec3f operator+(const float& f) {
		jVec3f res(
			this->x + f,
			this->y + f,
			this->z + f
		);
		return res;
	}
	
	jVec3f& operator+=(const float& f) {
		this->x += f;
		this->y += f;
		this->z += f;
		return *this;
	}
	
	jVec3f& operator+=(const jVec3f& _v) {
		this->x += _v.x;
		this->y += _v.y;
		this->z += _v.z;
		return *this;
	}

	/*****************************
	 *          operator -       
	 *****************************/
	
	jVec3f operator-(const jVec3f& _v) {
		jVec3f res(
			this->x - _v.x,
			this->y - _v.y,
			this->z - _v.z
			);
		return res;
	}

	  
	jVec3f operator-(const float& f) {
		jVec3f res(
			this->x - f,
			this->y - f,
			this->z - f
		);
		return res;
	}
	
	jVec3f& operator-=(const float& f) {
		this->x -= f;
		this->y -= f;
		this->z -= f;
		return *this;
	}

	jVec3f& operator-=(const jVec3f& _v) {
		this->x -= _v.x;
		this->y -= _v.y;
		this->z -= _v.z;
		return *this;
	}

	
	/*****************************
	 *          operator *      
	 *****************************/
	
	jVec3f operator*(const jVec3f& _v) {
		jVec3f res(
			this->x * _v.x,
			this->y * _v.y,
			this->z * _v.z
			);
		return res;
	}

	  
	jVec3f operator*(const float& f) {
		jVec3f res(
			this->x * f,
			this->y * f,
			this->z * f
		);
		return res;
	}
	
	jVec3f& operator*=(const float& f) {
		this->x *= f;
		this->y *= f;
		this->z *= f;
		return *this;
	}
	
	jVec3f& operator*=(const jVec3f& _v) {
		this->x *= _v.x;
		this->y *= _v.y;
		this->z *= _v.z;
		return *this;
	}

	/*****************************
	 *          operator /      
	 *****************************/
	
	jVec3f operator/(const jVec3f& _v) {
		jVec3f res(
			this->x / _v.x,
			this->y / _v.y,
			this->z / _v.z
			);
		return res;
	}

	  
	jVec3f operator/(const float& f) {
		jVec3f res(
			this->x / f,
			this->y / f,
			this->z / f
		);
		return res;
	}
	
	jVec3f& operator/=(const float& f) {
		this->x /= f;
		this->y /= f;
		this->z /= f;
		return *this;
	}
	
	jVec3f& operator/=(const jVec3f& _v) {
		this->x /= _v.x;
		this->y /= _v.y;
		this->z /= _v.z;
		return *this;
	}
};


class Mat3x3 {

 public:

	float 	xx, xy, xz,
			yx, yy, yz,
			zx, zy, zz;

	Mat3x3()
	: 	xx(1), xy(0), xz(0),
		yx(0), yy(1), yz(0),
		zx(0), zy(0), zz(1)
	{}

	Mat3x3(float roll, float pitch, float yaw)
	{
		float cosa = cos(yaw);
		float sina = sin(yaw);

		float cosb = cos(pitch);
		float sinb = sin(pitch);

		float cosc = cos(roll);
		float sinc = sin(roll);

		this->xx = cosa*cosb;
		this->xy = cosa*sinb*sinc - sina*cosc;
		this->xz = cosa*sinb*cosc + sina*sinc;

		this->yx = sina*cosb;
		this->yy = sina*sinb*sinc + cosa*cosc;
		this->yz = sina*sinb*cosc - cosa*sinc;

		this->zx = -sinb;
		this->zy = cosb*sinc;
		this->zz = cosb*cosc;
	}

	jVec3f operator*(const jVec3f& _v) {
		jVec3f res(
			this->xx * _v.x + this->xy * _v.y + this->xz * _v.z,
			this->yx * _v.x + this->yy * _v.y + this->yz * _v.z,
			this->zx * _v.x + this->zy * _v.y + this->zz * _v.z
			);
		return res;
	}
};