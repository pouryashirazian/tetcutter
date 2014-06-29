#include "SplineCatmullRom.h"
#include "selectgl.h"
#include "base/MathBase.h"

#include <iostream>
#include <string>
#include <sstream>
#include <istream>

#define DEFAULT_NUM_POINTS_PER_SEGMENT 400

using namespace std;

SplineCatmullRom::SplineCatmullRom(const SplineCatmullRom& cspline) {
	m_bDrawCtrlLine = m_bDrawCtrlPoints = true;
	this->m_nSegments = cspline.m_nSegments;
	this->m_vCtrlPoints.assign(cspline.m_vCtrlPoints.begin(), cspline.m_vCtrlPoints.end());
	this->m_vArcTable.assign(cspline.m_vArcTable.begin(), cspline.m_vArcTable.end());
}

SplineCatmullRom::SplineCatmullRom(): m_nSegments(DEFAULT_NUM_POINTS_PER_SEGMENT)
{
	m_bDrawCtrlLine = m_bDrawCtrlPoints = true;
}

SplineCatmullRom::~SplineCatmullRom()
{
	cleanup();
}

void SplineCatmullRom::setSegmentsCount(U32 nSegments)
{
	if(nSegments > 0)		
		m_nSegments = nSegments;
}


bool SplineCatmullRom::populateTable()
{
	int nCtrlPoints = m_vCtrlPoints.size();
	if(nCtrlPoints < 4)
		return false;


	int n = m_nSegments;
	float u = (float)(1.0f /(float)(n-1));

	//Erase Table
	m_vArcTable.resize(n);

	for(int i=0; i < n; i++)
	{
		float cu = (float)(u * i);

		ARCLENGTHPARAM arcLenParam;
		arcLenParam.pt = position(cu);
		arcLenParam.u = cu;
		arcLenParam.g = 0.0f;
		if(i > 0)
		{
			arcLenParam.g = m_vArcTable[i-1].g + vec3f::distance(m_vArcTable[i-1].pt, arcLenParam.pt);
		}
		m_vArcTable[i] = arcLenParam;
	}

	//Normalize ArcLength Parameter
	float TotalArcLen = m_vArcTable[n-1].g;
	for(int i=0; i < n; i++)
	{
		m_vArcTable[i].g = m_vArcTable[i].g / TotalArcLen;
	}

	return (m_vArcTable.size() > 0);
}

bool SplineCatmullRom::populateTableAdaptive()
{
	int nCtrlPoints = m_vCtrlPoints.size();
	if(nCtrlPoints < 4)
		return false;

	//Erase Table
	m_vArcTable.clear();

	float delta = 0.001f;
	float cu = 0.0f;
	vec3f prevTan = tangent(0.0f);
	prevTan.normalize();
	vec3f curTan;
	int i = 0;

	do
	{
		ARCLENGTHPARAM arcLenParam;
				
		arcLenParam.pt = position(cu);
		arcLenParam.u = cu;
		arcLenParam.g = 0.0f;
		if(i > 0)
		{
			arcLenParam.g = m_vArcTable[i - 1].g + vec3f::distance(m_vArcTable[i - 1].pt, arcLenParam.pt);
		}
		m_vArcTable.push_back(arcLenParam);
		i++;
		
		//update Steps
		curTan = tangent(cu + delta);
		curTan.normalize();
		
		
		float angle = vec3f::angleDeg(curTan, prevTan);
		if(FLOAT_EQ(angle, 0.0f, 0.5f))
			delta *= 2.0f;
		else		
			delta *= 0.5f; 

		cu += delta;
		prevTan = curTan;
	}while(cu  < 1.0f);

	if(i>0)
	{
		if(m_vArcTable[i-1].u < 1.0f)
		{
			ARCLENGTHPARAM arcLenParam;
			arcLenParam.pt = position(1.0f);
			arcLenParam.u = 1.0f;
			arcLenParam.g = m_vArcTable[i - 1].g + vec3f::distance(m_vArcTable[i - 1].pt, arcLenParam.pt);
			m_vArcTable.push_back(arcLenParam);
		}
	}

	//Normalize ArcLength Parameter
	int n = m_vArcTable.size();
	float TotalArcLen = m_vArcTable[n - 1].g;
	for(i=0; i < n; i++)
	{
		m_vArcTable[i].g = m_vArcTable[i].g / TotalArcLen;
	}

	return (n > 0);
}

bool SplineCatmullRom::isTableEmpty() const
{
	return (m_vArcTable.size() == 0);
}

float SplineCatmullRom::arcLength() const
{
	U32 n = m_vArcTable.size();
	if(n > 0)
		return m_vArcTable[n-1].g;
	else
		return 0.0f;
}

