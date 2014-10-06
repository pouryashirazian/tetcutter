/*
 * World.cpp
 *
 *  Created on: Aug 28, 2014
 *      Author: pourya
 */

#include "SGBulletSoftRigidDynamics.h"


using namespace PS::SG;

SGBulletSoftRigidDynamics::SGBulletSoftRigidDynamics():SGNode() {
	init();
}

SGBulletSoftRigidDynamics::~SGBulletSoftRigidDynamics() {
	cleanup();
}

void SGBulletSoftRigidDynamics::init() {
	///register some softbody collision algorithms on top of the default btDefaultCollisionConfiguration
	m_lpCollisionConfig = new btSoftBodyRigidBodyCollisionConfiguration();


	m_lpDispatcher = new btCollisionDispatcher(m_lpCollisionConfig);
	m_softBodyWorldInfo.m_dispatcher = m_lpDispatcher;

	btVector3 worldAabbMin(-1000,-1000,-1000);
	btVector3 worldAabbMax(1000,1000,1000);
	m_lpBroadPhase = new btAxisSweep3(worldAabbMin, worldAabbMax, maxProxies);
	m_softBodyWorldInfo.m_broadphase = m_lpBroadPhase;

	m_lpSolver = new btSequentialImpulseConstraintSolver();

	btSoftBodySolver* softBodySolver = 0;
	m_lpDynamicsWorld = new btSoftRigidDynamicsWorld(m_lpDispatcher, m_lpBroadPhase, m_lpSolver, m_lpCollisionConfig, softBodySolver);
	//m_lpDynamicsWorld->setInternalTickCallback(pickingPreTickCallback,this,true);

	m_lpDynamicsWorld->getDispatchInfo().m_enableSPU = true;
	m_lpDynamicsWorld->setGravity(btVector3(0, -10, 0));
	m_softBodyWorldInfo.m_gravity.setValue(0, -10, 0);
	m_softBodyWorldInfo.m_sparsesdf.Initialize();
}

void SGBulletSoftRigidDynamics::cleanup() {
	delete m_lpDynamicsWorld;
	delete m_lpSolver;
	delete m_lpCollisionConfig;
	delete m_lpDispatcher;
	delete m_lpBroadPhase;
}

void SGBulletSoftRigidDynamics::draw() {

}

const btSoftRigidDynamicsWorld* SGBulletSoftRigidDynamics::getSoftDynamicsWorld() const {
	///just make it a btSoftRigidDynamicsWorld please
	///or we will add type checking
	return m_lpDynamicsWorld;
}

const btSoftBodyWorldInfo& SGBulletSoftRigidDynamics::getSoftBodyWorldInfo() const {
	return m_softBodyWorldInfo;
}

btSoftBodyWorldInfo& SGBulletSoftRigidDynamics::getSoftBodyWorldInfo() {
	return m_softBodyWorldInfo;
}


void SGBulletSoftRigidDynamics::timestep() {
	m_lpDynamicsWorld->stepSimulation(1 / 60.f, 10);

}

void SGBulletSoftRigidDynamics::addRawRigidBody(btRigidBody* pRigidBody) {
	m_lpDynamicsWorld->addRigidBody(pRigidBody);
}

void SGBulletSoftRigidDynamics::removeRawRigidBody(btRigidBody* pRigidBody) {
	m_lpDynamicsWorld->removeRigidBody(pRigidBody);
}

void SGBulletSoftRigidDynamics::addRawSoftBody(btSoftBody* pSoftBody) {
	m_lpDynamicsWorld->addSoftBody(pSoftBody);
}

void SGBulletSoftRigidDynamics::removeRawSoftBody(btSoftBody* pSoftBody) {
	m_lpDynamicsWorld->removeSoftBody(pSoftBody);
}


bool SGBulletSoftRigidDynamics::addRigidBody(SGBulletRigidMesh* pMesh) {
	if(pMesh == NULL)
		return false;

	addRawRigidBody(pMesh->getB3RigidBody());
	return true;
}

bool SGBulletSoftRigidDynamics::removeRigidBody(SGBulletRigidMesh* pMesh) {
	if(pMesh == NULL)
		return false;

	removeRawRigidBody(pMesh->getB3RigidBody());
	return true;
}

bool SGBulletSoftRigidDynamics::addSoftBody(SGBulletSoftMesh* pMesh) {
	if(pMesh == NULL)
		return false;

	addRawSoftBody(pMesh->getB3SoftBody());
	return true;
}

bool SGBulletSoftRigidDynamics::removeSoftBody(SGBulletSoftMesh* pMesh) {
	if(pMesh == NULL)
		return false;

	removeRawSoftBody(pMesh->getB3SoftBody());
	return true;
}



