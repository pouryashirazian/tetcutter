#include "ArcBallCamera.h"
#include "selectgl.h"

namespace PS
{


ArcBallCamera::ArcBallCamera()
{
	m_keyModifier = 0;
    m_omega = horizontalAngle;
    m_phi = verticalAngle;
    m_rho = zoom;
    m_origin = vec3f(0.0f, 0.0f, 0.0f);
    m_center = vec3f(0.0f, 0.0f, 0.0f);
    m_mouseButton = mbNone;
    m_lastPos = vec2i(0, 0);
    m_pan = vec2f(0, 0);
}

//Constructor with valid values
ArcBallCamera::ArcBallCamera(float roll, float tilt, float zoom)
{
    setRoll(roll);
    setTilt(tilt);
    setZoom(zoom);
    setOrigin(vec3f(0.0f, 0.0f, 0.0f));
    setCenter(vec3f(0.0f, 0.0f, 0.0f));

    m_keyModifier = 0;
    m_mouseButton = mbNone;
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
void ArcBallCamera::setZoom(float r)
{
    Clamp<float>(r, zoomMin, zoomMax);
    m_rho = r;
}

void ArcBallCamera::incrZoom(float delta) {
	setZoom(m_rho + delta);
}

//convert spherical coordinates to Eulerian values
vec3f ArcBallCamera::getPos() const
{
    vec3f p = m_origin;

    p.x += float(m_rho * sin(m_phi) * cos(m_omega));
    p.z += float(m_rho * sin(m_phi) * sin(m_omega));
    p.y += float(m_rho * cos(m_phi));
    return p;
}

//Return Current CCamera Direction
vec3f ArcBallCamera::getDir() const
{
    vec3f dir = m_origin - getPos();
    dir.normalize();
    return dir;
}

//Calculate an Up vector
vec3f ArcBallCamera::getUp() const
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

vec3f ArcBallCamera::getStrafe() const
{
    vec3f dir;
    dir = getPos();
    dir.normalize();

    dir = vec3f::cross(dir, getUp());
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

void ArcBallCamera::mousePress(int button, int state, int x, int y)
{
    PS_UNUSED(state);
    m_mouseButton = (MouseButton)button;
    m_lastPos = vec2i(x, y);
}

void ArcBallCamera::mouseMove(int x, int y)
{
    float dx = x - m_lastPos.x;
    float dy = m_lastPos.y - y;
    m_lastPos = vec2i(x, y);

    //Spherical movement on the left button
    if(m_mouseButton == mbMiddle)
    {
        setRoll(this->getRoll() + dx);
        setTilt(this->getTilt() + dy);

        if (m_keyModifier == GLUT_ACTIVE_CTRL) {
        	m_pan.x += 0.01f * dx;
        	m_pan.y += 0.01f * dy;
        }
    }
}

void ArcBallCamera::mouseWheel(int button, int dir, int x, int y)
{
    PS_UNUSED(button);
    PS_UNUSED(x);
    PS_UNUSED(y);
    //Zoom on Mouse Wheel
    if(dir > 0)
        setZoom(m_rho + 0.5);
    else
        setZoom(m_rho - 0.5);
}

void ArcBallCamera::look() const
{
    glTranslatef(m_pan.x, m_pan.y, 0.0f);
    vec3f p = this->getPos();
    vec3f c = this->getCenter();
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

}