float SplineCatmullRom::arcLengthByParam(float u) const
{
	if(isTableEmpty())
		return 0.0f;

	if(u < 0.0f)
		u = 0.0f;
	if(u > 1.0f)
		u = 1.0f;

	int n = m_vArcTable.size();
	float d = (float)(1.0f /(float)n);

	int i = static_cast<int>(floor(u / d));

	float arcLen;
	if((i >=0)&&(i < n - 1))
		arcLen = m_vArcTable[i].g + float((u - m_vArcTable[i].u)/(m_vArcTable[i + 1].u - m_vArcTable[i].u)) * (m_vArcTable[i+1].g - m_vArcTable[i].g);
	else 
		//	if(i == n - 1)
		//	arcLen = varcTable[i].g + float((u - varcTable[i].u)/(1.00f - varcTable[i].u)) * (1.00f - varcTable[i].g);
		//	else
		arcLen = m_vArcTable[n-1].g;

	return arcLen;
}

float SplineCatmullRom::paramByArcLength(float arcLength) const
{
	//if((arcLength < 0)||(arcLength > 1.0f))
	if(arcLength < 0)
		return 0.0f;
	if(isTableEmpty())
		return 0.0f;

	int n = m_vArcTable.size();
	int i;

	for (i = n - 1; i >= 0; i--)
	{
		if(arcLength > m_vArcTable[i].g)
			break;
	}

	float u,s;
	s = arcLength;

	if((i >=0)&&(i < n - 1))
		u = m_vArcTable[i].u + (s - m_vArcTable[i].g)/(m_vArcTable[i+1].g - m_vArcTable[i].g)*(m_vArcTable[i+1].u - m_vArcTable[i].u);
	else if(i == n - 1)
		u = m_vArcTable[i].u + (s - m_vArcTable[i].g)/(1.00f - m_vArcTable[i].g)*(1.00f - m_vArcTable[i].u);
	else
		u = 1.00f;


	return u;
}

void SplineCatmullRom::addCtrlPoint(const vec3f& p)
{
	m_vCtrlPoints.push_back(p);
}

void SplineCatmullRom::removeCtrlPoint(U32 i)
{
	if(isCtrlPointIndex(i))
		m_vCtrlPoints.erase(m_vCtrlPoints.begin() + i);
}

void SplineCatmullRom::cleanup()
{
	m_vCtrlPoints.clear();
	m_vArcTable.clear();
}

vec3f SplineCatmullRom::getCtrlPoint(U32 i) const
{
	return m_vCtrlPoints[i];
}

void SplineCatmullRom::setCtrlPoint(int index, const vec3f& pt)
{
	if(!isCtrlPointIndex(index))
		return;
	m_vCtrlPoints[index] = pt;
}

bool SplineCatmullRom::isCtrlPointIndex(U32 i) const
{
	return (i >= 0)&&(i < m_vCtrlPoints.size());
}

bool SplineCatmullRom::getLocalSpline(float t, float& local_t, int indices[4]) const
{
	int n = m_vCtrlPoints.size();
	if(n < 4)	
		return false;

	//clamps t value between two limits
    Clamp<float>(t, 0.0f, 1.0f);

	int nSlices = n - 3;
	int p = static_cast<int>(ceil(nSlices * t));
	if(p == 0) p++;

	indices[0] = p-1;
	indices[1] = p;
	indices[2] = p+1;
	indices[3] = p+2;


	float delta = (1.0f / static_cast<float>(nSlices));
	local_t = (t - static_cast<float>(p - 1) * delta) / delta;

	return true;
}


vec3f SplineCatmullRom::tangent(float t) const
{
	vec3f v;
	int indices[4];
	float local;

	if(!getLocalSpline(t, local, indices))	
	{
		return v;
	}

	// Interpolate
	return tangent(local, m_vCtrlPoints[indices[0]], m_vCtrlPoints[indices[1]], m_vCtrlPoints[indices[2]], m_vCtrlPoints[indices[3]]);
}

vec3f SplineCatmullRom::tangent(float t, const vec3f& p0, const vec3f& p1, const vec3f& p2, const vec3f& p3) const
{
	//p(u) = U^T * M * B
	//U^ = [3u^2 2u 1 0]

	//M Matrix =0.5 * [-1 3 -3  1]
	//				  [2 -5  4 -1]
	//				  [-1 0  1  0]
	//                [0  2  0  0]
	//B = [Pi-1 pi pi+1 pi+2]
	float t2 = t * t;

	float b0 = float(0.5 * (-3*t2 + 4*t - 1));
	float b1 = float(0.5 * (9*t2 -10*t));
	float b2 = float(0.5 * (-9*t2 + 8*t + 1));
	float b3 = float(0.5 * (3*t2 - 2*t));

	return (p0 * b0 + p1 * b1 + p2 * b2 + p3 * b3);
}

