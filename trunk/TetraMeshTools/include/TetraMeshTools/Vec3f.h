/*
 * Vec3f.h
 *
 * Copyright (C) 1999 Stephen F. White
 *
 * With extensions by Dennis Luebke
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program (see the file "COPYING" for details); if
 * not, write to the Free Software Foundation, Inc., 675 Mass Ave,
 * Cambridge, MA 02139, USA.
 */

#ifndef _VEC3F_H
#define _VEC3F_H

#ifndef __MATH_H__
#include <math.h>
#endif

#include <iostream>

class Vec3f {
public:
	Vec3f() {
		x = 0.0f;
		y = 0.0f;
		z = 0.0f;
	}
	Vec3f(const Vec3f &v) {
		x = v.x;
		y = v.y;
		z = v.z;
	}
	Vec3f(float nx, float ny, float nz) {
		x = nx;
		y = ny;
		z = nz;
	}
	Vec3f(const float *v) {
		x = v[0];
		y = v[1];
		z = v[2];
	}

	int operator==(const Vec3f &v) const {
		return x == v.x && y == v.y && z == v.z;
	}
	int operator!=(const Vec3f &v) const {
		return x != v.x || y != v.y || z != v.z;
	}
	Vec3f operator+(const Vec3f &v) const {
		return Vec3f(x + v.x, y + v.y, z + v.z);
	}
	Vec3f operator-(const Vec3f &v) const {
		return Vec3f(x - v.x, y - v.y, z - v.z);
	}
	Vec3f operator-() const {
		return Vec3f(-x, -y, -z);
	}
	Vec3f operator*(float f) const {
		return Vec3f(x * f, y * f, z * f);
	}
	Vec3f operator/(float f) const {
		return Vec3f(x / f, y / f, z / f);
	}
	Vec3f operator*(const Vec3f &v) const {
		return Vec3f(x * v.x, y * v.y, z * v.z);
	}
	void zero() {
		x = y = z = 0.0f;
	}
	void scale(float s) {
		x *= s;
		y *= s;
		z *= s;
	}
	float length() const {
		return (float) sqrt(x * x + y * y + z * z);
	}
	// In some cases we want to know the squared length only.
	// This also avoids calculating the square root.
	float squaredLength() const {
		return (float) (x * x + y * y + z * z);
	}
	Vec3f cross(const Vec3f &v) const {
		return Vec3f(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x);
	}
	float dot(const Vec3f &v) const {
		return x * v.x + y * v.y + z * v.z;
	}
	void normalize() {
		float len = length();
		if (len != 0.0f) {
			x /= len;
			y /= len;
			z /= len;
		}
	}

	Vec3f& operator+=(const Vec3f &v) {
		x += v.x;
		y += v.y;
		z += v.z;
		return *this;
	}
	Vec3f& operator-=(const Vec3f &v) {
		x -= v.x;
		y -= v.y;
		z -= v.z;
		return *this;
	}
	Vec3f& operator*=(const float f) {
		x *= f;
		y *= f;
		z *= f;
		return *this;
	}
	Vec3f& operator/=(const float f) {
		x /= f;
		y /= f;
		z /= f;
		return *this;
	}
	// convenience [] operator for iterating over elements
	// this might be helpful for 3x3 matrices that consist of three Vec3fs
	const float operator[](unsigned int index) const 
	{
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
		default:
			break;
		}
		return 0.0f;
	}

	float& operator[](unsigned int index) 
	{
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
		default:
			break;
		}
	}

	friend std::ostream& operator<<( std::ostream& out, const Vec3f& vec)
	{
		out<<vec.x<<"/"<<vec.y<<"/"<<vec.z;
		return out;
	}

public:
	float x, y, z;
};


#endif // _VEC3F_H
