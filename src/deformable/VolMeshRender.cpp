/*
 * VolMeshRender.cpp
 *
 *  Created on: Oct 5, 2014
 *      Author: pourya
 */

#include <deformable/VolMeshRender.h>

namespace PS {
namespace MESH {

VolMeshRender::VolMeshRender() {
	// TODO Auto-generated constructor stub
	resetTransform();
	if(TheShaderManager::Instance().has("phong")) {
        m_spEffect = SmartPtrSGEffect(new SGEffect(TheShaderManager::Instance().get("phong")));
    }

}

VolMeshRender::VolMeshRender(const VolMesh* pmesh) {
	// TODO Auto-generated constructor stub
	resetTransform();
	if(TheShaderManager::Instance().has("phong")) {
        m_spEffect = SmartPtrSGEffect(new SGEffect(TheShaderManager::Instance().get("phong")));
    }

	sync(pmesh);
}


VolMeshRender::~VolMeshRender() {
	// TODO Auto-generated destructor stub
	SGMesh::cleanup();
}

bool VolMeshRender::sync(const VolMesh* pmesh) {
	if(pmesh == NULL)
		return false;

//	for(U32 i=0; i < pmesh->countNodes(); i++) {
//		NODE& n = pmesh->const_nodeAt(i);
//	}


	return true;
}

} /* namespace MESH */
} /* namespace PS */