vec3f SplineCatmullRom::acceleration(float t) const
{
	vec3f v;
	int indices[4];
	float local;

	if(!getLocalSpline(t, local, indices))	
	{
		return v;
	}

	// Interpolate
	return acceleration(local, m_vCtrlPoints[indices[0]], m_vCtrlPoints[indices[1]], m_vCtrlPoints[indices[2]], m_vCtrlPoints[indices[3]]);
}

vec3f SplineCatmullRom::acceleration(float t, const vec3f& p0, const vec3f& p1, const vec3f& p2, const vec3f& p3) const
{
	//p(u) = U^T * M * B
	//U^^ = [6^u 2 0 0]

	//M Matrix =0.5 * [-1 3 -3  1]
	//                [2 -5  4 -1]
	//		  [-1 0  1  0]
	//                [0  2  0  0]
	//B = [Pi-1 pi pi+1 pi+2]

	float b0 = float(0.5 * (-6*t + 4));
	float b1 = float(0.5 * (18*t - 10));
	float b2 = float(0.5 * (-18*t + 8));
	float b3 = float(0.5 * (6*t - 2));

	return (p0 * b0 + p1 * b1 + p2 * b2 + p3 * b3);
}


vec3f SplineCatmullRom::position(float t)
{	
	vec3f v;
	int indices[4];
	float local;

	if(!getLocalSpline(t, local, indices))	
	{
		return v;
	}

	// Interpolate
	return position(local, m_vCtrlPoints[indices[0]], m_vCtrlPoints[indices[1]], m_vCtrlPoints[indices[2]], m_vCtrlPoints[indices[3]]);
}

float SplineCatmullRom::length(float t) {
	return arcLengthByParam(t);
}

vec3f SplineCatmullRom::position(float t, const vec3f& p0, const vec3f& p1, const vec3f& p2, const vec3f& p3) const
{
	//p(u) = U^T * M * B
	//U = [u^3 u^2 u^1 1]

	//M Matrix =0.5 * [-1 3 -3  1]
	//                [2 -5  4 -1]
	//				  [-1 0  1  0]
	//                [0  2  0  0]
	//B = [Pi-1 pi pi+1 pi+2]
	float t2 = t * t;
	float t3 = t2 * t;

	float b0 = float(0.5 * (-t3 + 2*t2 - t));
	float b1 = float(0.5 * (3*t3 -5*t2 + 2));
	float b2 = float(0.5 * (-3*t3 + 4*t2 + t));
	float b3 = float(0.5 * (t3 - t2));

	return (p0 * b0 + p1 * b1 + p2 * b2 + p3 * b3);
}

void SplineCatmullRom::draw() {
	if(m_bDrawCtrlLine)
		drawCtrlLine(GL_LINE_STRIP);

	if(m_bDrawCtrlPoints)
		drawCtrlLine(GL_POINTS);

	drawCurve(GL_LINE_STRIP);
}

void SplineCatmullRom::drawCtrlLine(U32 gl_draw_mode) const
{
	vec3f v;
	glPushAttrib(GL_ALL_ATTRIB_BITS);
		glBegin(gl_draw_mode);
		for(U32 i=0; i<m_vCtrlPoints.size(); i++)
		{
			v = m_vCtrlPoints[i];
			glVertex3f(v.x, v.y, v.z);
		}
		glEnd();
	glPopAttrib();
}


void SplineCatmullRom::drawCurve(U32 gl_draw_mode) const
{
	glPushAttrib(GL_ALL_ATTRIB_BITS);
		glBegin(gl_draw_mode);
		for(U32 i=0; i<m_vArcTable.size(); i++)
		{
			glVertex3fv(m_vArcTable[i].pt.cptr());
		}
		glEnd();
	glPopAttrib();
}


std::istream& operator >>( istream& ins, SplineCatmullRom& spline )
{
	int ctCtrlPoints = 0;	
	ins >> ctCtrlPoints;

	if(ctCtrlPoints > 0) {
		
		float x, y, z;		
		for(int i=0; i<ctCtrlPoints; i++)
		{
			ins >> x >> y >> z;
			spline.addCtrlPoint(vec3f(x,y, z));
		}
	}
	return ins;
}

std::ostream& operator <<( std::ostream& outs, const SplineCatmullRom& spline )
{
	int ctCtrlPoints = spline.countCtrlPts();

	if(ctCtrlPoints > 0)
	{
		outs << ctCtrlPoints;

		vec3f p;
		for(int i=0; i<ctCtrlPoints; i++)
		{
			p = spline.getCtrlPoint(i);
			outs << p.x << p.y << p.z;	
		}
	}

	return outs;
}
