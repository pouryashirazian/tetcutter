/*
 * SGBulletSoftMesh.h
 *
 *  Created on: Sep 7, 2014
 *      Author: pourya
 */

#ifndef SGBULLETSOFTMESH_H_
#define SGBULLETSOFTMESH_H_

#include "SGMesh.h"
#include "Geometry.h"
#include <btBulletDynamicsCommon.h>
#include "BulletSoftBody/btSoftBody.h"

using namespace PS;
using namespace PS::GL;

namespace PS {
namespace SG {

class SGBulletSoftMesh : public SGMesh {

public:
	SGBulletSoftMesh();
	SGBulletSoftMesh(btSoftBody* aSoftBody);
	virtual ~SGBulletSoftMesh();
	btSoftBody* getB3SoftBody() const {return m_lpSoftBody;}

	void draw();
	void timestep();

	virtual void setup(const btSoftBody* pBody, float mass = 1.0f);
protected:
	void init();
	void cleanup();


protected:
	btSoftBody* m_lpSoftBody;
};

}
}



#endif /* SGBULLETSOFTMESH_H_ */
