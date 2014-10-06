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
	void setupIndexBufferObject(const vector<U32>& arrIndex, GLFaceType faceMode = ftTriangles);

    //Wireframe
	bool getWireFrameMode() const {return m_bWireFrame;}
	virtual void setWireFrameMode(bool bSet) { m_bWireFrame = bSet;}

	//face mode
	int getFaceMode() const {return m_faceMode;}
	void setFaceMode(int fmode);

	U32 countVertices() const {return m_ctVertices;}
	U32 countFaces() const {return m_ctFaceElements/m_gmbFaces.step();}


	//Validity
	bool isBufferValid(GLBufferType attribType = gbtPosition) const;
	const GLMemoryBuffer* buffer(GLBufferType attribType) const;


	//update
	bool modifyVertexBuffer(U32 offset, U32 szTotal, const void* lpData);
	bool readbackFaceBuffer(U32& count, vector<U32>& elements) const;


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

}
}

#endif /* PS_MESHGLBUFFER_H_ */
