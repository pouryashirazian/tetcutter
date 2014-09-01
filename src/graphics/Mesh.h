/*
 * PS_Mesh.h
 *
 *  Created on: Jul 4, 2011
 *      Author: pourya
 */

#ifndef PS_MESH_H_
#define PS_MESH_H_

#include <vector>

#include "base/String.h"
#include "base/AssetManager.h"
#include "base/Vec.h"
#include "base/Quaternion.h"

#include "GLTypes.h"
#include "AABB.h"


using namespace std;
using namespace PS;
using namespace PS::GL;
using namespace PS::MATH;

namespace PS{
namespace MESH{


/*!
 * structure holding a mesh material object
 */
class MeshMaterial : public Asset
{
public:
    MeshMaterial();
    MeshMaterial(const string& name);
    virtual ~MeshMaterial();

    bool load(const char *chrFilePath);
    bool store(const char *chrFilePath);
public:
    string strMTLName;
    string strTexName;
	vec3f ambient;
	vec3f diffused;
	vec3f specular;
	float reflect;
	float refract;
	float trans;
	float shininess;
	float glossy;
	float refractIndex;

	int illuminationModel;
};

/*!
 * MeshNode represents a single node in our mesh file.
 * It is complete mesh by itself.
 *
 */
class MeshNode {
public:
	MeshNode();
    explicit MeshNode(const string& name);
    explicit MeshNode(const MeshNode* lpNode);
	virtual ~MeshNode();

    void init();

	//Readback mesh
	bool readbackMeshV3T3(U32& ctVertices, vector<float>& vertices,
							 U32& ctTriangles, vector<U32>& elements);

	/*!
	 * Adds a vec3f v to the destination buffer dest.
	 * @return designated index of added vertex. This index is the actual vertex number
	 * not the position in the float array. The position in the float array can be computed as
	 * index x szUnitVertex
	 */
	int add(const vec3f& v, MemoryBufferType vat = mbtPosition, int step = 3);
	int add(const vec4f& v, MemoryBufferType vat = mbtPosition, int step = 4);
	int add(const vector<float>& arrValues, MemoryBufferType vat = mbtPosition, int step = 3);
	void setVertexAttrib(const vector<float>& arrAttribs, MemoryBufferType vat = mbtPosition, int step = 3);

	int addVertex(const vec3f& v);
	int addNormal(const vec3f& n);
	int addTexCoord2(const vec2f& t);
	int addTexCoord3(const vec3f& t);

	void addTriangle(U32 tri[3]);
	void addQuad(U32 quad[4]);
	void addFaceIndex(U32 index);
	void setFaceIndices(const vector<U32>& arrFaces, int unitFace);


	/*!
	 * Checks all face indices for invalid values
	 * @return number of indices found out of range
	 */
	int countFaceIndexErrors() const;

    /*!
     * \brief computeVertexNormalsFromFaces compute one normal per each vertex
     * by summing up all adjacent face normals and normalize it.
     */
    void computeVertexNormalsFromFaces();


	//Statistics
	U32 countVertices() const {return (U32)m_arrVertices.size() / (U32)m_szUnitVertex;}
	U32 countColors() const {return (U32)m_arrColors.size() / m_szUnitColor;}
	U32 countNormals() const {return (U32)m_arrNormals.size() / 3;}
	U32 countFaces() const {return (U32)m_arrIndices.size() / (U32)m_szUnitFace;}
	U32 countTexCoords() const {return (U32)m_arrTexCoords.size() / (U32)m_szUnitTexCoord;}

	/*!
	 * returns unit vertex memory stride in bytes
	 */
	U32 getVertexStride() const {return m_szUnitVertex * sizeof(float);}
	U32 getNormalStride() const {return 3 * sizeof(float);}
	U32 getFaceStride() const {return m_szUnitFace * sizeof(U32);}


	//Access Vertex, Normal
	vec3f getVertex(int idxVertex) const;
	vec3f getNormal(int idxVertex) const;
	vec2f getTexCoord2(int idxVertex) const;
	vec3f getTexCoord3(int idxVertex) const;

	//Access Material
	MeshMaterial* getMaterial() const {return m_lpMaterial;}
	void setMaterial(MeshMaterial* aMaterial) {m_lpMaterial = aMaterial;}

	AABB computeBoundingBox() const;

    //Name
    string getName() const {return m_strNodeName;}
    void setName(const string& strName) {m_strNodeName = strName;}

    //Steps
    U8 getUnitVertex() const {return m_szUnitVertex;}
    U8 getUnitNormal() const {return 3;}
    U8 getUnitColor() const {return m_szUnitColor;}
    U8 getUnitTexCoord() const {return m_szUnitTexCoord;}
    U8 getUnitFace() const {return m_szUnitFace;}
    void setUnitFace(U8 s) {m_szUnitFace = s;}

    //Mesh parts
    void getVertexAttrib(U32& count, vector<float>& arrAttribs, MemoryBufferType vat) const;
    void getFaces(U32& count, vector<U32>& faces) const;

    //Mesh Parts
    const vector<float>& vertices() const {return m_arrVertices;}
    const vector<float>& normals() const {return m_arrNormals;}
    const vector<float>& colors() const {return m_arrColors;}
    const vector<float>& texcoords() const {return m_arrTexCoords;}
    const vector<U32>& faceElements() const {return m_arrIndices;}

    /*!
     * Move the entire mesh to a new location
     */
    void move(const vec3f& d);
    void scale(const vec3f& s);
    void rotate(const quat& q);
    void fitToBBox(const AABB& box);

protected:
	std::vector<float> m_arrVertices;
	std::vector<float> m_arrNormals;
	std::vector<float> m_arrColors;
	std::vector<float> m_arrTexCoords;
	std::vector<U32>	m_arrIndices;

    //Init data
    string      m_strNodeName;
	U8			m_szUnitVertex;
	U8			m_szUnitColor;
	U8   		m_szUnitTexCoord;
	U8			m_szUnitFace;
	MeshMaterial* m_lpMaterial;
};


/*
 * structure holding a complete mesh
 */
class Mesh : public Asset{
public:
    Mesh();
	Mesh(const AnsiStr& strFilePath);
	virtual ~Mesh();

	//IO
	bool read(const AnsiStr& strFilePath);
	bool store(const AnsiStr& strFilePath);

	//Mesh Nodes
	void addNode(MeshNode* lpMeshNode);
	MeshNode* getNode(int idx) const;
	MeshNode* getNode(const string& name) const;
    U32 countNodes() const {return (U32)m_nodes.size();}

	//Mesh Materials
	void addMeshMaterial(MeshMaterial* lpMaterial);
	MeshMaterial* getMaterial(int idx) const;
    MeshMaterial* getMaterial(const string& strName) const;
    U32 countMaterials() const { return (U32)m_materials.size();}

    //Get the AABB for the entire mesh
	AABB computeBoundingBox() const;

    //Compute all missing normals
    void computeMissingNormals();

    //Moves the entire mesh to a new location
    void move(const vec3f& d);
    void scale(const vec3f& s);
    void rotate(const quat& q);
    void fitToBBox(const AABB& box);
private:

    //Loads Obj Mesh File
	bool loadObj(const char* chrFileName);

private:
	std::vector<MeshNode*> m_nodes;
	std::vector<MeshMaterial*> m_materials;
    std::map<string, MeshMaterial*> m_mapMaterial;
    AnsiStr m_strFilePath;
};

}
}
#endif /* PS_MESH_H_ */
