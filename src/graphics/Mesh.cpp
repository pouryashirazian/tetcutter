/*
 * PS_Mesh.cpp
 *
 *  Created on: Jul 4, 2011
 *      Author: pourya
 */

#include <assert.h>
#include <fstream>
#include <algorithm>
#include <map>
#include "Mesh.h"
#include "base/FileDirectory.h"
#include "base/Logger.h"

using namespace PS;
using namespace PS::GL;
using namespace PS::FILESTRINGUTILS;

namespace PS{
namespace MESH{

MeshMaterial::MeshMaterial():Asset() {
    m_isSerializeable = true;
}

MeshMaterial::MeshMaterial(const string& name) {
    m_isSerializeable = true;
    strMTLName = name;
}

MeshMaterial::~MeshMaterial() {

}

bool MeshMaterial::load(const char *chrFilePath) {
    std::vector<AnsiStr> content;
    if(!ReadTextFile(chrFilePath, content))
        return false;

    //Process content line by line
    AnsiStr strLine;
    std::vector<AnsiStr> words;
    for(U32 i=0; i<content.size(); i++)
    {
        strLine = content[i];
        if(strLine.firstChar() == '#')
            continue;
        if(strLine.decompose(' ', words) == 0)
            continue;

        int ctWords = (int)words.size();

        if((words[0] == "newmtl")&&(ctWords == 2))
        {
            this->strMTLName = string(words[1].cptr());
        }
        else if((words[0] == "Ns")&&(ctWords == 2))
        {
            this->shininess = static_cast<float>(atof(words[1].ptr()));
        }
        else if((words[0] == "Ka")&&(ctWords == 4))
        {
            this->ambient.x = static_cast<float>(atof(words[1].ptr()));
            this->ambient.y = static_cast<float>(atof(words[2].ptr()));
            this->ambient.z = static_cast<float>(atof(words[3].ptr()));
        }
        else if((words[0] == "Kd")&&(ctWords == 4))
        {
            this->diffused.x = static_cast<float>(atof(words[1].ptr()));
            this->diffused.y = static_cast<float>(atof(words[2].ptr()));
            this->diffused.z = static_cast<float>(atof(words[3].ptr()));
        }
        else if((words[0] == "Ks")&&(ctWords == 4))
        {
            this->specular.x = static_cast<float>(atof(words[1].ptr()));
            this->specular.y = static_cast<float>(atof(words[2].ptr()));
            this->specular.z = static_cast<float>(atof(words[3].ptr()));
        }
        else if((words[0] == "Ni")&&(ctWords == 2))
        {
            this->refractIndex = static_cast<float>(atof(words[1].ptr()));
        }
        else if((words[0] == "d")&&(ctWords == 2))
        {
            //d factor for transparency
            this->trans = static_cast<float>(atof(words[1].ptr()));
        }
        else if((words[0] == "illum")&&(ctWords == 2))
        {
            this->illuminationModel = static_cast<int>(atoi(words[1].ptr()));
        }
        else if((words[0] == "map_Kd")&&(ctWords == 2))
        {
            this->strTexName = string(words[1].cptr());
        }
    }
    content.resize(0);
    return true;

}

bool MeshMaterial::store(const char *chrFilePath) {
	std::vector<AnsiStr> vContent;

	if(strMTLName.length() > 0)
		vContent.push_back(printToAStr("# PS::MESH Material File: %s", strMTLName.c_str()));
	else
		vContent.push_back(AnsiStr("# PS::MESH Material File"));
	vContent.push_back("# Material Count: 1");
	vContent.push_back("newmtl");
	vContent.push_back(printToAStr("Ns %f", shininess));
	vContent.push_back(printToAStr("Ka %f %f %f", ambient.x, ambient.y, ambient.z));
	vContent.push_back(printToAStr("Kd %f %f %f", diffused.x, diffused.y, diffused.z));
	vContent.push_back(printToAStr("Ks %f %f %f", specular.x, specular.y, specular.z));

	vContent.push_back(printToAStr("Ni %f", refractIndex));
	vContent.push_back(printToAStr("d %f", trans));
	vContent.push_back(printToAStr("illum %d", illuminationModel));

	if(strTexName.length() > 0)
		vContent.push_back(printToAStr("map_Kd %s", strTexName.c_str()));

	return WriteTextFile(AnsiStr(chrFilePath), vContent);
}





MeshNode::MeshNode()
{
    init();
}

MeshNode::MeshNode(const string &name) {
    init();
    m_strNodeName = name;
}

MeshNode::MeshNode(const MeshNode* lpNode) {
	m_arrVertices.assign(lpNode->m_arrVertices.begin(), lpNode->m_arrVertices.end());
	m_arrNormals.assign(lpNode->m_arrNormals.begin(), lpNode->m_arrNormals.end());
	m_arrColors.assign(lpNode->m_arrColors.begin(), lpNode->m_arrColors.end());
	m_arrTexCoords.assign(lpNode->m_arrTexCoords.begin(), lpNode->m_arrTexCoords.end());
	m_arrIndices.assign(lpNode->m_arrIndices.begin(), lpNode->m_arrIndices.end());

	m_strNodeName = lpNode->m_strNodeName;
	m_szUnitVertex = lpNode->m_szUnitVertex;
	m_szUnitColor = lpNode->m_szUnitColor;
	m_szUnitTexCoord = lpNode->m_szUnitTexCoord;
	m_szUnitFace = lpNode->m_szUnitFace;
	m_lpMaterial = lpNode->m_lpMaterial;
}

MeshNode::~MeshNode()
{
	m_arrIndices.resize(0);
	m_arrTexCoords.resize(0);
	m_arrNormals.resize(0);
	m_arrVertices.resize(0);
}

void MeshNode::init() {
    m_strNodeName = "UNTITLED";
    m_szUnitVertex   = 3;
    m_szUnitColor 	 = 3;
    m_szUnitTexCoord = 2;
    m_szUnitFace 	 = 3;

    m_lpMaterial = NULL;
}

bool MeshNode::readbackMeshV3T3(U32& ctVertices, vector<float>& vertices,
									U32& ctTriangles, vector<U32>& elements) {
	if(m_szUnitFace != 3) {
		LogError("This is not a triangle mesh");
		return false;
	}

	if(m_szUnitVertex != 3 && m_szUnitVertex != 4) {
		LogError("Number of vertices is not 3 nor 4");
		return false;
	}

	ctVertices = this->countVertices();
	ctTriangles = this->countFaces();
	elements.assign(this->m_arrIndices.begin(), this->m_arrIndices.end());

	if(m_szUnitVertex == 4) {
		vertices.resize(ctVertices * 3);

		vector<float> arrXYZW;
		arrXYZW.assign(this->m_arrVertices.begin(), this->m_arrVertices.end());
		for(U32 i=0; i < ctVertices; i++) {
			vertices[i * 3] = arrXYZW[i*4];
			vertices[i * 3 + 1] = arrXYZW[i*4 + 1];
			vertices[i * 3 + 2] = arrXYZW[i*4 + 2];
		}
	}
	else if(m_szUnitVertex == 3)
		vertices.assign(this->m_arrVertices.begin(), this->m_arrVertices.end());

	return true;
}


int MeshNode::add(const vec3f& v, MemoryBufferType vat, int step)
{
	if(step > 3 || step < 0)
		return -1;

	int idxAdded = -1;
	switch(vat) {
	case(mbtPosition):
	{
		m_szUnitVertex = step;
		for(int i=0; i<step; i++)
			m_arrVertices.push_back(v.e[i]);
		idxAdded = (int)this->countVertices() - 1;
	}
	break;

	case(mbtNormal):
	{
		for(int i=0; i<step; i++)
			m_arrNormals.push_back(v.e[i]);
		idxAdded = (int)this->countNormals() - 1;
	}
	break;

	case(mbtTexCoord):
	{
		m_szUnitTexCoord = step;
		for(int i=0; i<step; i++)
			m_arrTexCoords.push_back(v.e[i]);
		idxAdded = (int)this->countTexCoords() - 1;
	}
	break;

	default: {
	}

	}

	return idxAdded;
}

int MeshNode::add(const vec4f& v, MemoryBufferType vat, int step) {
	if(step > 4 || step < 0)
		return -1;

	int idxAdded = -1;
	switch(vat) {
	case(mbtPosition):
	{
		m_szUnitVertex = step;
		for(int i=0; i<step; i++)
			m_arrVertices.push_back(v.e[i]);
		idxAdded = (int)this->countVertices() - 1;
	}
	break;

	case(mbtNormal):
	{
		for(int i=0; i<step; i++)
			m_arrNormals.push_back(v.e[i]);
		idxAdded = (int)this->countNormals() - 1;
	}
	break;

	case(mbtTexCoord):
	{
		m_szUnitTexCoord = step;
		for(int i=0; i<step; i++)
			m_arrTexCoords.push_back(v.e[i]);
		idxAdded = (int)this->countTexCoords() - 1;
	}
	break;

	default: {
	}

	}

	return idxAdded;

}

int MeshNode::add(const vector<float>& arrValues, MemoryBufferType vat, int step) {

	int idxAdded = -1;
	switch(vat) {
	case(mbtPosition):
	{
		m_szUnitVertex = step;
		for(int i=0; i<step; i++)
			m_arrVertices.push_back(arrValues[i]);
		idxAdded = (int)this->countVertices() - 1;
	}
	break;

	case(mbtNormal):
	{
		for(int i=0; i<step; i++)
			m_arrNormals.push_back(arrValues[i]);
		idxAdded = (int)this->countNormals() - 1;
	}
	break;

	case(mbtTexCoord):
	{
		m_szUnitTexCoord = step;
		for(int i=0; i<step; i++)
			m_arrTexCoords.push_back(arrValues[i]);
		idxAdded = (int)this->countTexCoords() - 1;
	}
	break;

	default: {
	}

	}

	return idxAdded;

}

void MeshNode::setVertexAttrib(const vector<float>& arrAttribs, MemoryBufferType vat, int step) {
	switch(vat) {
	case(mbtPosition): {
		m_szUnitVertex = step;
		m_arrVertices.assign(arrAttribs.begin(), arrAttribs.end());
	}
	break;

	case(mbtColor): {
		m_szUnitColor = step;
		m_arrColors.assign(arrAttribs.begin(), arrAttribs.end());
	}
	break;

	case(mbtNormal): {
		m_arrNormals.assign(arrAttribs.begin(), arrAttribs.end());
	}
	break;

	case(mbtTexCoord): {
		m_szUnitTexCoord = step;
		m_arrTexCoords.assign(arrAttribs.begin(), arrAttribs.end());
	}
	break;



	default: {

	}
	}
}

int MeshNode::addVertex(const vec3f& v)
{
	assert(m_szUnitVertex == 3);
	m_arrVertices.push_back(v.x);
	m_arrVertices.push_back(v.y);
	m_arrVertices.push_back(v.z);
	return (int)countVertices() - 1;
}

int MeshNode::addNormal(const vec3f& n)
{
	m_arrNormals.push_back(n.x);
	m_arrNormals.push_back(n.y);
	m_arrNormals.push_back(n.z);
	return (int)countNormals() - 1;
}

int MeshNode::addTexCoord2(const vec2f& t)
{
	assert(m_szUnitTexCoord == 2);
	m_arrTexCoords.push_back(t.x);
	m_arrTexCoords.push_back(t.y);
	return (int)countTexCoords() - 1;
}

int MeshNode::addTexCoord3(const vec3f& t)
{
	assert(m_szUnitTexCoord == 3);
	m_arrTexCoords.push_back(t.x);
	m_arrTexCoords.push_back(t.y);
	m_arrTexCoords.push_back(t.z);
	return (int)countTexCoords() - 1;
}

void MeshNode::addFaceIndex(U32 index)
{
	m_arrIndices.push_back(index);
}

void MeshNode::setFaceIndices(const vector<U32>& arrFaces, int unitFace) {
	m_szUnitFace = unitFace;
	m_arrIndices.assign(arrFaces.begin(), arrFaces.end());
}

int MeshNode::countFaceIndexErrors() const
{
	size_t ctVertices = countVertices();

	int ctErrors = 0;
	for(size_t i=0; i<m_arrIndices.size(); i++)
	{
		if(m_arrIndices[i] >= ctVertices)
			ctErrors++;
	}

	return ctErrors;
}

void MeshNode::computeVertexNormalsFromFaces() {
    if(m_szUnitFace != 3)
        return;

    //Per each vertex we need to compute all the neighboring faces
    vec3u32 face;
    vec3f v[3];
    vec3f n[3];
    vec3f nf;
    U32 ctFaces = countFaces();
    m_arrNormals.resize(m_arrVertices.size());
    std::fill(m_arrNormals.begin(), m_arrNormals.end(), 0.0f);
    for(U32 i=0; i<ctFaces; i++) {
        face.load(&m_arrIndices[i*3]);
        v[0] = getVertex(face.x);
        v[1] = getVertex(face.y);
        v[2] = getVertex(face.z);

        //cross(b-a, c-a)
        //Face Normal
        nf = vec3f::cross(v[1] - v[0], v[2] - v[0]);
        n[0] = getNormal(face.x) + nf;
        n[1] = getNormal(face.y) + nf;
        n[2] = getNormal(face.z) + nf;

        n[0].store(&m_arrNormals[face.x*3]);
        n[1].store(&m_arrNormals[face.y*3]);
        n[2].store(&m_arrNormals[face.z*3]);
    }

    //Normalize all normals
    U32 ctVertices = this->countVertices();
    for(U32 i=0; i<ctVertices; i++) {
        nf = getNormal(i);
        nf.normalize();
        nf.store(&m_arrNormals[i*3]);
    }
}


vec3f MeshNode::getVertex(int idxVertex) const
{
    //assert(szUnitVertex == 3);
    return vec3f(&m_arrVertices[idxVertex*m_szUnitVertex]);
}

vec3f MeshNode::getNormal(int idxVertex) const
{
    return vec3f(&m_arrNormals[idxVertex*3]);
}

vec2f MeshNode::getTexCoord2(int idxVertex) const
{
    //assert(szUnitTexCoord == 2);
    return vec2f(&m_arrTexCoords[idxVertex * m_szUnitTexCoord]);
}

vec3f MeshNode::getTexCoord3(int idxVertex) const
{
    //assert(szUnitTexCoord == 2);
    return vec3f(&m_arrTexCoords[idxVertex * m_szUnitTexCoord]);
}

AABB MeshNode::computeBoundingBox() const
{
	AABB oct;
	size_t ctVertices = this->countVertices();
	if(ctVertices == 0)
		return oct;

	vec3f lo = this->getVertex(0);
	vec3f hi = this->getVertex(0);

	for(int i=1; i<ctVertices; i++)
	{
		vec3f v = this->getVertex(i);
		if(v.x < lo.x)	lo.x = v.x;
		if(v.y < lo.y)	lo.y = v.y;
		if(v.z < lo.z)	lo.z = v.z;

		if(v.x > hi.x)	hi.x = v.x;
		if(v.y > hi.y)	hi.y = v.y;
		if(v.z > hi.z)	hi.z = v.z;
	}

	oct.set(lo, hi);
	return oct;
}

void MeshNode::getVertexAttrib(U32& count, vector<float>& arrAttribs, MemoryBufferType vat) const {
	switch(vat) {
	case(mbtPosition): {
		count = countVertices();
		arrAttribs.assign(m_arrVertices.begin(), m_arrVertices.end());
	}
	break;

	case(mbtColor): {
		count = countColors();
	}
	break;

	case(mbtTexCoord): {
		count = countTexCoords();
		arrAttribs.assign(m_arrTexCoords.begin(), m_arrTexCoords.end());
	}
	break;

	case(mbtNormal): {
		count = countVertices();
		arrAttribs.assign(m_arrNormals.begin(), m_arrNormals.end());
	}
	break;

	case(mbtCount): {
		LogError("Invalid attribute for reading operation");
	}
	break;

	case(mbtFaceIndices): {
		LogError("Invalid attribute for reading operation");
	}
	break;

	}
}

void MeshNode::getFaces(U32& count, vector<U32>& faces) const {
	count = countFaces();
	faces.assign(m_arrIndices.begin(), m_arrIndices.end());
}

void MeshNode::move(const vec3f& d) {
    U32 count = (U32)m_arrVertices.size() / 3;
    for(U32 i=0; i<count; i++) {
        float* pValues = &m_arrVertices[i * 3];
        vec3f v = vec3f(pValues) + d;
        v.store(pValues);
    }
}

void MeshNode::scale(const vec3f& s) {
    U32 count = (U32)m_arrVertices.size() / 3;
    for(U32 i=0; i<count; i++) {
        float* pValues = &m_arrVertices[i * 3];
        vec3f v = vec3f(pValues);
        v.x *= s.x;
        v.y *= s.y;
        v.z *= s.z;
        v.store(pValues);
    }
}

void MeshNode::rotate(const quat& q) {
    quat qInv = q.inverted();
    U32 count = (U32)m_arrVertices.size() / 3;
    for(U32 i=0; i<count; i++) {
        float* pValues = &m_arrVertices[i * 3];
        vec3f v = vec3f(pValues);
        v = q.transform(qInv, v);
        v.store(pValues);
    }
}


void MeshNode::fitToBBox(const AABB& box) {

    vec3f e = this->computeBoundingBox().extent();    
    bool isDegenerate = ((e.x < EPSILON)||(e.y < EPSILON)||(e.z < EPSILON));


    //To keep the aspect ratio contant scale down the model to min BBOX
    if(!isDegenerate) {
        vec3f s = vec3f::div(box.extent(), e);
        float sMin = MATHMIN(MATHMIN(s.x, s.y), s.z);
        scale(vec3f(sMin));
    }

    AABB nb = computeBoundingBox();
    move(box.lower() - nb.lower());

    //Compare boxes now
    nb = computeBoundingBox();
}



/*
void CMeshNode::add(obj_vector* v, ArrayType where)
{
	if(where == atVertex)
	{
		for(int i=0;i<szUnitVertex;i++)
			arrVertices.push_back(static_cast<float>(v->e[i]));
	}
	else if(where == atNormal)
	{
		for(int i=0;i<3;i++)
			arrNormals.push_back(static_cast<float>(v->e[i]));
	}
	else if(where == atTexture)
	{
		for(int i=0;i<szUnitTexCoord;i++)
			arrTexCoords.push_back(static_cast<float>(v->e[i]));
	}
	else if(where == atFaceIndex)
	{
		for(int i=0;i<szUnitFace;i++)
			arrIndices.push_back(static_cast<float>(v->e[i]));
	}
}
*/
/////////////////////////////////////////////////////////////////////////
Mesh::Mesh():Asset()  {

}

Mesh::Mesh(const char* chrFileName)
{
    load(chrFileName);
}

Mesh::~Mesh()
{
	for(size_t i=0; i<m_nodes.size(); i++)
		SAFE_DELETE(m_nodes[i]);
	m_nodes.resize(0);

	for(size_t i=0; i<m_materials.size(); i++)
		SAFE_DELETE(m_materials[i]);
	m_materials.resize(0);
}

void Mesh::addNode(MeshNode* lpMeshNode)
{
	if(lpMeshNode != NULL)
		m_nodes.push_back(lpMeshNode);
}

MeshNode*	Mesh::getNode(int idx) const
{
	if(idx >=0 && idx < (int)m_nodes.size())
		return m_nodes[idx];
	else
		return NULL;
}

MeshNode* Mesh::getNode(const string& name) const {
	for(U32 i=0; i<m_nodes.size(); i++) {
		if(m_nodes[i]->getName() == name)
			return m_nodes[i];
	}

	return NULL;
}

void Mesh::addMeshMaterial(MeshMaterial* lpMaterial)
{
    if(lpMaterial != NULL) {
		m_materials.push_back(lpMaterial);
        m_mapMaterial.insert(std::pair<string, MeshMaterial*>(string(lpMaterial->name()), lpMaterial));
    }
}

MeshMaterial* Mesh::getMaterial(int idx) const
{
	if(idx >= 0 && idx < (int)m_materials.size())
		return m_materials[idx];
	else
		return NULL;
}

MeshMaterial* Mesh::getMaterial(const string& strName) const
{
    std::map<string, MeshMaterial*>::const_iterator it;
    it = m_mapMaterial.find(strName);
    if(it != m_mapMaterial.end())
        return it->second;
    else
        return NULL;
}

bool Mesh::load(const char* chrFilePath)
{
    m_strFilePath = string(chrFilePath);
    AnsiStr strExt = ExtractFileExt(AnsiStr(m_strFilePath.c_str()));
    bool bres = false;
    if(strExt == "obj") {
        bres = loadObj(m_strFilePath.c_str());
        computeMissingNormals();
    }
	else
		LogError("Invalid file format.");
    return bres;
}

bool Mesh::store(const char* chrFilePath) {
	ofstream fp;
	fp.open(chrFilePath);


	//Store All Materials
	AnsiStr strFilePath = ExtractFilePath(AnsiStr(chrFilePath));
	AnsiStr strMtlPath = strFilePath;
	for(U32 i=0; i<countMaterials(); i++) {

		if(getMaterial(i)->name().length() > 0)
			strMtlPath += AnsiStr(getMaterial(i)->name().c_str()) + ".mtl";
		else
			strMtlPath += printToAStr("mtlnode%d.mtl", i+1);

		getMaterial(i)->store(strMtlPath.cptr());
	}



	//Face indices are at index 1
	MeshNode* lpNode = NULL;
	for(U32 iNode=0; iNode < countNodes(); iNode++) {
		lpNode = getNode(iNode);

		if(lpNode == NULL)
			continue;

		//Write All vertices
		fp << "# Generated by PS::MESH\n";
		fp << "# Number of vertices: " << lpNode->countVertices() << endl;
		fp << "# Number of texture coordinates: " << lpNode->countTexCoords() << endl;
		fp << "# Number of normals: " << lpNode->countNormals() << endl;
		fp << "# Number of faces: " << lpNode->countFaces() << endl;
		fp << "# Group " << iNode+1 << " of " << countNodes() << endl;

		//Vertices
		U32 ctVertices;
		U32 szUnitVertex = lpNode->getUnitVertex();
		vector<float> attribs;
		lpNode->getVertexAttrib(ctVertices, attribs, mbtPosition);
		for(U32 i=0; i < ctVertices; i++) {
			fp << "v";
			for(U32 j=0; j<szUnitVertex; j++) {
				fp << " " << attribs[i * szUnitVertex + j];
			}

			fp << endl;
		}

		//Normals
		for(U32 i=0; i < lpNode->countVertices(); i++) {

			vec3f n = lpNode->getNormal(i);
			fp << "vn " << n.x << " " << n.y << " " << n.z << endl;
		}

		//TexCoords
		if(lpNode->getUnitTexCoord() == 2) {
			for(U32 i=0; i < lpNode->countTexCoords(); i++) {
				vec2f t = lpNode->getTexCoord2(i);
				fp << "vt " << t.x << " " << t.y << endl;
			}
		}
		else if(lpNode->getUnitTexCoord() == 3) {
			for(U32 i=0; i < lpNode->countTexCoords(); i++) {
				vec3f t = lpNode->getTexCoord3(i);
				fp << "vt " << t.x << " " << t.y << " " << t.z << endl;
			}
		}
		else
			LogErrorArg1("Unsupported texcoords found in MeshGroup: %d", iNode);

		//Faces
		U32 ctFaces = 0;
		vector<U32> arrIndices;
		lpNode->getFaces(ctFaces, arrIndices);
		if(lpNode->getUnitFace() == 3) {
			vec3u32 f;
			vec3u32 inc(1);
			for(U32 i=0; i<ctFaces; i++) {
				f.load(&arrIndices[i * 3]);
				f = f + inc;
				fp << "f " << f.x << " " << f.y << " " << f.z << endl;
			}
		}
		else if(lpNode->getUnitFace() == 4) {
			vec4u32 f;
			vec4u32 inc(1);
			for(U32 i=0; i<ctFaces; i++) {
				f.load(&arrIndices[i * 4]);
				f = f + inc;
				fp << "f " << f.x << " " << f.y << " " << f.z << " " << f.w << endl;
			}
		}

	}



	fp.close();
	return true;
}

AABB Mesh::computeBoundingBox() const
{
	AABB box;
    if(this->countNodes() == 0)
		return box;

	box = m_nodes[0]->computeBoundingBox();
	for(size_t i=1; i<m_nodes.size(); i++)
		box = box.united(m_nodes[i]->computeBoundingBox());
	return box;
}

void Mesh::computeMissingNormals() {
    for(int i=0; i<this->countNodes(); i++) {
        MeshNode* aNode = this->getNode(i);
        if(aNode->countVertices() > 0 && aNode->countNormals() == 0)
            aNode->computeVertexNormalsFromFaces();
    }
}

void Mesh::move(const vec3f &d) {
    for(U32 i=0; i<this->countNodes(); i++)
        this->getNode(i)->move(d);
}

void Mesh::scale(const vec3f& s) {
    for(U32 i=0; i<this->countNodes(); i++)
        this->getNode(i)->scale(s);
}

void Mesh::rotate(const quat& q) {
    for(U32 i=0; i<this->countNodes(); i++)
        this->getNode(i)->rotate(q);
}

void Mesh::fitToBBox(const AABB& box) {
    for(U32 i=0; i<this->countNodes(); i++)
        this->getNode(i)->fitToBBox(box);
}

bool Mesh::loadObj(const char* chrFileName)
{
	std::vector<AnsiStr> content;
    if(!ReadTextFile(chrFileName, content))
		return false;

	//Process content line by line
	AnsiStr strLine;
	std::vector<AnsiStr> words;
	std::vector<bool> arrProcessed;
	arrProcessed.resize(content.size());
	std::fill(arrProcessed.begin(), arrProcessed.end(), false);

	//Memory for mesh object
	vector<float> arrVertices;
	vector<float> arrNormals;
	vector<float> arrTexCoords;
	vector<U32> arrIndices;
	U32 iFace = 0;
	U32 ctFaces = 0;
	U8 faceUnit = 0;
	U32 ctMeshNodes = 0;
	U32 ctMaterialNodes = 0;

	U8 arrAttribUnit[mbtCount];
	U32 arrAttribCount[mbtCount];
	U32 idxCurrent[mbtCount];
	for(int i=0; i<mbtCount; i++) {
		arrAttribUnit[i] = 0;
		arrAttribCount[i] = 0;
		idxCurrent[i] = 0;
	}

	//Pre-process once and reserve memory
	for(U32 i=0; i<content.size(); i++)
	{
		strLine = content[i];

		if(strLine.firstChar() == '#')
		{
			arrProcessed[i] = true;
			continue;
		}
		if(strLine.decompose(' ', words) == 0)
		{
			arrProcessed[i] = true;
			continue;
		}

		int ctWords = (int)words.size();

		//Position
		if(words[0] == "v")
		{
			if(arrAttribCount[mbtPosition] == 0)
				arrAttribUnit[mbtPosition] = ctWords-1;
			arrAttribCount[mbtPosition] ++;
		}
		//Normal
		else if(words[0] == "vn")
		{
			if(arrAttribCount[mbtNormal] == 0)
				arrAttribUnit[mbtNormal] = ctWords-1;
			arrAttribCount[mbtNormal] ++;
		}
		//TexCoords
		else if(words[0] == "vt")
		{
			if(arrAttribCount[mbtTexCoord] == 0)
				arrAttribUnit[mbtTexCoord] = ctWords-1;
			arrAttribCount[mbtTexCoord] ++;
		}
		else if(words[0] == "f") {
			if(ctFaces == 0)
				faceUnit = ctWords - 1;
			else if(faceUnit != ctWords -1)
				LogErrorArg2("Irregular mesh file! Face %d has %d vertices!", ctFaces, ctWords-1);

			ctFaces++;
		}
		else if((words[0] == "o")&&(ctWords == 2))
		{
			//Create a new mesh node
			MeshNode* aNode = new MeshNode(string(words[1].cptr()));
			addNode(aNode);
			ctMeshNodes++;
		}
		else if((words[0] == "mtllib")&&(ctWords == 2))
		{
			AnsiStr strFP = ExtractFilePath(AnsiStr(chrFileName)) + words[1];

            //Load Material
            MeshMaterial* aMtrl = new MeshMaterial(string(words[1].cptr()));
            aMtrl->load(strFP.cptr());
            addMeshMaterial(aMtrl);
            ctMaterialNodes ++;
		}
	}

	//Normals
	if(arrAttribCount[mbtPosition] != arrAttribCount[mbtNormal]) {
		LogErrorArg2("Number of normals not match vertices. V# %d, N# %d", arrAttribCount[mbtPosition], arrAttribCount[mbtNormal]);
		arrAttribCount[mbtNormal] = 0;
	}

	//Allocate memory
	arrVertices.resize(arrAttribCount[mbtPosition] * arrAttribUnit[mbtPosition]);
	arrNormals.resize(arrAttribCount[mbtNormal] * arrAttribUnit[mbtNormal]);
	arrTexCoords.resize(arrAttribCount[mbtTexCoord] * arrAttribUnit[mbtTexCoord]);

	//We won't triangulate quad meshes. Might use quad mesh for subdivision or Micropolygon rendering!
	arrIndices.resize(ctFaces * faceUnit);

	//Now we can process faces and assemble them from the list of vertices normals
	U32 idxMeshNode = 0;
	MeshNode* lpCurrentMeshNode = NULL;
	if(ctMeshNodes > 0)
		lpCurrentMeshNode = getNode(0);
	else {
		lpCurrentMeshNode = new MeshNode();
		this->addNode(lpCurrentMeshNode);
	}

	//Process all obj file content
	for(U32 i=0; i<content.size(); i++)
	{
		strLine = content[i];

		if(arrProcessed[i])
			continue;
		if(strLine.firstChar() == '#')
			continue;
		if(strLine.decompose(' ', words) == 0)
			continue;


		int ctWords = (int)words.size();
		if((words[0] == "v") && (arrAttribCount[mbtPosition] > 0)) {
			if(arrAttribUnit[mbtPosition] == 3) {
				vec3f v;
				v.x = static_cast<float>(atof(words[1].ptr()));
				v.y = static_cast<float>(atof(words[2].ptr()));
				v.z = static_cast<float>(atof(words[3].ptr()));

				v.store(&arrVertices[idxCurrent[mbtPosition] * arrAttribUnit[mbtPosition]]);
			}
			else if(arrAttribUnit[mbtPosition] == 4) {
				vec4f v;
				v.x = static_cast<float>(atof(words[1].ptr()));
				v.y = static_cast<float>(atof(words[2].ptr()));
				v.z = static_cast<float>(atof(words[3].ptr()));
				v.w = static_cast<float>(atof(words[4].ptr()));

				v.store(&arrVertices[idxCurrent[mbtPosition] * arrAttribUnit[mbtPosition]]);
			}

			idxCurrent[mbtPosition] ++;
		}
		else if((words[0] == "vn") && (arrAttribCount[mbtNormal] > 0)) {

			vec3f n;
			n.x = static_cast<float>(atof(words[1].ptr()));
			n.y = static_cast<float>(atof(words[2].ptr()));
			n.z = static_cast<float>(atof(words[3].ptr()));
			n.store(&arrNormals[idxCurrent[mbtNormal] * 3]);
			idxCurrent[mbtNormal] ++;
		}
		else if((words[0] == "vt" && arrAttribCount[mbtTexCoord] > 0)) {
			U32 idxTex = idxCurrent[mbtTexCoord] * arrAttribUnit[mbtTexCoord];
			for(int j=0; j<arrAttribUnit[mbtTexCoord]; j++)
				arrTexCoords[idxTex + j] = static_cast<float>(atof(words[j+1].ptr()));
			idxCurrent[mbtTexCoord] ++;
		}
		else if((words[0] == "o")&&(ctWords == 2)) {

			if(lpCurrentMeshNode != NULL)
				lpCurrentMeshNode->setFaceIndices(arrIndices, faceUnit);

			lpCurrentMeshNode = getNode(string(words[1].cptr()));
			idxMeshNode++;
		}
		else if((words[0] == "usemtl")&&(lpCurrentMeshNode)&&(ctWords == 2))
		{
			lpCurrentMeshNode->setMaterial(getMaterial(string(words[1].cptr())));
		}
        else if((words[0] == "f")&&(ctWords > 3)&&(ctFaces > 0))
		{
			int idxVertex[4];
			int idxTexCoords[4];
			int idxNormal[4];

			//Process line of face
			for(int j=0; j<ctWords-1; j++)
			{
				std::vector<AnsiStr> segments;
				strLine = words[j + 1];
				strLine.removeStartEndSpaces();
				strLine.replaceChars('/', ' ');
				if(strLine.decompose(' ', segments) > 1)
				{
					int ctSegs = (U32)segments.size();
					if(ctSegs == 3)
					{
						idxVertex[j] 	= atoi(segments[0].ptr()) - 1;
						idxTexCoords[j] = atoi(segments[1].ptr()) - 1;
						idxNormal[j] 	= atoi(segments[2].ptr()) - 1;
					}
					else
					{
						idxVertex[j] 	= atoi(segments[0].ptr()) - 1;
						idxNormal[j] 	= atoi(segments[1].ptr()) - 1;
					}
				}
				else
				{
					idxVertex[j] = atoi(words[j + 1].ptr()) - 1;
					idxNormal[j] = idxVertex[j];
				}


				//Add to index buffer
				arrIndices[ iFace * faceUnit + j] = idxVertex[j];
			}

			//Increment index of face
			iFace ++;
		}
		else if((words[0] == "s")&&(ctWords == 2))
		{
			if(words[1] == "off")
			{

			}
		}
	}

	//Add Last Node indices
	lpCurrentMeshNode->setFaceIndices(arrIndices, faceUnit);

	for(U32 i=0; i < countNodes(); i++) {

		lpCurrentMeshNode = getNode(i);
		if(arrAttribCount[mbtPosition] > 0)
			lpCurrentMeshNode->setVertexAttrib(arrVertices, mbtPosition, arrAttribUnit[mbtPosition]);

		if(arrAttribCount[mbtNormal] > 0)
			lpCurrentMeshNode->setVertexAttrib(arrNormals, mbtNormal, arrAttribUnit[mbtNormal]);

		if(arrAttribCount[mbtTexCoord] > 0)
			lpCurrentMeshNode->setVertexAttrib(arrTexCoords, mbtTexCoord, arrAttribUnit[mbtTexCoord]);
	}


	//Cleanup
	arrProcessed.resize(0);
	words.resize(0);
	content.resize(0);
	return (m_nodes.size() > 0);
}


////////////////////////////////////////////////////////////////////////////////////////////
/*
CMeshManager::CMeshManager()
{

}

CMeshManager::~CMeshManager()
{
	cleanup();
}

void CMeshManager::cleanup()
{
	CMesh* lpMesh = NULL;
	for(size_t i=0; i<m_storage.size(); i++)
	{
		lpMesh = getResource(i);
		SAFE_DELETE(lpMesh);
		this->setResource(i, NULL);
	}

	m_storage.resize(0);
	m_vFilePaths.resize(0);
}

CMesh* CMeshManager::loadResource(const AnsiStr& inStrFilePath, AnsiStr& inoutStrName)
{
	inoutStrName = PS::FILESTRINGUTILS::ExtractFileTitleOnly(inStrFilePath);
	CMesh* aMesh = new CMesh();
	if(aMesh->load(inStrFilePath.cptr()))
	{
		return aMesh;
	}
	else
	{
		SAFE_DELETE(aMesh);
		return NULL;
	}
}
*/

}
}
