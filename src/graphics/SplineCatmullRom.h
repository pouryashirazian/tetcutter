#ifndef SPLINECATMULLROM_H
#define SPLINECATMULLROM_H

#include <vector>
#include <istream>
#include "base/Vec.h"
#include "graphics/SGMesh.h"
#include "graphics/ProfileCurve.h"

#define MIN_CTRL_POINTS_COUNT 4

using namespace std;
using namespace PS::MATH;
using namespace PS::MODEL;


class SplineCatmullRom : public IProfileCurve, public SGNode
{

public:
	struct ARCLENGTHPARAM{
		float u;//Parameter
		float g;//ArcLength Accumulated
		vec3f pt;

		void operator=(const ARCLENGTHPARAM& other) {
			u = other.u;
			g = other.g;
			pt = other.pt;
		}
	};

public:
	SplineCatmullRom();
	SplineCatmullRom(const SplineCatmullRom& cspline);
	virtual ~SplineCatmullRom();

	bool isValid() const { return ((m_vCtrlPoints.size() >= 4) && (m_vArcTable.size() > 0)); }
	void cleanup();

	//Ctrl Points
	U32 countCtrlPts() const {return m_vCtrlPoints.size();}
	vec3f  getCtrlPoint(U32 i) const;
	void setCtrlPoint(int index, const vec3f& pt);
	void addCtrlPoint(const vec3f& p);
	void removeCtrlPoint(U32 i);

	//build the spline
	void build() {
		assert(populateTable() == true);
	}

	//Segment count
	U32 getSegmentsCount() const { return m_nSegments; }
	void setSegmentsCount(U32 nSegments);

	//ctrl line and ctrl points draw
	bool getDrawCtrlLine() const { return m_bDrawCtrlLine;}
	void setDrawCtrlLine(bool bdraw) { m_bDrawCtrlLine = bdraw;}

	bool getDrawCtrlPoints() const { return m_bDrawCtrlPoints;}
	void setDrawCtrlPoints(bool bdraw) { m_bDrawCtrlPoints = bdraw;}


	//Get Position Tangent and Acceleration
	vec3f position(float t) override;
	float length(float t) override;

	vec3f position(float t, const vec3f& p0, const vec3f& p1, const vec3f& p2, const vec3f& p3) const;
	vec3f tangent(float t) const;
	vec3f tangent(float t, const vec3f& p0, const vec3f& p1, const vec3f& p2, const vec3f& p3) const;
	vec3f acceleration(float t) const;
	vec3f acceleration(float t, const vec3f& p0, const vec3f& p1, const vec3f& p2, const vec3f& p3) const;
	float arcLength() const;


	void draw();
	void drawCtrlLine(U32 gl_draw_mode = 0) const;
	void drawCurve(U32 gl_draw_mode = 3) const;

	//Serialize Control Points
	friend std::ostream& operator <<(std::ostream& outs, const SplineCatmullRom& spline);
	friend std::istream& operator >>(std::istream& ins, const SplineCatmullRom& spline);

protected:
	bool isCtrlPointIndex(U32 i) const;
	bool getLocalSpline(float t, float &local_t, int indices[4]) const;

	//Arc Length Parameterization
	bool populateTable();
	bool populateTableAdaptive();

	//Access via arc length and param
	float arcLengthByParam(float u) const;
	float paramByArcLength(float arcLength) const;
	bool isTableEmpty() const;


private:	
	bool m_bDrawCtrlLine;
	bool m_bDrawCtrlPoints;
	U32  m_nSegments;

	vector<vec3f> m_vCtrlPoints;
	vector<ARCLENGTHPARAM> m_vArcTable;
};


#endif
