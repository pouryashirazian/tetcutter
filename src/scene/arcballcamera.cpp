#include <iostream>
#include "arcballcamera.h"

#include "base/imouselistener.h"
#include "glbackend/glselect.h"

using namespace ps::scene;


ArcBallCamera::ArcBallCamera()
{
	m_keyModifier = 0;
    m_omega = horizontalAngle;
    m_phi = verticalAngle;
    m_rho = zoom;
    m_origin = vec3f(0.0f, 0.0f, 0.0f);
    m_center = vec3f(0.0f, 0.0f, 0.0f);
    m_mouseButton = mbLeft;
    m_mouseButtonState = mbsReleased;
    m_lastPos = vec2i(0, 0);
    m_pan = vec2f(0, 0);
}

//Constructor with valid values
ArcBallCamera::ArcBallCamera(float roll, float tilt, float zoom)
{
    setRoll(roll);
    setTilt(tilt);
    setZoomLevel(zoom);
    setOrigin(vec3f(0.0f, 0.0f, 0.0f));
    setCenter(vec3f(0.0f, 0.0f, 0.0f));

    m_keyModifier = 0;
    m_mouseButton = mbLeft;
    m_lastPos = vec2i(0, 0);
}

//Set our horizontal angle can be any value (m_omega)
void ArcBallCamera::setRoll(float rollHDeg)
{
	int deg = (int)rollHDeg;
	float frac = rollHDeg - deg;
	deg = deg % 360;
	rollHDeg = (float)deg + frac;
	if(rollHDeg < 0.0f)
		rollHDeg += 360.0f;
	//printf("ROLL IS= %.3f\n", rollHDeg);

	//Modulo 360 degrees
    m_omega = DEGTORAD(rollHDeg);
}

//Set our vertical angle. This is clamped between 0 and 180
void ArcBallCamera::setTilt(float tiltVDeg)
{
    float tiltVRad = DEGTORAD(tiltVDeg);
    Clamp<float>(tiltVRad, verticalAngleMin, verticalAngleMax);
    m_phi = tiltVRad;
}

//Zoom or CCamera distance from scene is clamped.
void ArcBallCamera::setZoomLevel(float r)
{
    Clamp<float>(r, zoomMin, zoomMax);
    m_rho = r;
}

void ArcBallCamera::incrZoomLevel(float delta) {
    setZoomLevel(m_rho + delta);
}

//convert spherical coordinates to Eulerian values
vec3f ArcBallCamera::pos() const
{
    vec3f p = m_origin;

    p.x += float(m_rho * sin(m_phi) * cos(m_omega));
    p.z += float(m_rho * sin(m_phi) * sin(m_omega));
    p.y += float(m_rho * cos(m_phi));
    return p;
}

//Return Current CCamera Direction
vec3f ArcBallCamera::direction() const
{
    vec3f dir = m_origin - pos();
    dir.normalize();
    return dir;
}

//Calculate an Up vector
vec3f ArcBallCamera::upVector() const
{
    float o = (m_omega + PiOver2);
    float ph = Absolutef(m_phi - PiOver2);

    vec3f p;
    p.x = (m_rho * cos(o) * sin(ph));
    p.z = (m_rho * sin(o) * sin(ph));
    p.y = (m_rho * cos(ph));
    p.normalize();
    return p;
}

vec3f ArcBallCamera::strafe() const
{
    vec3f dir;
    dir = pos();
    dir.normalize();

    dir = vec3f::cross(dir, upVector());
    return dir;

}

void ArcBallCamera::goHome()
{
    m_omega = horizontalAngle;
    m_phi = verticalAngle;
    m_rho = zoom;
    m_origin = vec3f(0.0f, 0.0f, 0.0f);
    m_center = vec3f(0.0f, 0.0f, 0.0f);
}

void ArcBallCamera::mousePress(MouseButton button, MouseButtonState state, int x, int y)
{
    PS_UNUSED(state);

    m_mouseButton = button;
    m_mouseButtonState = state;
    m_lastPos = vec2i(x, y);
}

void ArcBallCamera::mouseMove(int x, int y)
{
    float dx = x - m_lastPos.x;
    float dy = m_lastPos.y - y;
    m_lastPos = vec2i(x, y);

    //Spherical movement on the left button
    if(m_mouseButton == mbRight && m_mouseButtonState == mbsPressed)
    {
        setRoll(this->roll() + dx);
        setTilt(this->tilt() + dy);
    }
}

void ArcBallCamera::mouseWheel(MouseWheelDir dir)
{
    //Zoom on Mouse Wheel
    if(dir == mwUp)
        setZoomLevel(m_rho + 0.5);
    else
        setZoomLevel(m_rho - 0.5);
}

void ArcBallCamera::look() const
{
    glTranslatef(m_pan.x, m_pan.y, 0.0f);
    vec3f p = this->pos();
    vec3f c = this->center();
    gluLookAt(p.x, p.y, p.z, c.x, c.y, c.z, 0.0f, 1.0f, 0.0f);
}

void ArcBallCamera::computeLocalCoordinateSystem()
{
    m_xAxis.x = - m_rho * sin(m_phi) * cos(m_omega);
    m_xAxis.y = 0;
    m_xAxis.z = - m_rho * cos(m_phi) * cos(m_omega);

    m_yAxis.x = - m_rho * cos(m_phi) * sin(m_omega);
    m_yAxis.y = m_rho * cos(m_omega);
    m_yAxis.z = m_rho * sin(m_phi) * sin(m_omega);

    m_zAxis = m_origin - m_center;
    m_xAxis.normalize();
    m_yAxis.normalize();
    m_zAxis.normalize();
}

void ArcBallCamera::screenToWorld_OrientationOnly3D(const vec3f& ptScreen, vec3f& ptWorld)
{
    computeLocalCoordinateSystem();

    ptWorld = m_xAxis * ptScreen.x;
    ptWorld = ptWorld + m_yAxis * ptScreen.y;
    ptWorld = ptWorld + m_zAxis * ptScreen.z;
}

