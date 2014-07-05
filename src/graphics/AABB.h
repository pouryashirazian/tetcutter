/*
 * PS_Box.cpp
 *
 *  Created on: Oct 29, 2012
 *      Author: pourya
 */
#include <vector>
#include "base/Vec.h"
#include "base/Ray.h"
#include "base/Matrix.h"
#include "base/Interval.h"

#ifndef PS_BOX_H
#define PS_BOX_H

using namespace std;

namespace PS{
namespace MATH{


class AABB{
public:
	AABB(){
		m_lower = m_upper = vec3(0,0,0);
	}

	AABB(const vec3& lo, const vec3& hi):m_lower(lo), m_upper(hi)
	{}

	virtual ~AABB(){}

	void set(const vec3& lo, const vec3& hi){
		m_lower = lo;
		m_upper = hi;
	}

	inline vec3 bounds(int idx) const { return (idx == 0)?m_lower:m_upper;}

    bool isValid() const { return ((m_lower.x < m_upper.x)&&(m_lower.y < m_upper.y)&&(m_lower.z < m_upper.z));}
    bool contains(const vec3& p) const;
    bool intersect(const AABB& rhs) const;
    bool intersect(const Ray& ray, const Range& t, Range& hit) const;
    bool intersect(const Ray& ray, float t0, float t1) const;

    AABB united(const AABB& rhs) const{
    	AABB result;
    	result.m_lower = vec3::minP(this->m_lower, rhs.m_lower);
    	result.m_upper = vec3::maxP(this->m_upper, rhs.m_upper);
    	return result;
    }

    void translate(const vec3& d){
    	m_lower = m_lower + d;
    	m_upper = m_upper + d;
    }

    //Transform using graphics gems method
    void transform(const mat44f& mtx);


    //dimensions
    vec3f lower() const {return this->m_lower;}
    vec3f upper() const {return this->m_upper;}

    void expand(float d);
    vec3f extent() const { return (m_upper - m_lower);}
    vec3f center() const { return (m_upper + m_lower) * 0.5f;}
    void getVertices(vector<vec3f>& vertices) const;
    void print() const;

    AABB operator=(const AABB& other) {
    	m_lower = other.m_lower;
    	m_upper = other.m_upper;
    	return (*this);
    }
private:
	vec3 m_lower;
	vec3 m_upper;
};


template <typename T>
bool Contains(const Vec3<T>& lo, const Vec3<T>& hi, const Vec3<T>& p)
{
	if(((p.x >= lo.x) && (p.x <= hi.x))&&
	  ((p.y >= lo.y) && (p.y <= hi.y))&&
 	  ((p.z >= lo.z) && (p.z <= hi.z)))
		return true;
	else
		return false;
}

}
}

#endif

