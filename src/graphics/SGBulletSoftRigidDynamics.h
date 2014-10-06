/*
 * CollisionDetection.h
 *
 *  Created on: Aug 28, 2014
 *      Author: pourya
 */

#ifndef SGBULLETSOFTRIGIDYNAMICS_H
#define SGBULLETSOFTRIGIDYNAMICS_H

#include "btBulletDynamicsCommon.h"
#include "BulletSoftBody/btSoftBody.h"
#include "BulletSoftBody/btSoftBodyRigidBodyCollisionConfiguration.h"
#include "BulletSoftBody/btSoftRigidDynamicsWorld.h"
#include "SGBulletRigidMesh.h"
#include "SGBulletSoftMesh.h"
#include "SGNode.h"

namespace PS {
namespace SG {


class SGBulletSoftRigidDynamics : public SGNode {
public:
	static const int maxProxies = 32766;
	static const int maxOverlap = 65535;

	SGBulletSoftRigidDynamics();
	virtual ~SGBulletSoftRigidDynamics();

	void addRawRigidBody(btRigidBody* pRigidBody);
	void removeRawRigidBody(btRigidBody* pRigidBody);

	void addRawSoftBody(btSoftBody* pSoftBody);
	void removeRawSoftBody(btSoftBody* pSoftBody);

	bool addRigidBody(SGBulletRigidMesh* pMesh);
	bool removeRigidBody(SGBulletRigidMesh* pMesh);

	bool addSoftBody(SGBulletSoftMesh* pMesh);
	bool removeSoftBody(SGBulletSoftMesh* pMesh);

	virtual const btSoftRigidDynamicsWorld*	getSoftDynamicsWorld() const;
	const btSoftBodyWorldInfo& getSoftBodyWorldInfo() const;
	btSoftBodyWorldInfo& getSoftBodyWorldInfo();

	void draw();
	void timestep();


protected:
	void init();
	void cleanup();

private:
	btSoftBodyWorldInfo	m_softBodyWorldInfo;

	btBroadphaseInterface* m_lpBroadPhase;
	btDefaultCollisionConfiguration* m_lpCollisionConfig;
	btCollisionDispatcher* m_lpDispatcher;
	btConstraintSolver* m_lpSolver;
	btSoftRigidDynamicsWorld* m_lpDynamicsWorld;

};

}
}


#endif /* SGBULLETSOFTRIGIDYNAMICS_H */
