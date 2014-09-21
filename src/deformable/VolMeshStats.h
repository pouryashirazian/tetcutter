/*
 * VolMeshStats.h
 *
 *  Created on: Sep 15, 2014
 *      Author: pourya
 */

#ifndef VOLMESHSTATS_H_
#define VOLMESHSTATS_H_

#include "VolMesh.h"

namespace PS {
namespace MESH {

class VolMeshStats {
public:
	VolMeshStats();
	virtual ~VolMeshStats();

	static void printAllStats(const VolMesh* pmesh);

	static bool computeVolMaxMin(const VolMesh* pmesh, double& outVolMax, double& outVolMin);
	static bool computeEdgeLenMaxMin(const VolMesh* pmesh, double& outEdgeLenMax, double& outEdgeLenMin);
	static bool computeMinAspectRatio(const VolMesh* pmesh, double& outMinAR);


};

} /* namespace MESH */
} /* namespace PS */

#endif /* VOLMESHSTATS_H_ */
