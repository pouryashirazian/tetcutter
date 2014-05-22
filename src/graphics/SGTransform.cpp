/*
 * SGTransform.cpp
 *
 *  Created on: Jun 7, 2013
 *      Author: pourya
 */

#include "SGTransform.h"
#include "selectgl.h"

namespace PS {
namespace SG {


SGTransform::SGTransform(bool bAutoUpdateBackward) {
	m_mtxForward.identity();
	m_mtxBackward.identity();
	m_bAutoUpdate = bAutoUpdateBackward;
}

SGTransform::SGTransform(const SGTransform* other) {
	m_mtxForward = other->m_mtxForward;
	m_mtxBackward = other->m_mtxBackward;
	m_bAutoUpdate = other->m_bAutoUpdate;
}

SGTransform::~SGTransform() {

}


void SGTransform::scale(const vec3f& s) {
	m_mtxForward.scale(s);
	if(m_bAutoUpdate)
		updateBackward();
}

void SGTransform::rotate(const quat& q) {
	mat44f work = mat44f::quatToMatrix(q);
	m_mtxForward = m_mtxForward * work;
	if(m_bAutoUpdate)
		updateBackward();

}

void SGTransform::rotate(const vec3f& axis, float deg) {
	quat q;
	q.fromAxisAngle(axis, deg);
	this->rotate(q);
}

void SGTransform::translate(const vec3f& t) {
	m_mtxForward.translate(t);
	if(m_bAutoUpdate)
		updateBackward();
}

void SGTransform::updateBackward() {
	m_mtxBackward = m_mtxForward.inverted();
}

void SGTransform::reset() {
	m_mtxForward.identity();
}

void SGTransform::bind() {

	glMatrixMode(GL_MODELVIEW_MATRIX);
	glPushMatrix();
	glMultMatrixf(m_mtxForward.cptr());
}

void SGTransform::unbind() {
	glMatrixMode(GL_MODELVIEW_MATRIX);
	glPopMatrix();
}

}
}

