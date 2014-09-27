/*
 * PS_MeshGLBuffer.cpp
 *
 *  Created on: Sep 29, 2012
 *      Author: pourya
 */
#include <cstring>
#include "GLMeshBuffer.h"
#include "selectgl.h"
#include "base/DebugUtils.h"

namespace PS {
namespace GL {

GLMemoryBuffer::GLMemoryBuffer() {
	m_isValid = false;
	m_usage = GL_DYNAMIC_DRAW;
	m_step = 0;
	m_dataType = GL_FLOAT;
	m_szBuffer = 0;
	m_handle = INVALID_GLBUFFER;
	m_bufferType = mbtPosition;
}


GLMemoryBuffer::GLMemoryBuffer(MemoryBufferType type, int usage, int step,
		   	   	   	   	   	   	   int datatype, U32 szTotal, const void* lpData) {
	m_isValid  = false;
	m_usage    = GL_DYNAMIC_DRAW;
	m_step 	   = 0;
	m_dataType = GL_FLOAT;
	m_szBuffer = 0;
	m_handle   = INVALID_GLBUFFER;
	m_bufferType = mbtPosition;

	this->setup(type, usage, step, datatype, szTotal, lpData);
}

GLMemoryBuffer::~GLMemoryBuffer() {
	cleanup();
}

void GLMemoryBuffer::cleanup() {
	if(m_isValid)
		glDeleteBuffers(1, &m_handle);
	m_handle = 0;
	m_isValid = false;
}

bool GLMemoryBuffer::setup(MemoryBufferType type, int usage, int step,
							   int datatype, U32 szTotal, const void* lpData) {
	m_bufferType = type;
	m_usage = usage;
	m_step = step;
	m_dataType = datatype;
	m_szBuffer = szTotal;


	glGenBuffers(1, &m_handle);
	glBindBuffer(GL_ARRAY_BUFFER, m_handle);
	glBufferData(GL_ARRAY_BUFFER, m_szBuffer, lpData, m_usage);
	m_isValid = true;
	return true;
}

void GLMemoryBuffer::resize(U32 szTotal, const void* lpData) {
	if(!m_isValid)
		return;

	m_szBuffer = szTotal;
	glBindBuffer(GL_ARRAY_BUFFER, m_handle);
	glBufferData(GL_ARRAY_BUFFER, m_szBuffer, lpData, m_usage);
}

bool GLMemoryBuffer::readBackBuffer(U32 szOutBuffer, void* lpOutBuffer) {
	if(!m_isValid || szOutBuffer < m_szBuffer)
		return false;

	glBindBuffer( GL_ARRAY_BUFFER, m_handle);
	float* lpMappedData = (float *) glMapBuffer( GL_ARRAY_BUFFER, GL_READ_ONLY );
	if(lpMappedData == NULL) {
		return false;
	}

	memcpy(lpOutBuffer, lpMappedData, m_szBuffer);
	glUnmapBuffer( GL_ARRAY_BUFFER );

	return true;
}

void GLMeshBuffer::setFaceMode(int fmode) {
	m_faceMode = fmode;
	m_ctFaceElements = m_ctVertices;
}

void GLMemoryBuffer::attach() {
	if(!m_isValid)
		return;

	//Bind Buffer
	if(m_bufferType == mbtFaceIndices)
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_handle);
	else
		glBindBuffer(GL_ARRAY_BUFFER, m_handle);

