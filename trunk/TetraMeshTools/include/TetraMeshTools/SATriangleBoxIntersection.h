/**
 *	SATriangleBoxIntersection.h
 *
 *	Methods for testing triangle-AABB intersection using the "Separating Axis Theorem" (SAT).
 *	C/C++ translation of:
 *	http://fileadmin.cs.lth.se/cs/Personal/Tomas_Akenine-Moller/code/tribox3.txt
 *
 *	Author: Dennis Luebke, 2013
 *
 */

#pragma once

#include "GeometryTypes.h"

void findMinMax(float x0_, float x1_, float x2_, float& min_, float& max_);

int planeBoxOverlap(const Vec3f& normal, const Vec3f& vert, const Vec3f& maxbox);

int axisTest_x01(const float a, const float b, const float fa, const float fb);

int axisTest_x2(const float a, const float b, const float fa, const float fb);

int axisTest_y02(const float a, const float b, const float fa, const float fb);

int axisTest_y1(const float a, const float b, const float fa, const float fb);

int axisTest_z12(const float a, const float b, const float fa, const float fb);

int axisTest_z0(const float a, const float b, const float fa, const float fb);

/**
 *	use separating axis theorem to test overlap between triangle and box
 *  need to test for overlap in these directions: 
 *    1) the {x,y,z}-directions (actually, since we use the AABB of the triangle 
 *       we do not even need to test these) 
 *    2) normal of the triangle
 *    3) crossproduct(edge from tri, {x,y,z}-direction)
 *       this gives 3x3=9 more tests
 */
int triBoxOverlap(const Vec3f& boxcenter, const Vec3f& boxhalfsize, const Vec3f triverts[3]);
