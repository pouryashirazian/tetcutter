/*
 * SGSphere.cpp
 *
 *  Created on: Mar 31, 2014
 *      Author: pourya
 */
#include "Geometry.h"
#include "SGSphere.h"

namespace PS {
namespace SG {

SGSphere::SGSphere(float radius, int hseg, int vseg) {

	Geometry g;
	g.addSphere(radius, hseg, vseg);

	this->setup(g);
}

SGSphere::~SGSphere() {
	GLMeshBuffer::cleanup();
}


}
}
