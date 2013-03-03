/*
 * Vec2f.h
 *
 */

#ifndef _VEC2F_H
#define _VEC2F_H

#ifndef __MATH_H__
#include <math.h>
#endif

#include <iostream>

class Vec2f {
public:
	Vec2f() {
		x = 0.0f;
		y = 0.0f;
	}
	Vec2f(const Vec2f &v) {
		x = v.x;
		y = v.y;
	}
	Vec2f(float nx, float ny) {
		x = nx;
		y = ny;
	}
	Vec2f(const float *v) {
		x = v[0];
		y = v[1];
	}

	int operator==(const Vec2f &v) const {
		return x == v.x && y == v.y;
	}
	int operator!=(const Vec2f &v) const {
		return x != v.x || y != v.y;
	}
	Vec2f operator+(const Vec2f &v) const {
		return Vec2f(x + v.x, y + v.y);
	}
	Vec2f operator-(const Vec2f &v) const {
		return Vec2f(x - v.x, y - v.y);
	}
	Vec2f operator-() const {
		return Vec2f(-x, -y);
	}
	Vec2f operator*(float f) const {
		return Vec2f(x * f, y * f);
	}
	Vec2f operator/(float f) const {
		return Vec2f(x / f, y / f);
	}
	Vec2f operator*(const Vec2f &v) const {
		return Vec2f(x * v.x, y * v.y);
	}
	void zero() {
		x = y = 0.0f;
	}
	void scale(float s) {
		x *= s;
		y *= s;
	}
	// In some cases we want to know the squared length only.
	// This also avoids calculating the square root.
	float squaredLength() const {
		return (float) (x * x + y * y);
	}
	float length() const {
		return (float) sqrt(squaredLength());
	}
	float dot(const Vec2f &v) const {
		return x * v.x + y * v.y;
	}
	void normalize() {
		float len = length();
		if (len != 0.0f) {
			x /= len;
			y /= len;
		}
	}
	Vec2f &operator+=(const Vec2f &v) {
		x += v.x;
		y += v.y;
		return *this;
	}
	Vec2f &operator-=(const Vec2f &v) {
		x -= v.x;
		y -= v.y;
		return *this;
	}
	Vec2f &operator*=(const float f) {
		x *= f;
		y *= f;
		return *this;
	}
	Vec2f &operator/=(const float f) {
		x /= f;
		y /= f;
		return *this;
	}
	// convenience [] operator for iterating over elements
	// this might be helpful for 2x2 matrices that consist of three Vec2fs
	float operator[](unsigned int index) const {
		switch (index) {
		case 0:
			return x;
			break;
		case 1:
			return y;
			break;
		default:
			break;
		}
		return 0;
	}

	friend std::ostream& operator<<( std::ostream& out, const Vec2f& vec)
	{
		out<<vec.x<<"/"<<vec.y;
		return out;
	}

public:
	float x, y;
};


#endif // _VEC2F_H
