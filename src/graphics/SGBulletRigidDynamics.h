/*
 * SGBulletRigidDynamics.h
 *
 *  Created on: Sep 7, 2014
 *      Author: pourya
 */

#ifndef SGBULLETRIGIDDYNAMICS_H_
#define SGBULLETRIGIDDYNAMICS_H_

#include "btBulletDynamicsCommon.h"
#include "SGBulletRigidMesh.h"
#include "SGNode.h"

namespace PS {
namespace SG {


class SGBulletRigidDynamics : public SGNode {
public:
	SGBulletRigidDynamics();
	virtual ~SGBulletRigidDynamics();

	bool addRigidBody(SGBulletRigidMesh* pMesh);
	bool removeRigidBody(SGBulletRigidMesh* pMesh);

	void draw();
	void timestep();

protected:
	void init();
	void cleanup();

private:
	btBroadphaseInterface* m_lpBroadPhase;
	btDefaultCollisionConfiguration* m_lpCollisionConfig;
	btCollisionDispatcher* m_lpDispatcher;
	btSequentialImpulseConstraintSolver* m_lpSolver;
	btDiscreteDynamicsWorld* m_lpDynamicsWorld;

};

}
}



#endif /* SGBULLETRIGIDDYNAMICS_H_ */
