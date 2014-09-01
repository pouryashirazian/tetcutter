/*
 * SGMesh.cpp
 *
 *  Created on: Mar 5, 2014
 *      Author: pourya
 */

#include "SGMesh.h"
#include "selectgl.h"


namespace PS {
namespace SG {

SGMesh::SGMesh(const Geometry& g):GLMeshBuffer(g) {
	this->setAABB(g.aabb());
}

SGMesh::SGMesh(const AnsiStr& strFilePath):GLMeshBuffer() {
	Geometry g;
	g.read(strFilePath);
	this->setup(g);
	this->setAABB(g.aabb());
}

void SGMesh::draw() {

	if(m_spTransform)
		m_spTransform->bind();
    if(m_spEffect)
        m_spEffect->bind();

    GLMeshBuffer::draw();

    if(m_spEffect)
        m_spEffect->unbind();
	if(m_spTransform)
		m_spTransform->unbind();

}

void SGMesh::drawNoEffect() {
	if(m_spTransform)
		m_spTransform->bind();

	GLMeshBuffer::draw();

	if(m_spTransform)
		m_spTransform->unbind();
}

}
}


