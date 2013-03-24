/**
 *	SATriangleBoxIntersection.cpp
 *
 *	Author: Dennis Luebke, 2013
 *
 */

#include "SATriangleBoxIntersection.h"

Vec3f v0, v1, v2, boxhalfsize;
float min, max, p0, p1, p2, rad, fex, fey, fez;

void findMinMax(float x0, float x1, float x2, float& min_, float& max_)
{
	min_ = max_ = x0;
	if(x1 < min_) min_ = x1;
	if(x1 > max_) max_ = x1;
	if(x2 < min_) min_ = x2;
	if(x2 > max_) max_ = x2;
}

int planeBoxOverlap(const Vec3f& normal, const Vec3f& vert, const Vec3f& maxbox)
{
	unsigned int q;
	Vec3f vmin, vmax;
	float v;
	for(q=0; q<3; ++q)
	{
		v = vert[q];
		if(normal[q] > 0.0f)
		{
			vmin[q] = -maxbox[q] - v;
			vmax[q] = maxbox[q] - v;
		}
		else
		{
			vmin[q]= maxbox[q] - v;
			vmax[q]=-maxbox[q] - v;
		}
	}
	if (normal.dot(vmin) > 0.0f) return 0;
	if (normal.dot(vmax) >= 0.0f) return 1;
	return 0;
}

int axisTest_x01(const float a, const float b, const float fa, const float fb)
{
	float p0 = a * v0[1] - b * v0[2];
	float p2 = a * v2[1] - b * v2[2];
	if(p0 < p2) 
	{
		min = p0; 
		max = p2;
	} 
	else 
	{
		min = p2; 
		max = p0;
	}
	float rad = fa * boxhalfsize[1] + fb * boxhalfsize[2];
	if(min > rad || max < -rad) 
		return 0;
	else
		return -1;
}

int axisTest_x2(const float a, const float b, const float fa, const float fb)
{
	float p0 = a * v0[1] - b * v0[2];
	float p1 = a * v1[1] - b * v1[2];
	if(p0 < p1) 
	{
		min = p0; 
		max = p1;
	} 
	else 
	{
		min = p1; 
		max=p0;
	}
	float rad = fa * boxhalfsize[1] + fb * boxhalfsize[2];
	if(min > rad || max < -rad) 
		return 0;
	else
		return -1;
}

int axisTest_y02(const float a, const float b, const float fa, const float fb)
{
	float p0 = -a * v0[0] + b * v0[2];
	float p2 = -a * v2[0] + b * v2[2];
	if(p0 < p2) 
	{
		min = p0; 
		max = p2;
	} 
	else 
	{
		min = p2; 
		max = p0;
	}
	float rad = fa * boxhalfsize[0] + fb * boxhalfsize[2];
	if(min > rad || max < -rad) 
		return 0;
	else
		return -1;
}

int axisTest_y1(const float a, const float b, const float fa, const float fb)
{
	float p0 = -a * v0[0] + b * v0[2];
	float p1 = -a * v1[0] + b * v1[2];
	if(p0 < p1) 
	{
		min = p0; 
		max = p1;
	} 
	else 
	{
		min = p1; 
		max = p0;
	}
	float rad = fa * boxhalfsize[0] + fb * boxhalfsize[2];
	if(min > rad || max < -rad) 
		return 0;
	else
		return -1;
}

int axisTest_z12(const float a, const float b, const float fa, const float fb)
{
	float p1 = a * v1[0] - b * v1[1];
	float p2 = a * v2[0] - b * v2[1];
	if(p2 < p1) 
	{
		min = p2; 
		max = p1;
	} 
	else 
	{
		min = p1; 
		max = p2;
	}
	float rad = fa * boxhalfsize[0] + fb * boxhalfsize[1];
	if(min > rad || max < -rad) 
	{
		return 0;
	}
	else
		return -1;
}

