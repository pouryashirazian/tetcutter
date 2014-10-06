/*
 * SGBulletRigidMesh.cpp
 *
 *  Created on: Aug 28, 2014
 *      Author: pourya
 */

#include "SGBulletRigidMesh.h"

using namespace PS::SG;

SGBulletRigidMesh::SGBulletRigidMesh():SGMesh() {
	init();
}

SGBulletRigidMesh::SGBulletRigidMesh(const Geometry& g, float mass) {
	init();

	setup(g, mass);
}

SGBulletRigidMesh::SGBulletRigidMesh(const SGTransform& t, const Geometry& g, float mass) {
	init();
	this->transform()->copyFrom(t);

	setup(g, mass);
}

SGBulletRigidMesh::~SGBulletRigidMesh() {
	cleanup();
}

void SGBulletRigidMesh::init() {
	m_lpRigidBody = NULL;
	resetTransform();
}

void SGBulletRigidMesh::cleanup() {

	btCollisionShape* pShape = NULL;
	if(m_lpRigidBody) {
		pShape = m_lpRigidBody->getCollisionShape();
		delete (m_lpRigidBody->getMotionState());
		SAFE_DELETE(m_lpRigidBody);
	}
	SAFE_DELETE(pShape);
}

void SGBulletRigidMesh::setup(const Geometry& g, float mass) {
	//1
	btQuaternion rotation(0.0, 0.0, 0.0, 1.0);

	//2
	vec3f t = this->transform()->getTranslate();
	btVector3 position = btVector3(t.x, t.y, t.z);

	//3
	btDefaultMotionState* motionState = new btDefaultMotionState(
			btTransform(rotation, position));

	//4
	btScalar bodyMass = mass;
	btVector3 bodyInertia(0, 0, 0);
	btConvexHullShape* pCDShape = new btConvexHullShape();
	for (int i = 0; i < g.countVertices(); i++) {
		vec3f v = g.vertexAt(i);
		btVector3 btv = btVector3(v.x, v.y, v.z);
		pCDShape->addPoint(btv);
	}
	pCDShape->calculateLocalInertia(bodyMass, bodyInertia);
	pCDShape->setMargin(0.04);

	//5
	btRigidBody::btRigidBodyConstructionInfo bodyCI =
			btRigidBody::btRigidBodyConstructionInfo(bodyMass, motionState,
					pCDShape, bodyInertia);

	//6
	bodyCI.m_restitution = 1.0f;
	bodyCI.m_friction = 0.5f;

	//7
	m_lpRigidBody = new btRigidBody(bodyCI);

	//8
	m_lpRigidBody->setUserPointer((void*)this);

	//9
	m_lpRigidBody->setLinearFactor(btVector3(1, 1, 0));

	//setup mesh
	SGMesh::setup(g);

	//animate
	setAnimate(true);

	if(TheShaderManager::Instance().has("phong")) {
        m_spEffect = SmartPtrSGEffect(new SGEffect(TheShaderManager::Instance().get("phong")));
    }
}

void SGBulletRigidMesh::setup(const Geometry& g, const btRigidBody* pBody) {
	m_lpRigidBody = const_cast<btRigidBody*>(pBody);

	//setup mesh
	SGMesh::setup(g);

	//animate
	setAnimate(true);

	if(TheShaderManager::Instance().has("phong")) {
        m_spEffect = SmartPtrSGEffect(new SGEffect(TheShaderManager::Instance().get("phong")));
    }
}

void SGBulletRigidMesh::updateNodeTransformFromMotionState() {

	//btScalar m[16];
	btTransform trans = m_lpRigidBody->getWorldTransform();

	//rotate
	btQuaternion q = trans.getRotation();
	transform()->setRotate(quat(q.x(), q.y(), q.z(), q.w()));

	//translate
	btVector3 t = trans.getOrigin();
	transform()->setTranslate(vec3f(t.x(), t.y(), t.z()));
}

void SGBulletRigidMesh::updateMotionStateFromNodeTransform() {
	if(!m_lpRigidBody->getMotionState())
		return;

	vec3f t = transform()->getTranslate();

	btTransform trans(btQuaternion(0, 0, 0, 1), btVector3(t.x, t.y, t.z));
	m_lpRigidBody->getMotionState()->setWorldTransform(trans);
}


void SGBulletRigidMesh::draw() {

	SGMesh::draw();
}

void SGBulletRigidMesh::timestep() {
	if(m_lpRigidBody == NULL)
		return;

	updateNodeTransformFromMotionState();
}




