/*
 * Vec4f.h
 *
 */

#ifndef _VEC4F_H
#define _VEC4F_H

#ifndef __MATH_H__
#include <math.h>
#endif

#include "Vec3f.h"

class Vec4f {
public:
	Vec4f() {
		x = 0.0f;
		y = 0.0f;
		z = 0.0f;
		w = 0.0f;
	}
	Vec4f(const Vec4f &v) {
		x = v.x;
		y = v.y;
		z = v.z;
		w = v.w;
	}
	// convenience constructor to allow constructing a
	// Vec4f from a Vec3f and an additional parameter
	Vec4f(const Vec3f &v, float f) {
		x = v.x;
		y = v.y;
		z = v.z;
		w = f;
	}
	Vec4f(float nx, float ny, float nz, float nw) {
		x = nx;
		y = ny;
		z = nz;
		w = nw;
	}
	Vec4f(const float *v) {
		x = v[0];
		y = v[1];
		z = v[2];
		w = v[3];
	}

	int operator==(const Vec4f &v) const {
		return x == v.x && y == v.y && z == v.z && w == v.w;
	}
	int operator!=(const Vec4f &v) const {
		return x != v.x || y != v.y || z != v.z || w != v.w;
	}
	Vec4f operator+(const Vec4f &v) const {
		return Vec4f(x + v.x, y + v.y, z + v.z, w + v.w);
	}
	Vec4f operator-(const Vec4f &v) const {
		return Vec4f(x - v.x, y - v.y, z - v.z, w - v.w);
	}
	Vec4f operator-() const {
		return Vec4f(-x, -y, -z, -w);
	}
	Vec4f operator*(float f) const {
		return Vec4f(x * f, y * f, z * f, w * f);
	}
	Vec4f operator/(float f) const {
		return Vec4f(x / f, y / f, z / f, w / f);
	}
	void zero() {
		x = y = z = w = 0.0f;
	}
	void scale(float s) {
		x *= s;
		y *= s;
		z *= s;
		w *= s;
	}

	Vec4f& operator+=(const Vec4f &v) {
		x += v.x;
		y += v.y;
		z += v.z;
		w += v.w;
		return *this;
	}
	Vec4f& operator-=(const Vec4f &v) {
		x -= v.x;
		y -= v.y;
		z -= v.z;
		w /= v.w;
		return *this;
	}
	Vec4f& operator*=(const float f) {
		x *= f;
		y *= f;
		z *= f;
		w *= f;
		return *this;
	}
	Vec4f& operator/=(const float f) {
		x /= f;
		y /= f;
		z /= f;
		w /= f;
		return *this;
	}
	// convenience [] operator for iterating over elements
	// this might be helpful for 4x4 matrices that consist of four Vec4fs
	float operator[](unsigned int index) const {
		switch (index) {
		case 0:
			return x;
			break;
		case 1:
			return y;
			break;
		case 2:
			return z;
			break;
		case 3:
			return w;
			break;
		default:
			break;
		}
		return 0;
	}

	friend std::ostream& operator<<( std::ostream& out, const Vec4f& vec)
	{
		out<<vec.x<<"/"<<vec.y<<"/"<<vec.z<<"/"<<vec.w;
		return out;
	}

public:
	float x, y, z, w;
};

#endif // _VEC4F_H
