#ifndef GLMESHGLBUFFER_H_
#define GLMESHGLBUFFER_H_

#include <vector>

#include "glfuncs.h"
#include "glbuffer.h"
#include "base/color.h"

using namespace std;
using namespace ps;
using namespace ps::base;

namespace ps {
namespace opengl {


/*!
 * Synopsis: GLMeshBuffer Simplifies drawing geometries using GPU Buffers.
 * Types of buffer objects are: Vertex, Color, TexCoord, Normal and Index
 */
class GLMesh {
public:
    GLMesh();
    virtual ~GLMesh();

	//Setup Buffer Objects
	void setupVertexAttribs(const vector<float>& arrAttribs, int step = 3, GLBufferType attribKind = gbtPosition);
	void setupPerVertexColor(const vec4f& color, U32 ctVertices, int step = 4);
    void setupFaceIndices(U32 ctFaceElements, GLFaceType faceMode = ftTriangles);
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
    bool wireframeMode() const {return m_bWireFrame;}
    virtual void setWireFrameMode(bool mode) { m_bWireFrame = mode;}

	//face mode
    int faceMode() const {return m_faceMode;}
	void setFaceMode(GLFaceType faceMode);
    void setFaceElementsCount(U32 ctFaceElems);

	U32 countVertices() const {return m_ctVertices;}
	U32 countFaces() const {return m_ctFaceElements/m_gmbFaces.step();}


	//Validity
	bool isBufferValid(GLBufferType attribType = gbtPosition) const;
	const GLBuffer* const_buffer(GLBufferType attribType) const;
	GLBuffer* buffer(GLBufferType attribType);


	//update
	bool modifyVertexBuffer(U32 offset, U32 szTotal, const void* lpData);

	//clears all buffers
    void clearAllBuffers() { GLMesh::cleanup();}

	//Draw
	virtual void draw();

	//Mesh buffer to draw normals
	static int FaceStepSizeFromMode(GLFaceType face);
    static GLMesh* PrepareMeshBufferForDrawingNormals(float len, U32 ctVertices, U32 fstep,
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

	GLBuffer m_gmbColor;
	GLBuffer m_gmbNormal;
	GLBuffer m_gmbVertex;
	GLBuffer m_gmbTexcoord;
	GLBuffer m_gmbFaces;
	vector<GLBuffer*> m_vBuffers;
	bool m_bWireFrame;
	int m_faceMode;

	//Count
	U32 m_ctFaceElements;
	U32 m_ctVertices;

};


//////////////////////////////////////////////////////////////////////////////////////////////////////////
template <typename T>
void GLMesh::setupVertexAttribsT(int glType, const vector<T>& arrAttribs,
									   int step,
									   GLBufferType attribKind,
									   GLBufferUsage usage) {
	U32 szTotal = arrAttribs.size() * sizeof(T);
	m_vBuffers[attribKind]->setup(attribKind, step, glType, szTotal, &arrAttribs[0]);
	if(attribKind == gbtPosition)
		m_ctVertices = arrAttribs.size() / step;
}

template <typename T>
void GLMesh::setupPerVertexColorT(int glType, const Color& color, U32 ctVertices, int step) {

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
void GLMesh::setupFaceIndexBufferT(int glType, const vector<T>& arrIndex, GLFaceType faceMode) {

	int step = FaceStepSizeFromMode(faceMode);
	m_faceMode = faceMode;
	m_ctFaceElements = arrIndex.size();
	m_gmbFaces.setup(gbtFaceIndex, step, glType, arrIndex.size() * sizeof(T), &arrIndex[0]);
}

template <typename T>
bool GLMesh::readbackFaceBuffer(int glType, U32& count, vector<T>& indices) const {
	if(glType != m_gmbFaces.type())
		return false;

	count = m_gmbFaces.size() / sizeof(T);
	indices.resize(count);
	return m_gmbFaces.readBack(indices.size() * sizeof(T), &indices[0]);
}

}
}

#endif /* PS_MESHGLBUFFER_H_ */
