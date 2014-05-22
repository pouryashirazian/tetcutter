/*
 * PS_MeshGLBuffer.h
 *
 *  Created on: Sep 29, 2012
 *      Author: pourya
 */

#ifndef PS_MESHGLBUFFER_H_
#define PS_MESHGLBUFFER_H_

#include <vector>
#include "base/Vec.h"
#include "GLFuncs.h"
#include "GLTypes.h"
#include "Geometry.h"
#include "SceneGraph.h"

using namespace std;
using namespace PS;
using namespace PS::MATH;
using namespace PS::SG;

namespace PS {
namespace GL {

/*!
 * OpenGL Memory Buffer
 */
class GLMemoryBuffer {
public:
	GLMemoryBuffer();
	GLMemoryBuffer(MemoryBufferType type, int usage, int step,
			   	     int datatype, U32 szTotal, const void* lpData);
	virtual ~GLMemoryBuffer();

	bool setup(MemoryBufferType type, int usage, int step,
			   int datatype, U32 szTotal, const void* lpData);

	//Resize buffer
	void resize(U32 szTotal, const void* lpData);

	void attach();
	void detach();
	void drawElements(int faceMode, int ctElements);

	/*!
	 * Modifies the buffer for changes
	 */
	bool modify(U32 offset, U32 szTotal, const void* lpData);


	bool readBackBuffer(U32 szOutBuffer, void* lpOutBuffer);

	//Read Attribs
	bool isValid() const { return m_isValid;}
	int step() const {return m_step;}
	U32 size() const {return m_szBuffer;}
	U32 handle() const {return m_handle;}
	MemoryBufferType type() const {return m_bufferType;}

protected:
	void cleanup();
private:
	bool m_isValid;
	int m_usage;
	int m_step;
	int m_dataType;
	U32 m_szBuffer;
	U32 m_handle;
	MemoryBufferType m_bufferType;
};


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
	void setupVertexAttribs(const vector<float>& arrAttribs, int step = 3, MemoryBufferType attribKind = mbtPosition);
	void setupPerVertexColor(const vec4f& color, U32 ctVertices, int step = 4);
	void setupIndexBufferObject(const vector<U32>& arrIndex, int faceMode = ftTriangles);

    //Wireframe
	bool getWireFrameMode() const {return m_bWireFrame;}
	virtual void setWireFrameMode(bool bSet) { m_bWireFrame = bSet;}

	int getFaceMode() const {return m_faceMode;}
	void setFaceMode(int fmode) {
		m_faceMode = fmode;
		m_ctFaceElements = m_ctVertices;
	}

	U32 countVertices() const {return m_ctVertices;}
	U32 countTriangles() const {return m_ctFaceElements/3;}
	U32 countFaceElements() const {return m_ctFaceElements;}

	//Validity
	bool isFaceBufferValid() const { return m_isValidIndex;}
	bool isVertexBufferValid(MemoryBufferType attribType = mbtPosition) const;

	//Steps
	U32 faceStep() const {return m_stepFace;}
	U32 vertexAttribStep(MemoryBufferType attribType = mbtPosition) const;

	//Buffer Objects
	U32 indexBufferObjectGL() const { return m_iboFaces;}
	U32 vertexBufferObjectGL(MemoryBufferType attribType = mbtPosition) const;


	//Reads Mesh
	bool readbackMeshVertexAttribGL(MemoryBufferType attrib, U32& count, vector<float>& values) const;
	bool readbackMeshFaceGL(U32& count, vector<U32>& elements) const;


	//Draw
	virtual void draw();

	//Mesh buffer to draw normals
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

	//Index to the buffer objects
	U32 m_vboVertex;
	U32 m_vboColor;
	U32 m_vboNormal;
	U32 m_vboTexCoord;
	U32 m_iboFaces;

	//Flags
	bool m_isValidVertex;
	bool m_isValidNormal;
	bool m_isValidColor;
	bool m_isValidTexCoord;
	bool m_isValidIndex;
	bool m_bWireFrame;

	//Count
	U32 m_ctFaceElements;
	U32 m_ctVertices;

	int m_stepVertex;
	int m_stepColor;
	int m_stepTexCoord;
	int m_stepFace;
	int m_faceMode;
};

}
}

#endif /* PS_MESHGLBUFFER_H_ */
