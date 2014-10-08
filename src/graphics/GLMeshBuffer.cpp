/*
 * PS_MeshGLBuffer.cpp
 *
 *  Created on: Sep 29, 2012
 *      Author: pourya
 */
#include "GLMeshBuffer.h"
#include "selectgl.h"
#include "base/DebugUtils.h"

namespace PS {
namespace GL {

GLMeshBuffer::GLMeshBuffer():SGNode()
{
	init();
}

GLMeshBuffer::GLMeshBuffer(const Geometry& g) {
	init();
	this->setup(g);
}

GLMeshBuffer::~GLMeshBuffer()
{
	cleanup();
}

void GLMeshBuffer::init() {
	m_ctFaceElements = m_ctVertices = 0;
	m_faceMode = GL_TRIANGLES;
	m_bWireFrame = false;

	//gbtPosition, gbtNormal, gbtColor, gbtTexCoord, gbtFaceIndex
	m_vBuffers.resize(gbtCount);
	m_vBuffers[gbtPosition] = &m_gmbVertex;
	m_vBuffers[gbtNormal] = &m_gmbNormal;
	m_vBuffers[gbtColor] = &m_gmbColor;
	m_vBuffers[gbtTexCoord] = &m_gmbTexcoord;
	m_vBuffers[gbtFaceIndex] = &m_gmbFaces;
}

void GLMeshBuffer::cleanup()
{
	m_gmbColor.cleanup();
	m_gmbNormal.cleanup();
	m_gmbTexcoord.cleanup();
	m_gmbVertex.cleanup();
	m_gmbFaces.cleanup();
}

void GLMeshBuffer::setup(const Geometry& g) {
	GLMeshBuffer::cleanup();
	if(g.countVertices() > 0)
		this->setupVertexAttribs(g.vertices(), g.getVertexStep(), gbtPosition);
	if(g.countColor() > 0)
		this->setupVertexAttribs(g.colors(), g.getColorStep(), gbtColor);
	if(g.countTexCoords() > 0)
		this->setupVertexAttribs(g.texcoords(), g.getTexCoordStep(), gbtTexCoord);
	if(g.countNormals() > 0)
		this->setupVertexAttribs(g.normals(), 3, gbtNormal);

	if(g.countFaces() > 0)
		this->setupFaceIndexBuffer(g.indices(), g.getFaceMode());
	else
		this->setFaceMode(g.getFaceMode());
}

void GLMeshBuffer::setFaceMode(int fmode) {
	m_faceMode = fmode;
	m_ctFaceElements = m_ctVertices;
}

void GLMeshBuffer::setupVertexAttribs(const vector<float>& arrAttribs, int step, GLBufferType attribKind)
{
	setupVertexAttribsT<float>(GL_FLOAT, arrAttribs, step, attribKind);
}

void GLMeshBuffer::setupPerVertexColor(const vec4f& color, U32 ctVertices, int step)
{
	Color cl(color);
	setupPerVertexColorT<float>(GL_FLOAT, cl, ctVertices, step);
}

void GLMeshBuffer::setupFaceIndexBuffer(const vector<U32>& arrIndex, GLFaceType faceMode)
{
	setupFaceIndexBufferT(GL_UNSIGNED_INT, arrIndex, faceMode);
}

bool GLMeshBuffer::modifyVertexBuffer(U32 offset, U32 szTotal, const void* lpData) {
	return m_gmbVertex.modify(offset, szTotal, lpData);
}


bool GLMeshBuffer::isBufferValid(GLBufferType attribType) const {
	return m_vBuffers[attribType]->isValid();
}

const GLMemoryBuffer* GLMeshBuffer::const_buffer(GLBufferType attribType) const {
	return m_vBuffers[attribType];
}

GLMemoryBuffer* GLMeshBuffer::buffer(GLBufferType attribType) {
	return m_vBuffers[attribType];
}

void GLMeshBuffer::draw()
{
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	if(m_bWireFrame)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	//Color
	m_gmbColor.attach();

	//TexCoord
	m_gmbTexcoord.attach();

	//Normal
	m_gmbNormal.attach();

	//Vertex
	m_gmbVertex.attach();

	//Draw Faces
	if(m_gmbFaces.isValid())
	{
		m_gmbFaces.attach();
		m_gmbFaces.drawElements(m_faceMode, (GLsizei)m_ctFaceElements);
		m_gmbFaces.detach();
	}
	else
		glDrawArrays(m_faceMode, 0, m_ctFaceElements);

	m_gmbColor.detach();
	m_gmbTexcoord.detach();
	m_gmbNormal.detach();
	m_gmbVertex.detach();

	glPopAttrib();
}

int GLMeshBuffer::FaceStepSizeFromMode(GLFaceType face) {
	if(face == ftPoints)
		return 1;
	else if(face == ftLines)
		return 2;
	else if(face == ftTriangles)
		return 3;
	else if(face == ftQuads)
		return 4;
	else
		return 3;
}

GLMeshBuffer* GLMeshBuffer::PrepareMeshBufferForDrawingNormals(float len, U32 ctVertices, U32 fstep,
                                                               const vector<float>& arrVertices,
                                                               const vector<float>& arrNormals) {
    if(arrVertices.size() == 0 || arrNormals.size() == 0)
        return NULL;

    vector<float> allvertices;
    allvertices.resize(ctVertices* 3 * 2);
    for(U32 i=0; i < ctVertices; i++) {
        vec3f ptStart = vec3f(&arrVertices[i * fstep]);
        vec3f ptEnd = ptStart + vec3f(&arrNormals[i*3]) * len;
        allvertices[i*6] = ptStart.x;
        allvertices[i*6 + 1] = ptStart.y;
        allvertices[i*6 + 2] = ptStart.z;
        allvertices[i*6 + 3] = ptEnd.x;
        allvertices[i*6 + 4] = ptEnd.y;
        allvertices[i*6 + 5] = ptEnd.z;
    }

    //Create scene node
    GLMeshBuffer* lpDrawNormal = new GLMeshBuffer();
    lpDrawNormal->setupPerVertexColor(vec4f(0,0,1,1), ctVertices*2, 4);
    lpDrawNormal->setupVertexAttribs(allvertices, 3, gbtPosition);
    lpDrawNormal->setFaceMode(ftLines);

    return lpDrawNormal;
}

U32 GLMeshBuffer::ConvertFloat4ToFloat3(const vector<float>& arrInFloat4, vector<float>& arrOutFloat3) {
	if(arrInFloat4.size() == 0)
		return 0;

	U32 ctAttribs = (U32)arrInFloat4.size() / 4;
	arrOutFloat3.resize(ctAttribs * 3);
	for(U32 i=0; i < ctAttribs; i++) {
		arrOutFloat3[i * 3] = arrInFloat4[i * 4];
		arrOutFloat3[i * 3 + 1] = arrInFloat4[i * 4 + 1];
		arrOutFloat3[i * 3 + 2] = arrInFloat4[i * 4 + 2];
	}

	return ctAttribs;
}

U32 GLMeshBuffer::ConvertFloat3ToFloat4(const vector<float>& arrInFloat3, vector<float>& arrOutFloat4) {
	if(arrInFloat3.size() == 0)
		return 0;

	U32 ctAttribs = (U32)arrInFloat3.size() / 3;
	arrOutFloat4.resize(ctAttribs * 4);
	for(U32 i=0; i < ctAttribs; i++) {
		arrOutFloat4[i * 4] = arrInFloat3[i * 3];
		arrOutFloat4[i * 4 + 1] = arrInFloat3[i * 3 + 1];
		arrOutFloat4[i * 4 + 2] = arrInFloat3[i * 3 + 2];
		arrOutFloat4[i * 4 + 3] = 1.0f;
	}

	return ctAttribs;
}


}
}

