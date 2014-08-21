#include "AABB.h"
#include <stdio.h>


namespace PS{
namespace MATH{

void AABB::transform(const mat44f& mtx) {

	float amin[3], amax[3];
	float bmin[3], bmax[3];
	float a, b;

	//Store current AABB into a vector
	m_lower.store(amin);
	m_upper.store(amax);


	//Store translare vector in to b vector
	vec3f t = mtx.getTranslate();
	t.store(bmin);
	t.store(bmax);

	// Find extreme points by considering product of
	// min and max with each component of M.
	for (int i = 0; i < 3; i++) {
	for (int j = 0; j < 3; j++) {
			a = mtx.element(i, j) * amin[j];
			b = mtx.element(i, j) * amax[j];
			if (a < b) {
				bmin[i] += a;
				bmax[i] += b;
			}
			else
			{
				bmin[i] += b;
				bmax[i] += a;
			}
		}
	}

	m_lower = vec3f(&bmin[0]);
	m_upper = vec3f(&bmax[0]);
}

bool AABB::contains(const vec3& p) const {
	if(((p.x >= m_lower.x) && (p.x <= m_upper.x))&&
	  ((p.y >= m_lower.y) && (p.y <= m_upper.y))&&
 	  ((p.z >= m_lower.z) && (p.z <= m_upper.z)))
		return true;
	else
		return false;
}

bool AABB::intersect(const AABB& rhs) const {
    if ((m_lower.x >= rhs.m_upper.x) || (m_upper.x <= rhs.m_lower.x))
        return false;
    if ((m_lower.y >= rhs.m_upper.y) || (m_upper.y <= rhs.m_lower.y))
        return false;
    if ((m_lower.z >= rhs.m_upper.z) || (m_upper.z <= rhs.m_lower.z))
        return false;

    return true;
}

bool AABB::intersect(const Ray& ray, const Range& t, Range& hit) const {
	float tmin, tmax, tymin, tymax, tzmin, tzmax;

	tmin = (bounds(ray.sign[0]).x - ray.start.x) * ray.inv_direction.x;
	tmax = (bounds(1 - ray.sign[0]).x - ray.start.x) * ray.inv_direction.x;
	tymin = (bounds(ray.sign[1]).y - ray.start.y) * ray.inv_direction.y;
	tymax = (bounds(1 - ray.sign[1]).y - ray.start.y) * ray.inv_direction.y;
	if ((tmin > tymax) || (tymin > tmax))
		return false;
	if (tymin > tmin)
		tmin = tymin;
	if (tymax < tmax)
		tmax = tymax;
	tzmin = (bounds(ray.sign[2]).z - ray.start.z) * ray.inv_direction.z;
	tzmax = (bounds(1 - ray.sign[2]).z - ray.start.z) * ray.inv_direction.z;
	if ((tmin > tzmax) || (tzmin > tmax))
		return false;
	if (tzmin > tmin)
		tmin = tzmin;
	if (tzmax < tmax)
		tmax = tzmax;

	hit.set(tmin, tmax);
	return ((tmin < t.upper()) && (tmax > t.lower()));
}

bool AABB::intersect(const Ray& ray, float t0, float t1) const
{
	Range hit;
	return intersect(ray, Range(t0, t1), hit);
}

void AABB::getVertices(vector<vec3f>& vertices) const {
	vertices.resize(8);
	vec3f dims = m_upper - m_lower;
	for(int i=0; i<8; i++) {
		vertices[i] = m_lower + vec3f::mul(vec3f((i & 0x04) >> 2, (i & 0x02) >> 1, i & 0x01), dims);
	}
}

void AABB::print() const {

	vec3f side = m_upper - m_lower;
	printf("LO: [%.3f, %.3f, %.3f]\n", m_lower.x, m_lower.y, m_lower.z);
	printf("HI: [%.3f, %.3f, %.3f]\n", m_upper.x, m_upper.y, m_upper.z);
	printf("SIDES: [%.3f, %.3f, %.3f]\n", side.x, side.y, side.z);
}

void AABB::expand(float d) {
	vec3f dh(d * 0.5);
	m_lower = m_lower - dh;
	m_upper = m_upper + dh;
}




}
}
