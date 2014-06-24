#ifndef CCYLINDER_H
#define CCYLINDER_H

#include "GenCylinderBase.h"
#include "SplineCatmullRom.h"

#define DEFAULT_STACKS 10

namespace PS {
namespace MODEL {


//Generalized Cylinder
class GenCylinder
{
public:
    GenCylinder(){
        m_radiusStart = DEFAULT_RADIUS;
        m_radiusEnd   = DEFAULT_RADIUS;
        m_sectors = DEFAULT_SECTORS;
        m_stacks = DEFAULT_STACKS;
    };

    virtual ~GenCylinder();

    void cleanup();

    //Stacks
    void setStacks(int stacks) { m_stacks = stacks;}
    int getStacksCount() const {return m_stacks;}

    //Sectors
    void setSectors(int sectors) { m_sectors = sectors; }
    int getSectorsCount() const {return m_sectors;}

    //Start Radius
    void setRadiusStart(float r1)    {  m_radiusStart = r1;  }
    float getRadiusStart() const { return m_radiusStart;}

    //End Radius
    void setRadiusEnd(float r2)    {  m_radiusEnd = r2; }
    float getRadiusEnd() const { return m_radiusEnd;}

    /*!
     * Computes all cross sections across the profile curve.
     * Each cross section is oriented using a Frenet Frame.
     */
    void calc();

    void drawCrossSections();
    void drawWireFrame();
    void drawNormals();
    void drawSurface();

    SplineCatmullRom& getProfileCurve() {return m_profile;}
    bool closeCurve();

private:
    int m_stacks;
    int m_sectors;
    float m_radiusStart;
    float m_radiusEnd;

    //CSplineCatmullRom m_axisCurve;
    SplineCatmullRom m_profile;
    std::vector<GenCylinderBase> m_vStacks;
};

}
}

#endif