	switch(m_bufferType) {
	case(mbtColor):
		glColorPointer(m_step, m_dataType, 0, 0);
		glEnableClientState(GL_COLOR_ARRAY);
		break;
	case(mbtTexCoord):
		glTexCoordPointer(m_step, m_dataType, 0, 0);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		break;
	case(mbtNormal):
		glNormalPointer(m_dataType, 0, 0);
		glEnableClientState(GL_NORMAL_ARRAY);
		break;
	case(mbtPosition):
		glVertexPointer(m_step, m_dataType, 0, 0);
		glEnableClientState(GL_VERTEX_ARRAY);
		break;
	case(mbtFaceIndices):
		glEnableClientState(GL_ELEMENT_ARRAY_BUFFER);
		break;
    case(mbtCount):
        break;
	}

}

void GLMemoryBuffer::detach() {
	if(!m_isValid)
		return;

	switch(m_bufferType) {
	case(mbtColor):
		glDisableClientState(GL_COLOR_ARRAY);
		break;
	case(mbtTexCoord):
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		break;
	case(mbtNormal):
		glDisableClientState(GL_NORMAL_ARRAY);
		break;
	case(mbtPosition):
		glDisableClientState(GL_VERTEX_ARRAY);
		break;
	case(mbtFaceIndices):
		glDisableClientState(GL_ELEMENT_ARRAY_BUFFER);
		break;
    case(mbtCount):
        break;
	}
}

void GLMemoryBuffer::drawElements(int faceMode, int ctElements) {
	glDrawElements(faceMode, ctElements, m_dataType, (GLvoid*)0);
}

bool GLMemoryBuffer::modify(U32 offset, U32 szTotal, const void* lpData) {
	if(!m_isValid)
		return false;

	if(szTotal != m_szBuffer || lpData == NULL)
		return false;

	//Bind Buffer
	if(m_bufferType == mbtFaceIndices) {
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_handle);
		glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, offset, szTotal, lpData);
	}
	else {
		glBindBuffer(GL_ARRAY_BUFFER, m_handle);
		glBufferSubData(GL_ARRAY_BUFFER, offset, szTotal, lpData);
	}


	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////
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
	m_vboColor = m_vboNormal = m_vboTexCoord = m_vboVertex = m_iboFaces = INVALID_GLBUFFER;
	m_isValidColor = m_isValidIndex = m_isValidNormal = m_isValidTexCoord = m_isValidVertex = false;
	m_ctFaceElements = m_ctVertices = 0;
	m_faceMode = GL_TRIANGLES;
	m_bWireFrame = false;

	m_stepVertex = 3;
	m_stepColor = 3;
	m_stepTexCoord = 2;
	m_stepFace = 3;
}

void GLMeshBuffer::cleanup()
{
	if(m_isValidColor)
		glDeleteBuffers(1, &m_vboColor);
	if(m_isValidTexCoord)
		glDeleteBuffers(1, &m_vboTexCoord);
	if(m_isValidVertex)
		glDeleteBuffers(1, &m_vboVertex);
	if(m_isValidNormal)
		glDeleteBuffers(1, &m_vboNormal);
	if(m_isValidIndex)
		glDeleteBuffers(1, &m_iboFaces);
	m_isValidColor = m_isValidIndex = m_isValidNormal = m_isValidTexCoord = m_isValidVertex = false;
}

void GLMeshBuffer::setup(const Geometry& g) {
	GLMeshBuffer::cleanup();
	if(g.countVertices() > 0)
		this->setupVertexAttribs(g.vertices(), g.getVertexStep(), mbtPosition);
	if(g.countColor() > 0)
		this->setupVertexAttribs(g.colors(), g.getColorStep(), mbtColor);
	if(g.countTexCoords() > 0)
		this->setupVertexAttribs(g.texcoords(), g.getTexCoordStep(), mbtTexCoord);
	if(g.countNormals() > 0)
		this->setupVertexAttribs(g.normals(), 3, mbtNormal);

	if(g.countFaces() > 0)
		this->setupIndexBufferObject(g.indices(), g.getFaceMode());
	else
		this->setFaceMode(g.getFaceMode());
}

