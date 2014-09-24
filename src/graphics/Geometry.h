/*
 * Geometry.h
 *
 *  Created on: Dec 28, 2013
 *      Author: pourya
 */

#ifndef GEOMETRY_H_
#define GEOMETRY_H_

#include <vector>
#include "base/Vec.h"
#include "base/Matrix.h"
#include "base/Quaternion.h"
#include "base/String.h"
#include "graphics/AABB.h"
#include "GLTypes.h"

using namespace std;
using namespace PS;
using namespace PS::MATH;

namespace PS {
namespace GL {

class Geometry {
public:
	Geometry();
    Geometry(const Geometry& other);
	explicit Geometry(const AnsiStr& strFilePath);
	virtual ~Geometry();

	void init();
	void init(int stepVertex, int stepColor,
			   int stepTexCoords, PS::GL::FaceType faceMode = ftTriangles);

	//stats
	bool isCompatible(const Geometry& other) const;
	int countVertices() const;
	int countColor() const;
	int countTexCoords() const;
	int countNormals() const;
	int countFaces() const;

	int getVertexStep() const {return m_stepVertex;}
	int getColorStep() const {return m_stepColor;}
	int getTexCoordStep() const {return m_stepTexCoord;}
	int getFaceStep() const {return m_stepFace;}
	int getFaceMode() const {return m_faceMode;}

	//Access
	vec3f vertexAt(int index) const;
	vec4f colorAt(int index) const;
	vec2f texcoordAt(int index) const;
	vec3f normalAt(int index) const;
	vec3u32 triangleAt(int index) const;
	vec4u32 quadAt(int index) const;

	void setVertex(int index, const vec3f& v);

	const vector<float>& vertices() const {return m_vertices;}
	const vector<float>& colors() const {return m_colors;}
	const vector<float>& texcoords() const {return m_texCoords;}
	const vector<float>& normals() const {return m_normals;}
	const vector<U32>& indices() const {return m_indices;}

	vector<float>& vertices() {return m_vertices;}
	vector<float>& colors()  {return m_colors;}
	vector<float>& texcoords()  {return m_texCoords;}
	vector<float>& normals()  {return m_normals;}
	vector<U32>& indices()  {return m_indices;}


	//Add Attributes
	bool addVertex(const vec3f& v);
	bool addColor(const vec4f& c);
	bool addTexCoord(float t);
	bool addTexCoord(const vec2f& t);
	bool addTexCoord(const vec3f& t);
    void addNormal(const vec3f& n);
    
    
	bool addTriangle(const vec3u32& f);
	bool addQuad(const vec4u32& f);
	void extrude(const vec3f& v);
	void transform(const mat44f& m);

	//Add Arrays
	bool addVertexAttribs(const vector<float>& arrAttribs, int step = 3, MemoryBufferType attribKind = mbtPosition);
	void addPerVertexColor(const vec4f& color, U32 ctVertices = 0);
	bool addFaceIndices(const vector<U32>& arrIndex, PS::GL::FaceType faceMode = ftTriangles);
	bool computeNormalsFromFaces();

	//Clear Buffers
	void clearBuffer(MemoryBufferType btype);

    //Lines
    bool addLine(const vec3f& start, const vec3f& end);
    bool addLines(const vector<vec3f>& vertices);
    bool addCircle2D(int sectors,
                     float radius = 1.0f,
                     const vec3f& o = vec3f(0,0,0));
    
	//Create Objects
	bool  addCircle3D(int sectors,
                    float radius = 1.0f,
                    const vec3f& o = vec3f(0,0,0));

	bool addCone(int sectors,
				  float radius = 1.0f,
				  float width = 1.0f,
				  const vec3f& o = vec3f(0,0,0));

	//bool addCircularTape(int sectors, float radius = 1.0f,)

    void addCube(const vec3f& lower, const vec3f& upper);
    void addCube(const vec3f& center, float side);
    void addSphere(float radius = 1.0f, int hseg = 8, int vseg = 8);
    void addCylinder(float radius, float height, int sectors, bool base = true, bool roof = true);

    void addTetrahedra(vec3f v[4]);
    void addTetrahedra(const vector<float>& vertices, const vector<U32>& tets);

    
    /*!
     * Add a ring to the geometry
     */
	bool addRingStripAroundXAxis(int sectors,
								 float radius,
								 float thickness,
								 const vec3f& origin);

	/*!
     * Add a disc to the geometry
     */
    bool addDisc(int sectors,
                 int xsections,
                 float radius,
                 float thickness,
                 const vec3f& o = vec3f(0,0,0));
    


	bool checkIndices() const;

    //Bounding Box
    AABB aabb() const;

	//Operators
	bool copyFrom(const Geometry& other);
	bool appendFrom(const Geometry& other);

	//IO
	bool read(const AnsiStr& strFilePath);
	bool write(const AnsiStr& strFilePath) const;
	bool readObj(const AnsiStr& strFilePath);
	bool writeObj(const AnsiStr& strFilePath) const;

	Geometry& operator=(const Geometry& other);
	Geometry operator+(const Geometry& other) const;

	//friend Geometry operator+(const Geometry& a, const Geometry& b);
protected:
	void cleanup();

private:
	int m_stepVertex;
	int m_stepColor;
	int m_stepTexCoord;
	int m_stepFace;
	FaceType m_faceMode;


	vector<float> m_vertices;
	vector<float> m_colors;
	vector<float> m_normals;
	vector<float> m_texCoords;
	vector<U32> m_indices;
	vector<U8> 	m_vFlags;
};


}
}


#endif /* GEOMETRY_H_ */
