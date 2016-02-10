#include "glmeshbuffer.h"
#include "glselect.h"
//#include "base/DebugUtils.h"

using namespace ps::opengl;

GLMesh::GLMesh()
{
	init();
}

GLMesh::~GLMesh()
{
	cleanup();
}

void GLMesh::init() {
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

void GLMesh::cleanup()
{
	m_gmbColor.cleanup();
	m_gmbNormal.cleanup();
	m_gmbTexcoord.cleanup();
	m_gmbVertex.cleanup();
	m_gmbFaces.cleanup();
}

void GLMesh::setFaceMode(GLFaceType faceMode) {
	m_faceMode = faceMode;	
}

void GLMesh::setFaceElementsCount(U32 ctFaceElems) {
    m_ctFaceElements = ctFaceElems;
}

void GLMesh::setupVertexAttribs(const vector<float>& arrAttribs, int step, GLBufferType attribKind)
{
	setupVertexAttribsT<float>(GL_FLOAT, arrAttribs, step, attribKind);
}

void GLMesh::setupPerVertexColor(const vec4f& color, U32 ctVertices, int step)
{
	Color cl(color);
	setupPerVertexColorT<float>(GL_FLOAT, cl, ctVertices, step);
}

void GLMesh::setupFaceIndices(U32 ctFaceElements, GLFaceType faceMode) {
    m_ctFaceElements = ctFaceElements;
    m_faceMode = faceMode;
}

void GLMesh::setupFaceIndexBuffer(const vector<U32>& arrIndex, GLFaceType faceMode)
{
	setupFaceIndexBufferT(GL_UNSIGNED_INT, arrIndex, faceMode);
}

bool GLMesh::modifyVertexBuffer(U32 offset, U32 szTotal, const void* lpData) {
	return m_gmbVertex.modify(offset, szTotal, lpData);
}


bool GLMesh::isBufferValid(GLBufferType attribType) const {
	return m_vBuffers[attribType]->isValid();
}

const GLBuffer* GLMesh::const_buffer(GLBufferType attribType) const {
	return m_vBuffers[attribType];
}

GLBuffer* GLMesh::buffer(GLBufferType attribType) {
	return m_vBuffers[attribType];
}

void GLMesh::draw()
{
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	if(m_bWireFrame)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	//Color
	m_gmbColor.bind();

	//TexCoord
	m_gmbTexcoord.bind();

	//Normal
	m_gmbNormal.bind();

	//Vertex
	m_gmbVertex.bind();

	//Draw Faces
	if(m_gmbFaces.isValid())
	{
		m_gmbFaces.bind();
		m_gmbFaces.drawElements(m_faceMode, (GLsizei)m_ctFaceElements);
		m_gmbFaces.unbind();
	}
	else
		glDrawArrays(m_faceMode, 0, m_ctFaceElements);

	m_gmbColor.unbind();
	m_gmbTexcoord.unbind();
	m_gmbNormal.unbind();
	m_gmbVertex.unbind();

	glPopAttrib();
}

int GLMesh::FaceStepSizeFromMode(GLFaceType face) {
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

GLMesh* GLMesh::PrepareMeshBufferForDrawingNormals(float len, U32 ctVertices, U32 fstep,
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
    GLMesh* lpDrawNormal = new GLMesh();
    lpDrawNormal->setupPerVertexColor(vec4f(0,0,1,1), ctVertices*2, 4);
    lpDrawNormal->setupVertexAttribs(allvertices, 3, gbtPosition);
    lpDrawNormal->setFaceMode(ftLines);

    return lpDrawNormal;
}

U32 GLMesh::ConvertFloat4ToFloat3(const vector<float>& arrInFloat4, vector<float>& arrOutFloat3) {
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

U32 GLMesh::ConvertFloat3ToFloat4(const vector<float>& arrInFloat3, vector<float>& arrOutFloat4) {
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