void GLMeshBuffer::setupVertexAttribs(const vector<float>& arrAttribs, int step, MemoryBufferType attribKind)
{
	/*
	glGenBuffers(1, &m_outputMesh.vboVertex);
	glBindBuffer(GL_ARRAY_BUFFER, m_outputMesh.vboVertex);
	glBufferData(GL_ARRAY_BUFFER, ctVertices*3 * sizeof(float), arrVertices, GL_DYNAMIC_DRAW);

	glGenBuffers(1, &m_outputMesh.vboColor);
	glBindBuffer(GL_ARRAY_BUFFER, m_outputMesh.vboColor);
	glBufferData(GL_ARRAY_BUFFER, ctVertices*3 * sizeof(float), arrColors, GL_DYNAMIC_DRAW);

	glGenBuffers(1, &m_outputMesh.vboNormal);
	glBindBuffer(GL_ARRAY_BUFFER, m_outputMesh.vboNormal);
	glBufferData(GL_ARRAY_BUFFER, ctVertices*3 * sizeof(float), arrNormals, GL_DYNAMIC_DRAW);


	glGenBuffers(1, &m_outputMesh.iboFaces);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_outputMesh.iboFaces);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, ctTriangles *3 * sizeof(U32), arrFaces, GL_DYNAMIC_DRAW);
	*/

	if(attribKind == mbtPosition)
	{
		m_isValidVertex = true;
		m_stepVertex = step;
		m_ctVertices = (U32)arrAttribs.size() / m_stepVertex;
		glGenBuffers(1, &m_vboVertex);
		glBindBuffer(GL_ARRAY_BUFFER, m_vboVertex);
		glBufferData(GL_ARRAY_BUFFER, arrAttribs.size() * sizeof(float), &arrAttribs[0], GL_STATIC_DRAW);
	}
	else if(attribKind == mbtNormal)
	{
		m_isValidNormal = true;
		vector<float> arrNormalizedNormals;
		arrNormalizedNormals.resize(arrAttribs.size());
		U32 ctNormals = (U32)arrAttribs.size() / 3;
		for(U32 i=0; i<ctNormals; i++)
		{
			vec3f n = vec3f(&arrAttribs[i * 3]);
			n.normalize();
			arrNormalizedNormals[i * 3 + 0] = n.x;
			arrNormalizedNormals[i * 3 + 1] = n.y;
			arrNormalizedNormals[i * 3 + 2] = n.z;
		}

		glGenBuffers(1, &m_vboNormal);
		glBindBuffer(GL_ARRAY_BUFFER, m_vboNormal);
		glBufferData(GL_ARRAY_BUFFER, arrAttribs.size() * sizeof(float), &arrAttribs[0], GL_STATIC_DRAW);
	}
	else if(attribKind == mbtColor)
	{
		m_isValidColor = true;
		m_stepColor = step;

		glGenBuffers(1, &m_vboColor);
		glBindBuffer(GL_ARRAY_BUFFER, m_vboColor);
		glBufferData(GL_ARRAY_BUFFER, arrAttribs.size() * sizeof(float), &arrAttribs[0], GL_STATIC_DRAW);
	}
	else if(attribKind == mbtTexCoord)
	{
		m_isValidTexCoord = true;
		m_stepTexCoord = step;

		glGenBuffers(1, &m_vboTexCoord);
		glBindBuffer(GL_ARRAY_BUFFER, m_vboTexCoord);
		glBufferData(GL_ARRAY_BUFFER, arrAttribs.size() * sizeof(float), &arrAttribs[0], GL_STATIC_DRAW);
	}
}

void GLMeshBuffer::setupPerVertexColor(const vec4f& color, U32 ctVertices, int step)
{
	if(m_isValidColor)
		glDeleteBuffers(1, &m_vboColor);

	m_stepColor = step;
	vector<float> arrColors;
	arrColors.resize(ctVertices * step);
	for(U32 i=0; i<ctVertices; i++)
	{
		for(int j=0; j<step; j++)
			arrColors[i*step + j] = color.e[j];
	}

	glGenBuffers(1, &m_vboColor);
	glBindBuffer(GL_ARRAY_BUFFER, m_vboColor);
	glBufferData(GL_ARRAY_BUFFER, arrColors.size() * sizeof(float), &arrColors[0], GL_STATIC_DRAW);
	m_isValidColor = true;
}

