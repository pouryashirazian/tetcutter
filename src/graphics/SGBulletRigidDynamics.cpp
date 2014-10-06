/*
 * SGBulletRigidDynamics.cpp
 *
 *  Created on: Sep 7, 2014
 *      Author: pourya
 */


#include "SGBulletRigidDynamics.h"

using namespace PS::SG;

SGBulletRigidDynamics::SGBulletRigidDynamics():SGNode() {
	init();
}

SGBulletRigidDynamics::~SGBulletRigidDynamics() {
	cleanup();
}

void SGBulletRigidDynamics::init() {
	m_lpBroadPhase = new btDbvtBroadphase();
	m_lpCollisionConfig = new btDefaultCollisionConfiguration();

	m_lpDispatcher = new btCollisionDispatcher(m_lpCollisionConfig);
	m_lpSolver = new btSequentialImpulseConstraintSolver();

	m_lpDynamicsWorld = new btDiscreteDynamicsWorld(m_lpDispatcher, m_lpBroadPhase, m_lpSolver, m_lpCollisionConfig);
	m_lpDynamicsWorld->setGravity(btVector3(0, -10, 0));
}

void SGBulletRigidDynamics::cleanup() {
	delete m_lpDynamicsWorld;
	delete m_lpSolver;
	delete m_lpCollisionConfig;
	delete m_lpDispatcher;
	delete m_lpBroadPhase;
}

void SGBulletRigidDynamics::draw() {

}

void SGBulletRigidDynamics::timestep() {
	m_lpDynamicsWorld->stepSimulation(1 / 60.f, 10);

}

bool SGBulletRigidDynamics::addRigidBody(SGBulletRigidMesh* pMesh) {
	if(pMesh == NULL)
		return false;

	m_lpDynamicsWorld->addRigidBody(pMesh->getB3RigidBody());
	return true;
}

bool SGBulletRigidDynamics::removeRigidBody(SGBulletRigidMesh* pMesh) {
	if(pMesh == NULL)
		return false;

	m_lpDynamicsWorld->removeRigidBody(pMesh->getB3RigidBody());
	return true;
}

