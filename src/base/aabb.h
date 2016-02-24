#ifndef AABB_H
#define AABB_H

#include <vector>
#include "base/vec.h"
#include "base/ray.h"
#include "base/matrix.h"
#include "base/interval.h"


using namespace std;

namespace ps {
namespace base {

/*!
 * \brief The AABB class represent an axis-aligned bounding box for trivial occlusion tests.
 * The internal structure only holds the lower and upper corners of the box. CSG operators are
 * provided to handle hierarchical bounding volumes.
 */
class AABB {
public:
    AABB() {
        m_lower = m_upper = vec3(0,0,0);
    }

    /*!
     * \brief AABB constructor
     * \param lower the lower corner of the box
     * \param upper the upper corner of the box
     */
    AABB(const vec3& lower, const vec3& upper):m_lower(lower), m_upper(upper)
    {}

    virtual ~AABB() {}

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

    AABB united(const AABB& rhs) const {
        AABB result;
        result.m_lower = vec3::minP(this->m_lower, rhs.m_lower);
        result.m_upper = vec3::maxP(this->m_upper, rhs.m_upper);
        return result;
    }

    void translate(const vec3& d){
        m_lower = m_lower + d;
        m_upper = m_upper + d;
    }

    /*!
     * \brief transform the box using the supplied 4x4 transformation matrix
     * \param mtx
     */
    void transform(const mat44f& mtx);



    vec3f lower() const {return this->m_lower;}
    vec3f upper() const {return this->m_upper;}

    /*!
     * \brief expand expands the box d units along all major axes.
     * \param d
     */
    void expand(float d);

    /*!
     * \brief extent
     * \return the extent of the box
     */
    vec3f extent() const { return (m_upper - m_lower);}

    vec3f center() const { return (m_upper + m_lower) * 0.5f;}


    /*!
     * \brief getVertices returns a vector containing all the 8 vertices in the AABB.
     * Each vertex is indexed according to the XYZ bits representation of an interger index where the msb denotes X and LSB Z.
     * \param vertices output vector
     */
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

