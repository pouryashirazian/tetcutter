/*
 * VolMeshRender.cpp
 *
 *  Created on: Oct 5, 2014
 *      Author: pourya
 */

#include <deformable/VolMeshRender.h>
#include "base/FlatArray.h"
#include "graphics/SceneGraph.h"
#include "graphics/selectgl.h"

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

	//cleanup the mesh
	cleanup();

	//nodes
	vector<double> vNodes;
	vector<double> vNormals;
	vNodes.resize(pmesh->countNodes() * 3);
	vNormals.resize(pmesh->countNodes() * 3);

	//render for high performance
	for(U32 i=0; i < pmesh->countNodes(); i++) {
		const NODE& n = pmesh->const_nodeAt(i);

		n.pos.store(&vNodes[i * 3]);
	}

	vector<vec3d> vFaceNormals;
	vFaceNormals.resize(pmesh->countFaces());

	vector<vec3d> vNodalNormals;
	vNodalNormals.resize(pmesh->countNodes());

	vector<U32> vNodalCount;
	vNodalCount.resize(pmesh->countNodes());
	std::fill(vNodalCount.begin(), vNodalCount.end(), 0);

	vector<U32> vIndices;
	vIndices.resize(pmesh->countFaces() * 3);

	for(U32 i=0; i < pmesh->countFaces(); i++) {
		U32 nodes[3];
		pmesh->getFaceNodes(i, nodes);

		vec3d p0 = pmesh->const_nodeAt(nodes[0]).pos;
		vec3d p1 = pmesh->const_nodeAt(nodes[1]).pos;
		vec3d p2 = pmesh->const_nodeAt(nodes[2]).pos;

		vec3d n = vec3d::cross(p1 - p0, p2 - p0).normalized();
		vec3f cp = TheSceneGraph::Instance().camera().getPos();
		vec3d cpd = vec3d(cp.x, cp.y, cp.z);
		vec3d cd = (cpd - p0).normalized();

		if(vec3d::dot(cd, n) < 0) {
			n = n * -1.0;
			std::swap<vec3d>(p0, p2);
			std::swap<U32>(nodes[0], nodes[2]);
		}

		//store nodes
		for(int j=0; j < 3; j++)
			vIndices[i * 3 + j] = nodes[j];
		vFaceNormals[i] = n;

		//compute nodal normals
		for(int j=0; j < 3; j++) {
			U32 nj = nodes[j];
			if(vNodalCount[ nj ] == 0)
				vNodalNormals[ nj ] = n;
			else
				vNodalNormals[ nj ] = vNodalNormals[ nj ] + n;

			vNodalCount[ nj ] ++;
		}
	}

	//normals
	for(U32 i=0; i < pmesh->countNodes(); i++)
		vNodalNormals[i].normalize();

	//flat vec3
	FlattenVec3(vNodalNormals, vNormals);


	//setup mesh
	setupVertexAttribsT<double>(GL_DOUBLE, vNodes, 3, gbtPosition);
	setupVertexAttribsT<double>(GL_DOUBLE, vNormals, 3, gbtNormal);
	setupPerVertexColorT<float>(GL_FLOAT, pmesh->getColor(), pmesh->countNodes(), 3);
	setupFaceIndexBufferT<U32>(GL_UNSIGNED_INT, vIndices, ftTriangles);

	return true;
}

void VolMeshRender::draw() {
	glDisable(GL_CULL_FACE);

	SGMesh::draw();

	glEnable(GL_CULL_FACE);
}

} /* namespace MESH */
} /* namespace PS */
