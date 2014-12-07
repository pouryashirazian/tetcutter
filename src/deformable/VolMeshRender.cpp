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

//The Effect for VolMesh
class VolMeshEffect: public SGEffect {
public:
	VolMeshEffect(GLShader* s) :
			SGEffect(s) {
		m_idCamPos = m_lpShader->getUniformLocation("camPos");
	}

	void setCamPos(const vec3f& camPos) {
		m_camPos = camPos;
		if (m_lpShader->isRunning())
			m_lpShader->setUniform(m_idCamPos, m_camPos);
	}

	void bind() {
		SGEffect::bind();
		m_lpShader->setUniform(m_idCamPos, m_camPos);
	}

private:
	vec3f m_camPos;
	int m_idCamPos;

};

///////////////////////////////////////////////////////////////////////
VolMeshRender::VolMeshRender() {
	init();
}

VolMeshRender::VolMeshRender(const VolMesh* pmesh) {
	init();
	sync(pmesh);
}


VolMeshRender::~VolMeshRender() {
	// TODO Auto-generated destructor stub
	SGMesh::cleanup();
}

void VolMeshRender::init() {
	resetTransform();
	if(TheShaderManager::Instance().has("volmeshphong")) {
        m_spEffect = SmartPtrSGEffect(new VolMeshEffect(TheShaderManager::Instance().get("volmeshphong")));
    }
}

bool VolMeshRender::sync(const VolMesh* pmesh) {
	if(pmesh == NULL)
		return false;

	//cleanup the mesh
	cleanup();

	//nodes
	vector<double> vFlatNodes;
	vector<double> vFlatNodeNormals;
	vFlatNodes.resize(pmesh->countNodes() * 3);
	vFlatNodeNormals.resize(pmesh->countNodes() * 3);

	//render for high performance
	for(U32 i=0; i < pmesh->countNodes(); i++) {
		const NODE& n = pmesh->const_nodeAt(i);

		n.pos.store(&vFlatNodes[i * 3]);
	}

	//per node normals
	vector<vec3d> vNodeNormals;
	vNodeNormals.resize(pmesh->countNodes());

	//per node normal update count
	vector<U32> vNodalCount;
	vNodalCount.resize(pmesh->countNodes());
	std::fill(vNodalCount.begin(), vNodalCount.end(), 0);

	//copy all face indices
	vector<U32> vIndices;
	vIndices.resize(pmesh->countFaces() * 3);

	//get camera position
	vec3f cp = TheSceneGraph::Instance().camera().getPos();
	vec3d cpd = vec3d(cp.x, cp.y, cp.z);

	//compute face normals using surface triangles
	for (U32 idxFace = 0; idxFace < pmesh->countFaces(); idxFace++) {
		U32 nodes[3];
		pmesh->getFaceNodes(idxFace, nodes);

		vec3d p0 = pmesh->const_nodeAt(nodes[0]).pos;
		vec3d p1 = pmesh->const_nodeAt(nodes[1]).pos;
		vec3d p2 = pmesh->const_nodeAt(nodes[2]).pos;

		vec3d n = vec3d::cross(p1 - p0, p2 - p0).normalized();
		vec3d cd = (cpd - p0).normalized();
		if (vec3d::dot(cd, n) < 0)
		{
			n = n * -1.0;
		}


		//store nodes
		for (int j = 0; j < 3; j++)
			vIndices[idxFace * 3 + j] = nodes[j];


		//if this is a surface triangle
		U32 ctCells = pmesh->countIncidentCells(idxFace);
		if(ctCells == 1)
		{
			for (int j = 0; j < 3; j++) {
				U32 nj = nodes[j];
				if (vNodalCount[nj] == 0)
					vNodeNormals[nj] = n;
				else
					vNodeNormals[nj] = vNodeNormals[nj] + n;

				vNodalCount[nj]++;
			}
		}
	}

	//normals
	for (U32 i = 0; i < pmesh->countNodes(); i++)
		vNodeNormals[i].normalize();

	//flat vec3
	FlattenVec3(vNodeNormals, vFlatNodeNormals);


	//setup surface mesh
	setupVertexAttribsT<double>(GL_DOUBLE, vFlatNodes, 3, gbtPosition);
	setupVertexAttribsT<double>(GL_DOUBLE, vFlatNodeNormals, 3, gbtNormal);
	setupPerVertexColorT<float>(GL_FLOAT, pmesh->getColor(), pmesh->countNodes(), 3);
	setupFaceIndexBufferT<U32>(GL_UNSIGNED_INT, vIndices, ftTriangles);

	//setup wireframe
	m_sgWireFrame.clearAllBuffers();
	m_sgWireFrame.setupVertexAttribsT<double>(GL_DOUBLE, vFlatNodes, 3, gbtPosition);
	m_sgWireFrame.setupPerVertexColorT<float>(GL_FLOAT, Color(0, 0, 0, 100), pmesh->countNodes(), 4);
	m_sgWireFrame.setupFaceIndexBufferT<U32>(GL_UNSIGNED_INT, vIndices, ftTriangles);
	m_sgWireFrame.setWireFrameMode(true);

	//setup nodes
	m_sgVertices.clearAllBuffers();
	m_sgVertices.setupVertexAttribsT<double>(GL_DOUBLE, vFlatNodes, 3, gbtPosition);
	m_sgVertices.setupPerVertexColorT<float>(GL_FLOAT, Color::red(), pmesh->countNodes(), 3);
	m_sgVertices.setFaceMode(GLFaceType::ftPoints);

	//setup normals
	/*
	{
		vector<vec3d> vNormalSegments;
		vNormalSegments.resize(pmesh->countNodes() * 2);
		for(U32 i=0; i < pmesh->countNodes(); i++) {
			vec3d p = pmesh->const_nodeAt(i).pos;
			vec3d n = vNodeNormals[i];

			vNormalSegments[i * 2] = p;
			vNormalSegments[i * 2 + 1] = p + n * 0.2;
		}

		vector<double> vFlattenedNormalSegments;
		FlattenVec3(vNormalSegments, vFlattenedNormalSegments);

		m_sgNormals.clearAllBuffers();
		m_sgNormals.setupVertexAttribsT<double>(GL_DOUBLE, vFlattenedNormalSegments, 3, gbtPosition);
		m_sgNormals.setupPerVertexColorT<float>(GL_FLOAT, Color::blue(), pmesh->countNodes() * 2, 3);
		m_sgNormals.setFaceMode(GLFaceType::ftLines);
	}
	*/

	return true;
}

void VolMeshRender::draw() {
	glDisable(GL_CULL_FACE);

	//draw surface
    m_spTransform->bind();
    VolMeshEffect* peff = dynamic_cast<VolMeshEffect*>(m_spEffect.get());
    peff->bind();
    peff->setCamPos(TheSceneGraph::Instance().camera().getPos());

    //draw surface
    SGMesh::drawNoEffect();

	//draw wireframe
	m_sgWireFrame.drawNoEffect();

	peff->unbind();


	//draw points
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	glPointSize(3.0f);
	glDisable(GL_LIGHTING);
		m_sgVertices.drawNoEffect();
		//m_sgNormals.drawNoEffect();
	glEnable(GL_LIGHTING);
	glPopAttrib();
	glEnable(GL_CULL_FACE);
}

} /* namespace MESH */
} /* namespace PS */
