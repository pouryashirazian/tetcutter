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

class SGBulletRigidMesh : public SGMesh {

public:
	SGBulletRigidMesh();
	SGBulletRigidMesh(const Geometry& g, float mass);
	SGBulletRigidMesh(const SGTransform& t, const Geometry& g, float mass);

	virtual ~SGBulletRigidMesh();

	virtual void setup(const Geometry& g, float mass);
	virtual void setup(const Geometry& g, const btRigidBody* pBody);

	void updateNodeTransformFromMotionState();
	void updateMotionStateFromNodeTransform();

	btRigidBody* getB3RigidBody() const {return m_lpRigidBody;}

	void draw();
	void timestep();

protected:
	void init();
	void cleanup();


protected:
	btRigidBody* m_lpRigidBody;
};

}
}


#endif /* SGPHYSICSMESH_H_ */
