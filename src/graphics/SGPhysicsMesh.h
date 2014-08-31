/*
 * SGPhysicsMesh.h
 *
 *  Created on: Aug 28, 2014
 *      Author: pourya
 */

#ifndef SGPHYSICSMESH_H_
#define SGPHYSICSMESH_H_

#include "SGMesh.h"
#include "Geometry.h"
#include <btBulletDynamicsCommon.h>

using namespace PS;
using namespace PS::GL;

namespace PS {
namespace SG {

class SGPhysicsMesh : public SGMesh {

public:
	SGPhysicsMesh();
	SGPhysicsMesh(const Geometry& g, float mass);
	virtual ~SGPhysicsMesh();

	virtual void setupPhysics(const Geometry& g, float mass);

	void updateNodeTransformFromMotionState();
	void updateMotionStateFromNodeTransform();



	btRigidBody* getB3RigidBody() const {return m_lpRigidBody;}
	btCollisionShape* getB3CollisionShape() const {return m_lpShape;}

	void draw();
	void timestep();

protected:
	void init();


protected:
	btRigidBody* m_lpRigidBody;
	btCollisionShape* m_lpShape;
};

}
}


#endif /* SGPHYSICSMESH_H_ */
