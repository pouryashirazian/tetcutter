#include "GenCylinder.h"
#include "selectgl.h"

namespace PS {
namespace MODEL {

GenCylinder::~GenCylinder() {
	cleanup();
}

void GenCylinder::cleanup() {
	for (size_t i = 0; i < m_vStacks.size(); i++)
		m_vStacks[i].removeAllPoints();
	m_vStacks.clear();
	m_profile.cleanup();
}

void GenCylinder::calc()
{
    if(m_profile.countCtrlPts() < 4)
        return;

    m_profile.build();
    if(m_vStacks.size() > 0)
    {
        for(size_t i=0; i< m_vStacks.size(); i++)
            m_vStacks[i].removeAllPoints();
        m_vStacks.clear();
    }

    vec3f xAxis(1.0f, 0.0f, 0.0f);
    vec3f yAxis(0.0f, 1.0f, 0.0f);
    vec3f zAxis(0.0f, 0.0f, 1.0f);

    float t = 0.0f;
    //Current Position + Frenet Frame
    vec3f T, N, B, P;

    //Previous Frenet Frame
    vec3f prevN, prevB;

    vec3f A;

    float deltaRadius = (m_radiusEnd - m_radiusStart) / static_cast<float>(m_stacks);
    float radius = m_radiusStart;

    for (int i=0; i < m_stacks; i++)
    {
        radius = m_radiusStart + i * deltaRadius;

        t = static_cast<float>(i)/ static_cast<float>(m_stacks - 1);

        P = m_profile.position(t);

        T = m_profile.tangent(t);
        T.normalize();

        if(i == 0)
        {
            A = m_profile.acceleration(t);

            if(A.length() < EPSILON)
                A = vec3f::cross(T, yAxis);
            A.normalize();

            //N = normalized(VxQxV)
            N = vec3f::cross(T, vec3f::cross(A, T) ); //v = u*w
            N.normalize();

            //B = TxN
            B = vec3f::cross(T, N);
            B.normalize();
        }
        else
        {
            N = vec3f::cross(prevB, T);
            B = vec3f::cross(T, N);
        }

        //Save for Next Iteration
        prevN = N;
        prevB = B;

        //At this point we have the frenet frame
        GenCylinderBase base;
        base.setRadius(radius);
        base.setSectors(m_sectors);
        base.setPosition(P);
        base.setFrenetFrame(T, N, B);
        base.calc();


        m_vStacks.push_back(base);
    }
}

bool GenCylinder::closeCurve()
{
    int ctCtrlPoints = (int)m_profile.countCtrlPts();
    if(ctCtrlPoints < MIN_CTRL_POINTS_COUNT)
        return false;

    vec3f v0 = m_profile.position(0.0f);
    vec3f t0 = m_profile.tangent(0.0f);
    vec3f last = m_profile.position(1.0f);
    float dist = vec3f::distance(v0, last);
    if(dist < EPSILON)
        return false;

    t0.normalize();
    float halfDist = dist * 0.5f;
    m_profile.addCtrlPoint(v0 - t0 * halfDist);
    m_profile.addCtrlPoint(v0);
    m_profile.addCtrlPoint(v0);

    this->calc();

    return true;
}


void GenCylinder::drawCrossSections()
{
    size_t n = m_vStacks.size();
    if(n == 0) return;
    for (size_t i=0; i< n; i++)
    {
        //m_lstStacks[i].calc();
        m_vStacks[i].draw(GL_LINE_LOOP);
    }
}

void GenCylinder::drawWireFrame()
{
    if(m_vStacks.size() < 2) return;

    size_t nPoints = m_vStacks[0].getPointsCount();
    size_t nStacks = m_vStacks.size();

    vec3f v1, v2;

    for (size_t i=0; i < nStacks - 1; i++)
    {
        //m_lstStacks[i].calc();
        //m_lstStacks[i + 1].calc();

        glBegin(GL_LINE_STRIP);
        for(size_t j=0; j < nPoints; j++)
        {
            v1 = m_vStacks[i].getPoint(j);
            v2 = m_vStacks[i + 1].getPoint(j);
            glVertex3f(v1.x, v1.y, v1.z);
            glVertex3f(v2.x, v2.y, v2.z);
        }
        v1 = m_vStacks[i].getPoint(0);
        v2 = m_vStacks[i + 1].getPoint(0);
        glVertex3f(v1.x, v1.y, v1.z);
        glVertex3f(v2.x, v2.y, v2.z);

        glEnd();
    }
}

void GenCylinder::drawNormals()
{
    if(m_vStacks.size() < 2) return;

    size_t nPoints = m_vStacks[0].getPointsCount();
    size_t nStacks = m_vStacks.size();

    vec3f v1, v2;
    vec3f n1, n2;
    vec3f t;

    for (size_t i=0; i < nStacks - 1; i++)
    {
        //m_lstStacks[i].calc();
        //m_lstStacks[i + 1].calc();

        glBegin(GL_LINES);
        for(size_t j=0; j < nPoints; j++)
        {
            v1 = m_vStacks[i].getPoint(j);
            n1 = m_vStacks[i].getNormal(j);

            v2 = m_vStacks[i + 1].getPoint(j);
            n2 = m_vStacks[i + 1].getNormal(j);
            //glNormal3fv(n1.ptr());
            glVertex3fv(v1.ptr());
            t = v1 + n1 * 0.2f;
            glVertex3fv(t.ptr());

            //glNormal3fv(n2.ptr());
            glVertex3fv(v2.ptr());
            t = v2 + n2 * 0.2f;
            glVertex3fv(t.ptr());

        }
        v1 = m_vStacks[i].getPoint(0);
        n1 = m_vStacks[i].getNormal(0);
        v2 = m_vStacks[i + 1].getPoint(0);
        n2 = m_vStacks[i + 1].getNormal(0);

        //glNormal3fv(n1.ptr());
        glVertex3fv(v1.ptr());
        t = v1 + n1 * 0.2f;
        glVertex3fv(t.ptr());


        //glNormal3fv(n2.ptr());
        glVertex3fv(v2.ptr());
        t = v2 + n2 * 0.2f;
        glVertex3fv(t.ptr());


        glEnd();
    }
}

void GenCylinder::drawSurface()
{
    if(m_vStacks.size() < 2) return;

    size_t nPoints = m_vStacks[0].getPointsCount();
    size_t nStacks = m_vStacks.size();

    vec3f v1, v2;
    vec3f n1, n2;

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    for (size_t i=0; i < nStacks - 1; i++)
    {
        //m_lstStacks[i].calc();
        //m_lstStacks[i + 1].calc();

        glBegin(GL_TRIANGLE_STRIP);
        for(size_t j=0; j < nPoints; j++)
        {
            v1 = m_vStacks[i].getPoint(j);
            n1 = m_vStacks[i].getNormal(j);

            v2 = m_vStacks[i + 1].getPoint(j);
            n2 = m_vStacks[i + 1].getNormal(j);
            glNormal3fv(n1.ptr());
            glVertex3fv(v1.ptr());

            glNormal3fv(n2.ptr());
            glVertex3fv(v2.ptr());
        }
        v1 = m_vStacks[i].getPoint(0);
        n1 = m_vStacks[i].getNormal(0);
        v2 = m_vStacks[i + 1].getPoint(0);
        n2 = m_vStacks[i + 1].getNormal(0);

        glNormal3fv(n1.ptr());
        glVertex3fv(v1.ptr());

        glNormal3fv(n2.ptr());
        glVertex3fv(v2.ptr());

        glEnd();
    }
}


}
}



