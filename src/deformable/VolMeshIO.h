/*
 * VolMeshExport.h
 *
 *  Created on: Aug 16, 2014
 *      Author: pourya
 */

#ifndef VOLMESHEXPORT_H_
#define VOLMESHEXPORT_H_

#include "VolMesh.h"

namespace PS {
namespace MESH {

class VolMeshIO {
public:

	static bool readVega(VolMesh* vm, const AnsiStr& strPath);
	static bool writeVega(const VolMesh* vm, const AnsiStr& strPath);

	//only export to obj file for inspection purposes
	static bool writeObj(const VolMesh* vm, const AnsiStr& strPath);

	static bool fitmesh(VolMesh* vm, const AABB& toBox);
	static bool rotatemesh(VolMesh* vm, const quatd& quat);


	static bool convertMatlabTextToVega(const AnsiStr& strNodesFP,
										const AnsiStr& strFacesFP,
								   	    const AnsiStr& strCellsFP);
};

}
}



#endif /* VOLMESHEXPORT_H_ */
