/*
 * VolMeshSamples.h
 *
 *  Created on: Aug 7, 2014
 *      Author: hupf2020
 */

#ifndef VOLMESHSAMPLES_H_
#define VOLMESHSAMPLES_H_


#include "VolMesh.h"

namespace PS {
namespace MESH {


class VolMeshSamples{
public:
	VolMeshSamples() {}

	static VolMesh* CreateOneTetra();
	static VolMesh* CreateTwoTetra();
	static VolMesh* CreateTruthCube(int nx, int ny, int nz, double cellsize);
	static VolMesh* CreateEggShell(int hseg = 8, int vseg = 8, double radius = 2.0, double shelltickness = 0.3);
};

}
}
#endif /* VOLMESHSAMPLES_H_ */
