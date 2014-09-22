/*
 * AvatarScalpel.h
 *
 *  Created on: Apr 6, 2014
 *      Author: pourya
 */

#ifndef AVATARSCALPEL_H_
#define AVATARSCALPEL_H_

#include "IScalpel.h"

using namespace PS;
using namespace PS::SG;


/*!
 * Synopsis: Haptics Avatar guide
 */
class AvatarScalpel : public IAvatar {
public:
	AvatarScalpel();
	AvatarScalpel(CuttableMesh* tissue);
	virtual ~AvatarScalpel();

	void init();
	void draw();

	//From Gizmo Manager
	void onTranslate(const vec3f& delta, const vec3f& pos);
	void clearCutContext();


protected:
	//flags
	bool m_isSweptQuadValid;
	AABB m_aabbCurrent;

	//Outline mesh for easier view
	SGMesh m_outline;

	//blade edges
	vec3f m_edgeref0;
	vec3f m_edgeref1;

	//cut info
	vector<vec3d> m_vCuttingPathEdge0;
	vector<vec3d> m_vCuttingPathEdge1;
	vector<vec3d> m_vSweptQuad;
	vector<vec3d> m_vBladeSegments;
};



#endif /* AVATARSCALPEL_H_ */
