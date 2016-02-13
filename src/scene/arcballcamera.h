#ifndef ARCBALLCAMERA_H
#define ARCBALLCAMERA_H

#include "base/vec.h"
#include "base/imouselistener.h"

using namespace ps;
using namespace ps::base;

//A basic ArcBall Camera class to be used with openGL or directX
namespace ps {
namespace scene {

const float zoomMin = 0.01f;
const float zoomMax = 80.0f;
const float zoom = 30.0f;
const float verticalAngleMin = 0.01f;
const float verticalAngleMax = Pi - 0.01f;
const float horizontalAngle = -1.0f * PiOver2;
const float verticalAngle = PiOver2;

class ArcBallCamera : public IMouseListener
{
private:
    //Omega and Phi are horizontal and vertical angles of spherical coordinates respectively
    //rho is the CCamera distance from scene (Zoom)
    float m_omega, m_phi, m_rho;
    vec3f m_origin;

    //Center point in scene to lookAt
    vec3f  m_center;
    vec2i m_lastPos;
    vec2f m_pan;

    vec3f m_xAxis;
    vec3f m_yAxis;
    vec3f m_zAxis;

    int m_keyModifier;
    MouseButton m_mouseButton;
    MouseButtonState m_mouseButtonState;
public:
    //Default Constructor
    ArcBallCamera();

    //Constructor with valid values
    ArcBallCamera(float roll, float tilt, float zoom);

    //Look Matrix
    void look() const;

    //Roll is the horizontal movement
    float roll() const {return RADTODEG(m_omega);}
    void setRoll(float rollHDeg);

    //Tilt is the vertical movement
    float tilt() const {return RADTODEG(m_phi);}
    void setTilt(float tiltVDeg);

    //Zoom
    float zoomlevel() const {return m_rho;}
    void setZoomLevel(float r);
    void incrZoomLevel(float delta);

    //Pan
    vec2f pan() const { return m_pan;}
    void setPan(const vec2f& pan) { m_pan = pan;}

    //Origin position
    vec3f origin() const {return m_origin;}
    void setOrigin(const vec3f& org) { m_origin = org;}

    //Center Position
    vec3f center() const {return m_center;}
    void setCenter(const vec3f& c) {m_center = c;}


    //Handle Mouse Events
    void mousePress(MouseButton button, MouseButtonState state, int x, int y) override;
    void mouseMove(int x, int y) override;
    void mouseWheel(MouseWheelDir dir) override;


    //convert spherical coordinates to Eulerian values
    vec3f pos() const;

    //Return Current CCamera Direction
    vec3f direction() const;

    //Calculate an Up vector
    vec3f upVector() const;

    vec3f strafe() const;

    //Last position
    vec2i lastPos() const {return m_lastPos;}
    void setLastPos(const vec2i& lastPos) { m_lastPos = lastPos;}

    //vec3f convertToWorld(const vec3f& pos);
    void computeLocalCoordinateSystem();
    void screenToWorld_OrientationOnly3D(const vec3f& ptScreen, vec3f& ptWorld);

    void goHome();
};

}
}
#endif
