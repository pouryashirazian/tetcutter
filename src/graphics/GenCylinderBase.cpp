/*
 * GenCylinderBase.cpp
 *
 *  Created on: Jun 23, 2014
 *      Author: pourya
 */

#include "GenCylinderBase.h"
#include "selectgl.h"

namespace PS {
namespace MODEL {

void GenCylinderBase::calc()
{
     float angle = (float)(TwoPi /(float)m_sectors);

     m_lstPoints.resize(0);
     m_lstNormals.resize(0);

     vec3f C, pt, n;
     for(int i=0; i < m_sectors; i++)
     {
         //1.Point on 2D cross section
         C.x = m_radius * cos(i * angle);
         C.y = m_radius * sin(i * angle);
         C.z = 1.0f;

         //2.Point on cylinder
         pt.x = m_position.x + C.x * m_normal.x + C.y * m_binormal.x;
         pt.y = m_position.y + C.x * m_normal.y + C.y * m_binormal.y;
         pt.z = m_position.z + C.x * m_normal.z + C.y * m_binormal.z;

         //Compute Normal
         n = pt - m_position;
         n.normalize();

         m_lstPoints.push_back(pt);
         m_lstNormals.push_back(n);
     }
}

void GenCylinderBase::draw(int glArg)
{
    glPushMatrix();
    glBegin(glArg);
    for(size_t i=0; i< m_lstPoints.size(); i++)
        glVertex3f(m_lstPoints[i].x, m_lstPoints[i].y, m_lstPoints[i].z);
    glEnd();
    glPopMatrix();
}


}
}
