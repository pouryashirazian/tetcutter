#include "sgmesh.h"


using namespace ps::scene;


SGMesh::SGMesh(const Geometry& g):SGNode() {
    setup(g);
}

SGMesh::SGMesh(const AnsiStr& strFilePath):SGNode() {
	Geometry g;
	g.read(strFilePath);
    setup(g);
}

SGMesh::~SGMesh() {
    m_meshbuffer.clearAllBuffers();
}

void SGMesh::setup(const Geometry& g) {

    m_meshbuffer.clearAllBuffers();

    if(g.countVertices() > 0)
       m_meshbuffer.setupVertexAttribs(g.vertices(), g.getVertexStep(), gbtPosition);
    if(g.countColor() > 0)
        m_meshbuffer.setupVertexAttribs(g.colors(), g.getColorStep(), gbtColor);
    if(g.countTexCoords() > 0)
        m_meshbuffer.setupVertexAttribs(g.texcoords(), g.getTexCoordStep(), gbtTexCoord);
    if(g.countNormals() > 0)
        m_meshbuffer.setupVertexAttribs(g.normals(), 3, gbtNormal);

    if(g.countFaces() > 0)
        m_meshbuffer.setupFaceIndexBuffer(g.indices(), g.getFaceMode());
    else
        m_meshbuffer.setFaceMode(g.getFaceMode());

    this->setAABB(g.aabb());
}


void SGMesh::draw() {

	if(m_spTransform)
		m_spTransform->bind();
    if(m_spEffect)
        m_spEffect->bind();

    m_meshbuffer.draw();

    if(m_spEffect)
        m_spEffect->unbind();
	if(m_spTransform)
		m_spTransform->unbind();

}

void SGMesh::drawNoEffect() {
	if(m_spTransform)
		m_spTransform->bind();

    m_meshbuffer.draw();

	if(m_spTransform)
		m_spTransform->unbind();
}



