/*
 * PS_MeshGLBuffer.h
 *
 *  Created on: Sep 29, 2012
 *      Author: pourya
 */

#ifndef PS_MESHGLBUFFER_H_
#define PS_MESHGLBUFFER_H_

#include <vector>

#include "GLFuncs.h"
#include "GLMemBuffer.h"
#include "Geometry.h"
#include "SGNode.h"
#include "base/Color.h"

using namespace std;
using namespace PS;
using namespace PS::MATH;
using namespace PS::SG;

namespace PS {
namespace GL {


/*!
 * Synopsis: GLMeshBuffer Simplifies drawing geometries using GPU Buffers.
 * Types of buffer objects are: Vertex, Color, TexCoord, Normal and Index
 */
class GLMeshBuffer : public SGNode {
public:
	GLMeshBuffer();
	GLMeshBuffer(const Geometry& g);
	virtual ~GLMeshBuffer();

	//Setup Buffer Objects
	virtual void setup(const Geometry& g);
	void setupVertexAttribs(const vector<float>& arrAttribs, int step = 3, GLBufferType attribKind = gbtPosition);
	void setupPerVertexColor(const vec4f& color, U32 ctVertices, int step = 4);
	void setupFaceIndexBuffer(const vector<U32>& arrIndex, GLFaceType faceMode = ftTriangles);

	//template funcs for GLBuffer
	template <typename T>
	void setupVertexAttribsT(int glType,
							 const vector<T>& arrAttribs,
							 int step,
							 GLBufferType attribKind,
							 GLBufferUsage usage = gbuStaticDraw);

	template <typename T>
	void setupPerVertexColorT(int glType, const Color& color, U32 ctVertices, int step = 4);

	template <typename T>
	void setupFaceIndexBufferT(int glType, const vector<T>& arrIndex, GLFaceType faceMode = ftTriangles);

	template <typename T>
	bool readbackFaceBuffer(int glType, U32& count, vector<T>& indices) const;

    //Wireframe
	bool getWireFrameMode() const {return m_bWireFrame;}
	virtual void setWireFrameMode(bool bSet) { m_bWireFrame = bSet;}

	//face mode
	int getFaceMode() const {return m_faceMode;}
	void setFaceMode(GLFaceType faceMode);

	U32 countVertices() const {return m_ctVertices;}
	U32 countFaces() const {return m_ctFaceElements/m_gmbFaces.step();}


	//Validity
	bool isBufferValid(GLBufferType attribType = gbtPosition) const;
	const GLMemoryBuffer* const_buffer(GLBufferType attribType) const;
	GLMemoryBuffer* buffer(GLBufferType attribType);


	//update
	bool modifyVertexBuffer(U32 offset, U32 szTotal, const void* lpData);

	//clears all buffers
	void clearAllBuffers() { GLMeshBuffer::cleanup();}

	//Draw
	virtual void draw();

	//Mesh buffer to draw normals
	static int FaceStepSizeFromMode(GLFaceType face);
    static GLMeshBuffer* PrepareMeshBufferForDrawingNormals(float len, U32 ctVertices, U32 fstep,
                                                             const vector<float>& arrVertices,
                                                             const vector<float>& arrNormals);

    //Convert from Homogenous to XYZ and reverse
    static U32 ConvertFloat4ToFloat3(const vector<float>& arrInFloat4, vector<float>& arrOutFloat3);
    static U32 ConvertFloat3ToFloat4(const vector<float>& arrInFloat3, vector<float>& arrOutFloat4);


protected:
	//Releases all buffer objects for rendering
	virtual void cleanup();
	void init();


protected:

	GLMemoryBuffer m_gmbColor;
	GLMemoryBuffer m_gmbNormal;
	GLMemoryBuffer m_gmbVertex;
	GLMemoryBuffer m_gmbTexcoord;
	GLMemoryBuffer m_gmbFaces;
	vector<GLMemoryBuffer*> m_vBuffers;
	bool m_bWireFrame;
	int m_faceMode;

	//Count
	U32 m_ctFaceElements;
	U32 m_ctVertices;

};


//////////////////////////////////////////////////////////////////////////////////////////////////////////
template <typename T>
void GLMeshBuffer::setupVertexAttribsT(int glType, const vector<T>& arrAttribs,
									   int step,
									   GLBufferType attribKind,
									   GLBufferUsage usage) {
	U32 szTotal = arrAttribs.size() * sizeof(T);
	m_vBuffers[attribKind]->setup(attribKind, step, glType, szTotal, &arrAttribs[0]);
	if(attribKind == gbtPosition)
		m_ctVertices = arrAttribs.size() / step;
}

template <typename T>
void GLMeshBuffer::setupPerVertexColorT(int glType, const Color& color, U32 ctVertices, int step) {

	vec4f c4 = color.toVec4f();
	vector<T> arrColors;
	arrColors.resize(ctVertices * step);
	for(U32 i=0; i<ctVertices; i++)
	{
		for(int j=0; j<step; j++)
			arrColors[i*step + j] = c4[j];
	}

	m_gmbColor.cleanup();
	m_gmbColor.setup(gbtColor, step, glType, arrColors.size() * sizeof(T), &arrColors[0]);
}

template <typename T>
void GLMeshBuffer::setupFaceIndexBufferT(int glType, const vector<T>& arrIndex, GLFaceType faceMode) {

	int step = FaceStepSizeFromMode(faceMode);
	m_faceMode = faceMode;
	m_ctFaceElements = arrIndex.size();
	m_gmbFaces.setup(gbtFaceIndex, step, glType, arrIndex.size() * sizeof(T), &arrIndex[0]);
}

template <typename T>
bool GLMeshBuffer::readbackFaceBuffer(int glType, U32& count, vector<T>& indices) const {
	if(glType != m_gmbFaces.type())
		return false;

	count = m_gmbFaces.size() / sizeof(T);
	indices.resize(count);
	return m_gmbFaces.readBack(indices.size() * sizeof(T), &indices[0]);
}

}
}

#endif /* PS_MESHGLBUFFER_H_ */
