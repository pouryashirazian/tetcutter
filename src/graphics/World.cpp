/*
 * World.cpp
 *
 *  Created on: Aug 28, 2014
 *      Author: pourya
 */

#include "World.h"

using namespace PS::SG;

World::World() {
	m_lpBroadPhase = new btDbvtBroadphase();
	m_lpCollisionConfig = new btDefaultCollisionConfiguration();

	m_lpDispatcher = new btCollisionDispatcher(m_lpCollisionConfig);
	m_lpSolver = new btSequentialImpulseConstraintSolver();

	m_lpDynamicsWorld = new btDiscreteDynamicsWorld(m_lpDispatcher, m_lpBroadPhase, m_lpSolver, m_lpCollisionConfig);
	m_lpDynamicsWorld->setGravity(btVector3(0, -10, 0));
}

World::~World() {
	delete m_lpDynamicsWorld;
	delete m_lpSolver;
	delete m_lpCollisionConfig;
	delete m_lpDispatcher;
	delete m_lpBroadPhase;
}

void World::step() {
	m_lpDynamicsWorld->stepSimulation(1 / 60.f, 10);

}

bool World::addNode(SGPhysicsMesh* pMesh) {
	if(pMesh == NULL)
		return false;

	m_lpDynamicsWorld->addRigidBody(pMesh->getB3RigidBody());
	return true;
}

bool World::removeNode(SGPhysicsMesh* pMesh) {
	if(pMesh == NULL)
		return false;

	m_lpDynamicsWorld->removeRigidBody(pMesh->getB3RigidBody());
	return true;
}





