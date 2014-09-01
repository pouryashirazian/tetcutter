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
	reset();
	m_autoUpdate = bAutoUpdateBackward;
}

SGTransform::SGTransform(const SGTransform& other):m_autoUpdate(false) {
	reset();
	this->copyFrom(other);
}

SGTransform::~SGTransform() {

}

void SGTransform::copyFrom(const SGTransform& other) {
	m_scale = other.m_scale;
	m_rotate = other.m_rotate;
	m_translate = other.m_translate;
	m_autoUpdate = other.m_autoUpdate;
	syncMatrices();
}

void SGTransform::scale(const vec3f& delta) {
	m_changes++;
	m_scale = m_scale + delta;
	if(m_autoUpdate)
		syncMatrices();
}

void SGTransform::scale(float sfactor) {
	m_changes++;
	m_scale = m_scale * sfactor;
	if(m_autoUpdate)
		syncMatrices();
}

void SGTransform::rotate(const quat& q) {
	m_changes++;
	m_rotate = quat::mul(m_rotate, q);
	if(m_autoUpdate)
		syncMatrices();

}

void SGTransform::rotate(const vec3f& axis, float deg) {
	quat q;
	q.fromAxisAngle(axis, deg);
	this->rotate(q);
}

void SGTransform::translate(const vec3f& delta) {
	m_changes++;
	m_translate = m_translate + delta;
	if(m_autoUpdate)
		syncMatrices();
}

void SGTransform::syncMatrices() {

	m_mtxForward.identity();

	//SRT is the right order
	m_mtxForward.translate(m_translate);
	m_mtxForward.scale(m_scale);
	m_mtxForward.rotate(m_rotate);

	m_mtxBackward = m_mtxForward.inverted();
	m_changes = 0;
}

void SGTransform::setScale(const vec3f& s) {
	m_changes++;
	m_scale = s;
	if(m_autoUpdate)
		syncMatrices();
}

void SGTransform::setRotate(const quat& r) {
	m_changes++;
	m_rotate = r;
	if(m_autoUpdate)
		syncMatrices();
}

void SGTransform::setTranslate(const vec3f& t) {
	m_changes++;
	m_translate = t;
	if(m_autoUpdate)
		syncMatrices();
}


void SGTransform::reset() {
	m_changes++;
	m_scale = vec3f(1,1,1);
	m_rotate.identity();
	m_translate = vec3f(0,0,0);
	if(m_autoUpdate)
		syncMatrices();
}

void SGTransform::resetScale() {
	setScale(vec3f(1,1,1));
}

void SGTransform::resetRotate() {
	setRotate(quatf(0, 0, 0, 1));
}

void SGTransform::resetTranslate() {
	setTranslate(vec3f(0,0,0));
}

void SGTransform::bind() {
	if(m_changes > 0)
		syncMatrices();

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

