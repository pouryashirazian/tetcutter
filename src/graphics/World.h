/*
 * CollisionDetection.h
 *
 *  Created on: Aug 28, 2014
 *      Author: pourya
 */

#ifndef COLLISIONDETECTION_H_
#define COLLISIONDETECTION_H_

#include "btBulletDynamicsCommon.h"
#include "SGPhysicsMesh.h"

namespace PS {
namespace SG {


class World {
public:
	World();
	virtual ~World();

	bool addNode(SGPhysicsMesh* pMesh);
	bool removeNode(SGPhysicsMesh* pMesh);
	void step();
private:
	btBroadphaseInterface* m_lpBroadPhase;
	btDefaultCollisionConfiguration* m_lpCollisionConfig;
	btCollisionDispatcher* m_lpDispatcher;
	btSequentialImpulseConstraintSolver* m_lpSolver;
	btDiscreteDynamicsWorld* m_lpDynamicsWorld;

};

}
}


#endif /* COLLISIONDETECTION_H_ */
