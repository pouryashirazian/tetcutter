/*
 * Intersections.h
 *
 *  Created on: Sep 8, 2013
 *      Author: pourya
 */

#ifndef INTERSECTIONS_H_
#define INTERSECTIONS_H_

#include "base/Vec.h"

using namespace PS;
using namespace PS::MATH;

namespace PS {
namespace INTERSECTIONS {

#define EPSILON 1E-5
#define X 0
#define Y 1
#define Z 2

#define CROSS(dest,v1,v2) \
		dest[0]=v1[1]*v2[2]-v1[2]*v2[1]; \
		dest[1]=v1[2]*v2[0]-v1[0]*v2[2]; \
		dest[2]=v1[0]*v2[1]-v1[1]*v2[0];

#define DOT(v1,v2) (v1[0]*v2[0]+v1[1]*v2[1]+v1[2]*v2[2])

#define SUB(dest,v1,v2) \
		dest[0]=v1[0]-v2[0]; \
		dest[1]=v1[1]-v2[1]; \
		dest[2]=v1[2]-v2[2];

#define FINDMINMAX(x0,x1,x2,min,max) \
		min = max = x0;   \
		if(x1<min) min=x1;\
		if(x1>max) max=x1;\
		if(x2<min) min=x2;\
		if(x2>max) max=x2;

/*======================== X-tests ========================*/
#define AXISTEST_X01(a, b, fa, fb)			   \
		p0 = a*v0[Y] - b*v0[Z];			       	   \
		p2 = a*v2[Y] - b*v2[Z];			       	   \
		if(p0<p2) {min=p0; max=p2;} else {min=p2; max=p0;} \
		rad = fa * boxhalfsize[Y] + fb * boxhalfsize[Z];   \
		if(min>rad || max<-rad) return 0;


#define AXISTEST_X2(a, b, fa, fb)			   \
		p0 = a*v0[Y] - b*v0[Z];			           \
		p1 = a*v1[Y] - b*v1[Z];			       	   \
		if(p0<p1) {min=p0; max=p1;} else {min=p1; max=p0;} \
		rad = fa * boxhalfsize[Y] + fb * boxhalfsize[Z];   \
		if(min>rad || max<-rad) return 0;
/*======================== Y-tests ========================*/

#define AXISTEST_Y02(a, b, fa, fb)			   \
		p0 = -a*v0[X] + b*v0[Z];		      	   \
		p2 = -a*v2[X] + b*v2[Z];	       	       	   \
		if(p0<p2) {min=p0; max=p2;} else {min=p2; max=p0;} \
		rad = fa * boxhalfsize[X] + fb * boxhalfsize[Z];   \
		if(min>rad || max<-rad) return 0;



#define AXISTEST_Y1(a, b, fa, fb)			   \
		p0 = -a*v0[X] + b*v0[Z];		      	   \
		p1 = -a*v1[X] + b*v1[Z];	     	       	   \
		if(p0<p1) {min=p0; max=p1;} else {min=p1; max=p0;} \
		rad = fa * boxhalfsize[X] + fb * boxhalfsize[Z];   \
		if(min>rad || max<-rad) return 0;

/*======================== Z-tests ========================*/
#define AXISTEST_Z12(a, b, fa, fb)			   \
		p1 = a*v1[X] - b*v1[Y];			           \
		p2 = a*v2[X] - b*v2[Y];			       	   \
		if(p2<p1) {min=p2; max=p1;} else {min=p1; max=p2;} \
		rad = fa * boxhalfsize[X] + fb * boxhalfsize[Y];   \
		if(min>rad || max<-rad) return 0;

#define AXISTEST_Z0(a, b, fa, fb)			   \
		p0 = a*v0[X] - b*v0[Y];				   \
		p1 = a*v1[X] - b*v1[Y];			           \
		if(p0<p1) {min=p0; max=p1;} else {min=p1; max=p0;} \
		rad = fa * boxhalfsize[X] + fb * boxhalfsize[Y];   \
		if(min>rad || max<-rad) return 0;

template<typename T>
int PlaneBoxOverlap(T normal[3], T vert[3], T maxbox[3]) {
	int q;
	T vmin[3], vmax[3], v;
	for (q = X; q <= Z; q++) {
		v = vert[q];					// -NJMP-
		if (normal[q] > 0.0f)
		{
			vmin[q] = -maxbox[q] - v;	// -NJMP-
			vmax[q] = maxbox[q] - v;	// -NJMP-
		}
		else
		{
			vmin[q] = maxbox[q] - v;	// -NJMP-
			vmax[q] = -maxbox[q] - v;	// -NJMP-
		}
	}

	if (DOT(normal,vmin) > 0.0f)
		return 0;	// -NJMP-
	if (DOT(normal,vmax) >= 0.0f)
		return 1;	// -NJMP-
	return 0;
}


//AABB Triangle Intersection
template <typename T>
int IntersectBoxTriangle(T boxcenter[3], T boxhalfsize[3], T triverts[3][3]) {

	/*  Use separating axis theorem to test overlap between triangle and box
		need to test for overlap in these directions:
		1) the {x,y,z}-directions (actually, since we use the AABB of the triangle
		we do not even need to test these)
		2) normal of the triangle
	    3) crossproduct(edge from tri, {x,y,z}-directin)
	    this gives 3x3=9 more tests
	*/
	T v0[3], v1[3], v2[3];
	T min, max, p0, p1, p2, rad, fex, fey, fez;	// -NJMP- "d" local variable removed
	T normal[3], e0[3], e1[3], e2[3];

	/* This is the fastest branch on Sun */
	/* move everything so that the boxcenter is in (0,0,0) */

	SUB(v0, triverts[0], boxcenter);

	SUB(v1, triverts[1], boxcenter);

	SUB(v2, triverts[2], boxcenter);

	/* compute triangle edges */

	SUB(e0, v1, v0); /* tri edge 0 */

	SUB(e1, v2, v1); /* tri edge 1 */

	SUB(e2, v0, v2); /* tri edge 2 */

	/* Bullet 3:  */

	/*  test the 9 tests first (this was faster) */

	fex = fabsf(e0[X]);

	fey = fabsf(e0[Y]);

	fez = fabsf(e0[Z]);

	AXISTEST_X01(e0[Z], e0[Y], fez, fey);

	AXISTEST_Y02(e0[Z], e0[X], fez, fex);

	AXISTEST_Z12(e0[Y], e0[X], fey, fex);

	fex = fabsf(e1[X]);

	fey = fabsf(e1[Y]);

	fez = fabsf(e1[Z]);

	AXISTEST_X01(e1[Z], e1[Y], fez, fey);

	AXISTEST_Y02(e1[Z], e1[X], fez, fex);

	AXISTEST_Z0(e1[Y], e1[X], fey, fex);

	fex = fabsf(e2[X]);

	fey = fabsf(e2[Y]);

	fez = fabsf(e2[Z]);

	AXISTEST_X2(e2[Z], e2[Y], fez, fey);

	AXISTEST_Y1(e2[Z], e2[X], fez, fex);

	AXISTEST_Z12(e2[Y], e2[X], fey, fex);

	/* Bullet 1: */

	/*  first test overlap in the {x,y,z}-directions */

	/*  find min, max of the triangle each direction, and test for overlap in */

	/*  that direction -- this is equivalent to testing a minimal AABB around */

	/*  the triangle against the AABB */

	/* test in X-direction */

	FINDMINMAX(v0[X], v1[X], v2[X], min, max);

	if (min > boxhalfsize[X] || max < -boxhalfsize[X])
		return 0;

	/* test in Y-direction */

	FINDMINMAX(v0[Y], v1[Y], v2[Y], min, max);

	if (min > boxhalfsize[Y] || max < -boxhalfsize[Y])
		return 0;

	/* test in Z-direction */

	FINDMINMAX(v0[Z], v1[Z], v2[Z], min, max);

	if (min > boxhalfsize[Z] || max < -boxhalfsize[Z])
		return 0;

	/* Bullet 2: */

	/*  test if the box intersects the plane of the triangle */

	/*  compute plane equation of triangle: normal*x+d=0 */

	CROSS(normal, e0, e1);

	// -NJMP- (line removed here)

	if (!PlaneBoxOverlap<T>(normal, v0, boxhalfsize))
		return 0;	// -NJMP-

	return 1; /* box and triangle overlaps */
}


/*!
 * Segment triangle intersection
 */
int IntersectSegmentTriangle(const vec3d& s0, const vec3d& s1, const vec3d p[3], double& t, vec3d& uvw, vec3d& xyz);
int IntersectSegmentTriangleF(const vec3f& s0, const vec3f& s1, const vec3f p[3], float& t, vec3f& uvw, vec3f& xyz);

/*!
 * Ray triangle intersection
 */
int IntersectRayTriangle(const vec3d& ro, const vec3d& rd, const vec3d p[3], vec3d& uvt);

/*!
 * Compute the intersection between a ray and a triangle
 */
template <typename T>
int IntersectRayTriangle(T rayorigin[3], T raydir[3], T triverts[3][3], T& outU, T& outV, T& outT) {
	//Set output
	outU = 0.0;
	outV = 0.0;
	outT = 0.0;

	Vec3<T> p0 = Vec3<T>(&triverts[0]);
	Vec3<T> p1 = Vec3<T>(&triverts[1]);
	Vec3<T> p2 = Vec3<T>(&triverts[2]);
	Vec3<T> o = Vec3<T>(&rayorigin[0]);
	Vec3<T> d = Vec3<T>(&raydir[0]);

	Vec3<T> e1 = p1 - p0;
	Vec3<T> e2 = p2 - p0;
	Vec3<T> q = Vec3<T>::cross(d, e2);

	//Test Determinant
	T a = Vec3<T>::dot(e1, q);
	if(fabs(a) < EPSILON)
		return 0;

	//Test U
	T f = 1.0 / a;
	Vec3<T> s = o - p0;
	outU = f * Vec3<T>::dot(s, q);
	if(outU < 0.0)
		return 0;

	//Test V
	Vec3<T> r = Vec3<T>::cross(s, e1);
	outV = f * Vec3<T>::dot(d, r);
	if((outV < 0.0) || (outU + outV > 1.0))
		return 0;

	//Test T
	outT = f * Vec3<T>::dot(e2, r);

	return 1;
}

}
}

#endif /* INTERSECTIONS_H_ */