int axisTest_z0(const float a, const float b, const float fa, const float fb)
{
	float p0 = a * v0[0] - b * v0[1];
	float p1 = a * v1[0] - b * v1[1];
	if(p0 < p1) 
	{
		min = p0; 
		max = p1;
	} 
	else 
	{
		min = p1; 
		max = p0;
	}
	float rad = fa * boxhalfsize[0] + fb * boxhalfsize[1];
	if(min > rad || max < -rad) 
		return 0;
	else
		return -1;
}

int triBoxOverlap(const Vec3f& boxcenter, const Vec3f& boxhalfsize_, const Vec3f triverts[3])
{
	boxhalfsize = boxhalfsize_;
	Vec3f normal, e0, e1 , e2;
	/* This is the fastest branch on Sun */
	/* move everything so that the boxcenter is in (0,0,0) */
	v0 = triverts[0] - boxcenter;
	v1 = triverts[1] - boxcenter;
	v2 = triverts[2] - boxcenter;

	/* compute triangle edges */
	e0 = v1 - v0;      /* tri edge 0 */
	e1 = v2 - v1;      /* tri edge 1 */
	e2 = v0 - v2;      /* tri edge 2 */

	/**
	 * Bullet 3:
	 * test the 9 tests first (this was faster)
	 */
	fex = fabsf(e0[0]);
	fey = fabsf(e0[1]);
	fez = fabsf(e0[2]);

	int result = -1;
	result = axisTest_x01(e0[2], e0[1], fez, fey);
	if (result == 0)
	{
		return 0;
	}

	result = axisTest_y02(e0[2], e0[0], fez, fex);
	if (result == 0)
	{
		return 0;
	}

	result = axisTest_z12(e0[1], e0[0], fey, fex);
	if (result == 0)
	{
		return 0;
	}

	fex = fabsf(e1[0]);
	fey = fabsf(e1[1]);
	fez = fabsf(e1[2]);

	result = axisTest_x01(e1[2], e1[1], fez, fey);
	if (result == 0)
	{
		return 0;
	}

	result = axisTest_y02(e1[2], e1[0], fez, fex);
	if (result == 0)
	{
		return 0;
	}

	result = axisTest_z0(e1[1], e1[0], fey, fex);
	if (result == 0)
	{
		return 0;
	}

	fex = fabsf(e2[0]);
	fey = fabsf(e2[1]);
	fez = fabsf(e2[2]);

	result = axisTest_x2(e2[2], e2[1], fez, fey);
	if (result == 0)
	{
		return 0;
	}

	result = axisTest_y1(e2[2], e2[0], fez, fex);
	if (result == 0)
	{
		return 0;
	}

	result = axisTest_z12(e2[1], e2[0], fey, fex);
	if (result == 0)
	{
		return 0;
	}

	/**
	 * Bullet 1:
	 *  first test overlap in the {x,y,z}-directions 
	 *  find min, max of the triangle each direction, and test for overlap in 
	 *  that direction -- this is equivalent to testing a minimal AABB around
	 *  the triangle against the AABB 
	 */

	// test in X-direction
	findMinMax(v0[0], v1[0], v2[0], min, max);
	if(min > boxhalfsize_[0] || max < -boxhalfsize_[0]) return 0;

	// test in Y-direction
	findMinMax(v0[1], v1[1], v2[1], min, max);
	if(min > boxhalfsize_[1] || max < -boxhalfsize_[1]) return 0;

	// test in Z-direction
	findMinMax(v0[2], v1[2], v2[2], min, max);
	if(min > boxhalfsize_[2] || max < -boxhalfsize_[2]) return 0;

	/**
	 * Bullet 2:
	 *  test if the box intersects the plane of the triangle
	 *  compute plane equation of triangle: normal*x+d=0
	 */
	normal = e0.cross(e1);

	if(!planeBoxOverlap(normal, v0, boxhalfsize_)) return 0;

	return 1;   /* box and triangle overlaps */

}
