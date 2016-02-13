/*
 * VolMeshRender.h
 *
 *  Created on: Oct 5, 2014
 *      Author: pourya
 */

#ifndef VOLMESHRENDER_H_
#define VOLMESHRENDER_H_

#include "scene/sgmesh.h"
#include "elastic/volmesh.h"

namespace ps {
namespace elastic {

class VolMeshRender: public SGMesh {
public:
	VolMeshRender();
	VolMeshRender(const VolMesh* pmesh);
	virtual ~VolMeshRender();

	bool sync(const VolMesh* pmesh);

	void draw();

protected:
	void init();

private:
//	bool m_flagDrawSurface;
//	bool m_flagDrawWireframe;
//	bool m_flagDrawVertices;
	SGMesh m_sgWireFrame;
	SGMesh m_sgVertices;
	SGMesh m_sgNormals;
};

} /* namespace MESH */
} /* namespace PS */

#endif /* VOLMESHRENDER_H_ */