void GLMeshBuffer::setupIndexBufferObject(const vector<U32>& arrIndex, int faceMode /*GL_TRIANGLES */)
{
	m_faceMode = faceMode;
	m_isValidIndex = true;
	m_ctFaceElements = (U32)arrIndex.size();
	glGenBuffers(1, &m_iboFaces);
	glBindBuffer(GL_ARRAY_BUFFER, m_iboFaces);
	glBufferData(GL_ARRAY_BUFFER, arrIndex.size() * sizeof(U32), &arrIndex[0], GL_STATIC_DRAW);
}

bool GLMeshBuffer::updateVertexBuffer(U32 offset, U32 szTotal, const void* lpData) {
	if (!m_isValidVertex)
		return false;

	U32 szVertexBuffer = m_stepVertex * m_ctVertices * sizeof(float);
	if ((szTotal > szVertexBuffer) || lpData == NULL)
		return false;

	//Bind Buffer
	glBindBuffer(GL_ARRAY_BUFFER, m_vboVertex);
	glBufferSubData(GL_ARRAY_BUFFER, offset, szTotal, lpData);

	return true;
}

bool GLMeshBuffer::readbackMeshVertexAttribGL(MemoryBufferType attrib, U32& count, vector<float>& values) const {
	U32 vbo = 0;
	U32 szRead = 0;
	count = 0;
	if(attrib == mbtPosition) {
		vbo = m_vboVertex;
		szRead = m_ctVertices * m_stepVertex;
	}
	else if(attrib == mbtNormal) {
		vbo = m_vboNormal;
		szRead = m_ctVertices * 3;
	}
	else if(attrib == mbtColor) {
		vbo = m_vboColor;
		szRead = m_ctVertices * m_stepColor;
	}
	else if(attrib == mbtTexCoord) {
		vbo = m_vboTexCoord;
		szRead = m_ctVertices * m_stepTexCoord;
	}
	else
		return false;


	glBindBuffer( GL_ARRAY_BUFFER, vbo);
	float* lpBuffer = (float *) glMapBuffer( GL_ARRAY_BUFFER, GL_READ_ONLY );
	if(lpBuffer == NULL) {
		return false;
	}

	values.assign(lpBuffer, lpBuffer + szRead);
	glUnmapBuffer( GL_ARRAY_BUFFER );
	count = m_ctVertices;

	//PS::DEBUG::PrintArrayF(&values[0], 180);
	return true;
}

bool GLMeshBuffer::isVertexBufferValid(MemoryBufferType attribType) const {
	switch(attribType) {
	case(mbtPosition):
		return m_isValidVertex;
	case(mbtNormal):
		return m_isValidNormal;
	case(mbtColor):
		return m_isValidColor;
	case(mbtTexCoord):
		return m_isValidTexCoord;
	default:
		return m_isValidVertex;
	}
}

//Steps
U32 GLMeshBuffer::vertexAttribStep(MemoryBufferType attribType) const {
	switch(attribType) {
	case(mbtPosition):
		return m_stepVertex;
	case(mbtNormal):
		return 3;
	case(mbtColor):
		return m_stepColor;
	case(mbtTexCoord):
		return m_stepTexCoord;
	default:
		return m_stepVertex;
	}

}

//Buffer Objects
U32 GLMeshBuffer::vertexBufferObjectGL(MemoryBufferType attribType) const {
	switch(attribType) {
	case(mbtPosition):
		return m_vboVertex;
	case(mbtNormal):
		return m_vboNormal;
	case(mbtColor):
		return m_vboColor;
	case(mbtTexCoord):
		return m_vboTexCoord;
	default:
		return m_vboVertex;
	}
}


