#ifndef GENCYLINDERBASE_H
#define GENCYLINDERBASE_H

#include <vector>
#include "base/Vec.h"

#define DEFAULT_SECTORS 3
#define DEFAULT_RADIUS 0.25f;

using namespace std;
using namespace PS::MATH;

namespace PS {
namespace MODEL {


class GenCylinderBase
{

public:
    GenCylinderBase()
    {
        m_sectors = DEFAULT_SECTORS;
        m_radius = DEFAULT_RADIUS;
        m_position = vec3f(0.0f, 0.0f, 0.0f);
        m_normal = vec3f(0.0f, 0.0f, 0.0f);
        calc();
    }

    GenCylinderBase(int nSectors, float radius)
    {
        m_sectors = nSectors;
        m_radius = radius;                
        m_position = vec3f(0.0f, 0.0f, 0.0f);
        m_normal = vec3f(0.0f, 0.0f, 0.0f);
        calc();
    }

    virtual ~GenCylinderBase() {
        removeAllPoints();
    }

    void removeAllPoints()
    {
        m_lstNormals.resize(0);
        m_lstPoints.resize(0);
    }

    void calc();

    void setSectors(int sectors)
    {
        m_sectors = sectors;
    }

    void setRadius(const float radius)
    {
        m_radius = radius;
    }

    void setPosition(vec3f v)
    {
        m_position = v;
    }

    void setFrenetFrame(vec3f T, vec3f N, vec3f B)
    {
        m_tangent = T;
        m_normal = N;
        m_binormal = B;
    }

    void draw(int glArg);

    vec3f getBaseNormal() const
    {
        return m_normal;
    }

    vec3f getNormal(size_t i) const
    {
        return m_lstNormals[i];
    }

    vec3f getPoint(size_t i) const
    {
        return m_lstPoints[i];
    }

    size_t getPointsCount() const
    {
        return m_lstPoints.size();
    }

private:
    int m_sectors;
    float m_radius;
    vec3f m_position;

    vec3f m_tangent;
    vec3f m_normal;
    vec3f m_binormal;

    vector<vec3f> m_lstPoints;
    vector<vec3f> m_lstNormals;
};

}
}

#endif // CCYLINDERBASE_H
