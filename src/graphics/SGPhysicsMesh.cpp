/*
 * SGPhysicsMesh.cpp
 *
 *  Created on: Aug 28, 2014
 *      Author: pourya
 */

#include "SGPhysicsMesh.h"

using namespace PS::SG;

SGPhysicsMesh::SGPhysicsMesh():SGMesh() {
	init();
}

SGPhysicsMesh::SGPhysicsMesh(const Geometry& g, float mass) {
	init();

	setupPhysics(g, mass);
}

SGPhysicsMesh::~SGPhysicsMesh() {
	delete (m_lpRigidBody->getMotionState());
	SAFE_DELETE(m_lpRigidBody);
	SAFE_DELETE(m_lpShape);
}

void SGPhysicsMesh::init() {
	m_lpRigidBody = NULL;
	m_lpShape = NULL;
}

void SGPhysicsMesh::setupPhysics(const Geometry& g, float mass) {

	//setup mesh
	setup(g);

	//1
	btQuaternion rotation(btVector3(1.0, 0.0, 0.0), 0.0);

	//2
	vec3f t = this->transform()->getTranslate();
	btVector3 position = btVector3(t.x, t.y, t.z);

	//3
	btDefaultMotionState* motionState = new btDefaultMotionState(
			btTransform(rotation, position));

	//4
	btScalar bodyMass = mass;
	btVector3 bodyInertia(0, 0, 0);
	m_lpShape = new btConvexHullShape();
	for (int i = 0; i < g.countVertices(); i++) {
		vec3f v = g.vertexAt(i);
		btVector3 btv = btVector3(v.x, v.y, v.z);
		((btConvexHullShape*) m_lpShape)->addPoint(btv);
	}
	m_lpShape->calculateLocalInertia(bodyMass, bodyInertia);

	//5
	btRigidBody::btRigidBodyConstructionInfo bodyCI =
			btRigidBody::btRigidBodyConstructionInfo(bodyMass, motionState,
					m_lpShape, bodyInertia);

	//6
	bodyCI.m_restitution = 1.0f;
	bodyCI.m_friction = 0.5f;

	//7
	m_lpRigidBody = new btRigidBody(bodyCI);

	//8
	m_lpRigidBody->setUserPointer((void*)this);

	//9
	m_lpRigidBody->setLinearFactor(btVector3(1, 1, 0));

	//animate
	setAnimate(true);
}

void SGPhysicsMesh::updateNodeTransformFromMotionState() {

	//btScalar m[16];
	btTransform trans;
	m_lpRigidBody->getMotionState()->getWorldTransform(trans);
	btVector3 t = trans.getOrigin();
	vec3f tt = vec3f(t.x(), t.y(), t.z());

	transform()->translate(tt - transform()->getTranslate());
	//trans.getOpenGLMatrix(m);
//	vec3f s = transform()->getScale();
//	mat44f mtxTR;
//	mtxTR.copyFrom(m);
//	transform()->set(mtxTR);
	//transform()->scale(s);
}

void SGPhysicsMesh::updateMotionStateFromNodeTransform() {
	if(!m_lpRigidBody->getMotionState())
		return;

	vec3f t = transform()->getTranslate();

	btTransform trans(btQuaternion(0, 0, 0, 1), btVector3(t.x, t.y, t.z));
	m_lpRigidBody->getMotionState()->setWorldTransform(trans);
}


void SGPhysicsMesh::draw() {
	SGMesh::draw();
}

void SGPhysicsMesh::timestep() {
	if(m_lpRigidBody == NULL)
		return;

	updateNodeTransformFromMotionState();
}