bool GLMeshBuffer::readbackMeshFaceGL(U32& count, vector<U32>& elements) const {

	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, m_iboFaces);
	U32* lpBuffer = (U32 *) glMapBuffer( GL_ELEMENT_ARRAY_BUFFER, GL_READ_ONLY );
	elements.assign(lpBuffer, lpBuffer + (m_ctFaceElements * m_stepFace));
	glUnmapBuffer( GL_ELEMENT_ARRAY_BUFFER );
	count = m_ctFaceElements;


	return true;
}


void GLMeshBuffer::draw()
{
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	if(m_bWireFrame)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	//Color
	if(m_isValidColor)
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_vboColor);
		glColorPointer(m_stepColor, GL_FLOAT, 0, 0);
		glEnableClientState(GL_COLOR_ARRAY);
	}

	//TexCoord
	if(m_isValidTexCoord)
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_vboTexCoord);
		glTexCoordPointer(m_stepTexCoord, GL_FLOAT, 0, 0);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	}

	//Normal
	if(m_isValidNormal)
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_vboNormal);
		glNormalPointer(GL_FLOAT, 0, 0);
		glEnableClientState(GL_NORMAL_ARRAY);
	}

	//Vertex
	if(m_isValidVertex)
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_vboVertex);
		glVertexPointer(m_stepVertex, GL_FLOAT, 0, 0);
		glEnableClientState(GL_VERTEX_ARRAY);
	}

	//Draw Faces
	if(m_isValidIndex)
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_iboFaces);
		glEnableClientState(GL_ELEMENT_ARRAY_BUFFER);
		glDrawElements(m_faceMode, (GLsizei)m_ctFaceElements, GL_UNSIGNED_INT, (GLvoid*)0);

		glDisableClientState(GL_ELEMENT_ARRAY_BUFFER);
	}
	else
		glDrawArrays(m_faceMode, 0, m_ctFaceElements);

	if(m_isValidColor)
		glDisableClientState(GL_COLOR_ARRAY);
	if(m_isValidTexCoord)
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	if(m_isValidNormal)
		glDisableClientState(GL_NORMAL_ARRAY);
	if(m_isValidVertex)
		glDisableClientState(GL_VERTEX_ARRAY);

	glPopAttrib();
}

/*
GLMeshBuffer* GLMeshBuffer::prepareMeshBufferNormals(float len) const {
	if (!m_isValidVertex || !m_isValidNormal)
		return NULL;

	U32 ctVertices;
	vector<float> vertices;
	vector<float> normals;
	readbackMeshVertexAttrib(vatPosition, ctVertices, vertices);
	readbackMeshVertexAttrib(vatNormal, ctVertices, normals);

	vector<float> allvertices;
	allvertices.resize(ctVertices * 3 * 2);
	for (U32 i = 0; i < ctVertices; i++) {
		vec3f ptStart = vec3f(&vertices[i * 3]);
		vec3f ptEnd = ptStart + vec3f(&normals[i * 3]) * len;
		allvertices[i * 6] = ptStart.x;
		allvertices[i * 6 + 1] = ptStart.y;
		allvertices[i * 6 + 2] = ptStart.z;
		allvertices[i * 6 + 3] = ptEnd.x;
		allvertices[i * 6 + 4] = ptEnd.y;
		allvertices[i * 6 + 5] = ptEnd.z;
	}

	//Create scene node
	GLMeshBuffer* lpDrawNormal = new GLMeshBuffer();
	lpDrawNormal->setupPerVertexColor(vec4f(0, 0, 1, 1), ctVertices * 2, 4);
	lpDrawNormal->setupVertexAttribs(allvertices, 3, vatPosition);
	lpDrawNormal->setFaceMode(ftLines);

	return lpDrawNormal;
}
*/

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
    lpDrawNormal->setupVertexAttribs(allvertices, 3, mbtPosition);
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

