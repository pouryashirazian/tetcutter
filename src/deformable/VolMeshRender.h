/*
 * VolMeshRender.h
 *
 *  Created on: Oct 5, 2014
 *      Author: pourya
 */

#ifndef VOLMESHRENDER_H_
#define VOLMESHRENDER_H_

#include <graphics/SGMesh.h>
#include "deformable/VolMesh.h"

namespace PS {
namespace MESH {

class VolMeshRender: public SG::SGMesh {
public:
	VolMeshRender();
	VolMeshRender(const VolMesh* pmesh);
	virtual ~VolMeshRender();

	bool sync(const VolMesh* pmesh);

	void draw();

private:
//	bool m_flagDrawSurface;
//	bool m_flagDrawWireframe;
//	bool m_flagDrawVertices;
	SGMesh m_sgWireFrame;
	SGMesh m_sgVertices;
};

} /* namespace MESH */
} /* namespace PS */

#endif /* VOLMESHRENDER_H_ */
