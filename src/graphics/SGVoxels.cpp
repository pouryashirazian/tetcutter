/*
 * SGVoxels.cpp
 *
 *  Created on: Apr 6, 2014
 *      Author: pourya
 */

#include "SGVoxels.h"

namespace PS {
namespace SG {

SGVoxels::SGVoxels(const vector<vec3f>& voxels, float side) {
	this->setName("voxels");

	Geometry g;
	g.init(3, 4, 2, ftQuads);

	vec3f sides(side);
	for(U32 i=0; i<voxels.size(); i++) {
		g.addCube(voxels[i], voxels[i] + side);
	}

	this->setup(g);
}

SGVoxels::~SGVoxels() {
	SGMesh::cleanup();
}

void SGVoxels::draw() {
	SGMesh::draw();
}

}
}
