/*
 * Intersections.cpp
 *
 *  Created on: Sep 26, 2013
 *      Author: pourya
 */
#include "Intersections.h"

namespace PS {
namespace INTERSECTIONS {

int IntersectRayTriangleF(const vec3f& ro, const vec3f& rd, const vec3f p[3], vec3f& uvt) {

	vec3f e1 = p[1] - p[0];
	vec3f e2 = p[2] - p[0];
	vec3f q = vec3f::cross(rd, e2);

	//Test Determinant
	float a = vec3f::dot(e1, q);
	if(fabs(a) < EPSILON)
		return 0;

	//Test U
	float f = 1.0 / a;
	vec3f s = ro - p[0];
	uvt.x = f * vec3f::dot(s, q);
	if(uvt.x < 0.0)
		return 0;

	//Test V
	vec3f r = vec3f::cross(s, e1);
	uvt.y = f * vec3f::dot(rd, r);
	if((uvt.y < 0.0) || ((uvt.x + uvt.y) > 1.0))
		return 0;

	//Test T
	uvt.z = f * vec3f::dot(e2, r);

	return 1;
}

int IntersectSegmentTriangleF(const vec3f& s0, const vec3f& s1, const vec3f p[3], vec3f& uvw, vec3f& xyz) {

	vec3f delta = s1 - s0;
	vec3f rd = delta.normalized();
	double deltaLen = delta.length();
	vec3f uvt;
	int res = IntersectRayTriangleF(s0, rd, p, uvt);
	if(res > 0) {
		if(uvt.z >= 0.0 && uvt.z <= deltaLen) {
			//Barycentric coordinate
			uvw = vec3f(uvt.x, uvt.y, 1.0 - uvt.x - uvt.y);

			//Cartesian Coordinate
			xyz = s0 + rd * uvt.z;
			return res;
		}
	}
	return 0;
}




int IntersectSegmentTriangle(const vec3d& s0, const vec3d& s1, const vec3d p[3], vec3d& uvw, vec3d& xyz) {

	vec3d delta = s1 - s0;
	vec3d rd = delta.normalized();
	double deltaLen = delta.length();
	vec3d uvt;
	int res = IntersectRayTriangle(s0, rd, p, uvt);
	if(res > 0) {
		if(uvt.z >= 0.0 && uvt.z <= deltaLen) {
			//Barycentric coordinate
			uvw = vec3d(uvt.x, uvt.y, 1.0 - uvt.x - uvt.y);

			//Cartesian Coordinate
			xyz = s0 + rd * uvt.z;
			return res;
		}
	}
	return 0;
}

int IntersectRayTriangle(const vec3d& ro, const vec3d& rd, const vec3d p[3], vec3d& uvt) {

	vec3d e1 = p[1] - p[0];
	vec3d e2 = p[2] - p[0];
	vec3d q = vec3d::cross(rd, e2);

	//Test Determinant
	double a = vec3d::dot(e1, q);
	if(fabs(a) < EPSILON)
		return 0;

	//Test U
	double f = 1.0 / a;
	vec3d s = ro - p[0];
	uvt.x = f * vec3d::dot(s, q);
	if(uvt.x < 0.0)
		return 0;

	//Test V
	vec3d r = vec3d::cross(s, e1);
	uvt.y = f * vec3d::dot(rd, r);
	if((uvt.y < 0.0) || ((uvt.x + uvt.y) > 1.0))
		return 0;

	//Test T
	uvt.z = f * vec3d::dot(e2, r);

	return 1;
}
}
}




