/*
 * AvatarRing.h
 *
 *  Created on: Sep 21, 2014
 *      Author: pourya
 */

#ifndef AVATARRING_H_
#define AVATARRING_H_

#include "IScalpel.h"
#include "graphics/GLTexture.h"

using namespace PS::GL;

namespace PS {
namespace MESH {

class AvatarRing: public IAvatar {
public:
	AvatarRing(GLTexture* aTex = NULL);
	virtual ~AvatarRing();

	void init();
	void draw();

	//From Gizmo Manager
	void onTranslate(const vec3f& delta, const vec3f& pos);
	void clearCutContext();
protected:
	GLTexture* m_lpTex;

	//flags
	bool m_isSweptQuadValid;
	AABB m_aabbCurrent;

	//Outline mesh for easier view
	SGMesh m_outline;

	//cut info
	vector<vec3d> m_vCuttingPath;
	vector<vec3d> m_vSweptQuads;
	vector<vec3d> m_vSegmentsRef;
	vector<vec3d> m_vSegmentsCur;
};

} /* namespace MESH */
} /* namespace PS */

#endif /* AVATARRING_H_ */
