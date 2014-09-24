/*
 * Geometry.cpp
 *
 *  Created on: Dec 28, 2013
 *      Author: pourya
 */
#include "Geometry.h"
#include "base/Logger.h"
#include "base/FileDirectory.h"
#include "Mesh.h"

using namespace PS;
using namespace PS::MESH;
using namespace PS::FILESTRINGUTILS;

namespace PS {
namespace GL {

Geometry::Geometry() {
	init();
}

Geometry::Geometry(const Geometry& other) {
	init();
	copyFrom(other);
}

Geometry::Geometry(const AnsiStr& strFilePath) {
	init();
	this->read(strFilePath);
}

Geometry::~Geometry() {
	this->cleanup();
}

void Geometry::cleanup() {
	m_colors.resize(0);
	m_vertices.resize(0);
	m_normals.resize(0);
	m_texCoords.resize(0);
	m_indices.resize(0);
}

void Geometry::init() {
	init(3, 4, 2, ftTriangles);
}

void Geometry::init(int stepVertex, int stepColor,
		   	   	   	   int stepTexCoords, PS::GL::FaceType faceMode) {
	this->cleanup();
	m_stepVertex = stepVertex;
	m_stepColor = stepColor;
	m_stepTexCoord = stepTexCoords;
	m_stepFace = (faceMode == ftQuads) ? 4 : 3;
	m_faceMode = faceMode;
}

//stats
bool Geometry::isCompatible(const Geometry& other) const {
	return ((m_stepVertex == other.m_stepVertex)&&(m_stepColor == other.m_stepColor)&&
			(m_stepTexCoord == other.m_stepTexCoord)&&(m_stepFace == other.m_stepFace)&&
			(m_faceMode == other.m_faceMode));
}

int Geometry::countVertices() const {
	assert(m_stepVertex != 0);
	return (int)m_vertices.size() / m_stepVertex;
}

int Geometry::countColor() const {
	assert(m_stepColor != 0);
	return (int)m_colors.size() / m_stepColor;
}

int Geometry::countTexCoords() const {
	assert(m_stepTexCoord != 0);
	return (int)m_texCoords.size() / m_stepTexCoord;
}

int Geometry::countNormals() const {
	return (int)m_normals.size() / 3;
}

int Geometry::countFaces() const {
	assert(m_stepFace != 0);
	return (int)m_indices.size() / m_stepFace;
}

vec3f Geometry::vertexAt(int index) const {
	assert(index >=0 && index < countVertices());
	return vec3f(&m_vertices[index * m_stepVertex]);
}

vec4f Geometry::colorAt(int index) const {
	assert(index >=0 && index < countColor());

	if(m_stepColor == 4)
		return vec4f(&m_colors[index * m_stepColor]);
	else {
		vec3f color = vec3f(&m_colors[index * m_stepColor]);
		return vec4f(color, 1.0f);
	}
}

vec2f Geometry::texcoordAt(int index) const {
	assert(index >= 0 && index < countTexCoords() && m_stepTexCoord == 2);
	return vec2f(&m_texCoords[index * m_stepTexCoord]);
}

vec3f Geometry::normalAt(int index) const {
	assert(index >=0 && index < countNormals());
	return vec3f(&m_normals[index * 3]);
}

vec3u32 Geometry::triangleAt(int index) const {
	assert(index >= 0 && index < countFaces());
	return vec3u32(&m_indices[index * m_stepFace]);
}

vec4u32 Geometry::quadAt(int index) const {
	assert(index >= 0 && index < countFaces());
	return vec4u32(&m_indices[index * m_stepFace]);
}

void Geometry::setVertex(int index, const vec3f& v) {

	m_vertices[index * m_stepVertex] = v.x;
	m_vertices[index * m_stepVertex + 1] = v.y;
	m_vertices[index * m_stepVertex + 2] = v.z;
}

//Add Attributes
bool Geometry::addVertex(const vec3f& v) {
	if(m_stepVertex != 3 && m_stepVertex != 4)
		return false;

	m_vertices.push_back(v.x);
	m_vertices.push_back(v.y);
	m_vertices.push_back(v.z);

	if(m_stepVertex == 4)
		m_vertices.push_back(1.0f);
	return true;
}

bool Geometry::addColor(const vec4f& c) {
	if(m_stepColor != 3 && m_stepColor != 4)
		return false;
	m_colors.push_back(c.x);
	m_colors.push_back(c.y);
	m_colors.push_back(c.z);

	if(m_stepColor == 4)
		m_colors.push_back(c.w);
	return true;
}

bool Geometry::addTexCoord(float t) {
	if(m_stepTexCoord != 1)
		return false;
	m_texCoords.push_back(t);
	return true;
}

bool Geometry::addTexCoord(const vec2f& t) {
	if(m_stepTexCoord != 2)
		return false;
	m_texCoords.push_back(t.x);
	m_texCoords.push_back(t.y);
	return true;
}

bool Geometry::addTexCoord(const vec3f& t) {
	if(m_stepTexCoord != 3)
		return false;
	m_texCoords.push_back(t.x);
	m_texCoords.push_back(t.y);
	m_texCoords.push_back(t.z);
	return true;
}
    
    
void Geometry::addNormal(const vec3f& n) {
    m_normals.push_back(n.x);
    m_normals.push_back(n.y);
    m_normals.push_back(n.z);
}

bool Geometry::addTriangle(const vec3u32& f) {
	if(m_stepFace != 3)
		return false;

	m_indices.push_back(f.x);
	m_indices.push_back(f.y);
	m_indices.push_back(f.z);
	return true;
}

bool Geometry::addQuad(const vec4u32& f) {
	if(m_stepFace != 4)
		return false;
	m_indices.push_back(f.x);
	m_indices.push_back(f.y);
	m_indices.push_back(f.z);
	m_indices.push_back(f.w);
	return true;
}

void Geometry::extrude(const vec3f& v) {
	vec3f p;
	U32 count = this->countVertices();

	//Start Index
	U32 idxStart = this->countVertices();

	vector<float> arrVertices;
	arrVertices.assign(m_vertices.begin(), m_vertices.end());
	for(U32 i=0; i<count; i++) {
		p.load(&arrVertices[i * m_stepVertex]);
		p = p + v;
		p.store(&arrVertices[i * m_stepVertex]);
	}

	//Append Vertices
	m_vertices.insert(m_vertices.end(), arrVertices.begin(), arrVertices.end());

	//Faces
	if(m_faceMode == ftTriangles) {
		U32 ctTriangles = countFaces();
		vec3u32 tri;
		vec3u32 start(idxStart);
		vector<U32> arrTempFaces;
		arrTempFaces.resize(m_indices.size());
		for(U32 i=0; i<ctTriangles; i++) {
			tri = this->triangleAt(i);
			tri = vec3u32(tri.z, tri.y, tri.x) + start;
			tri.store(&arrTempFaces[i*3]);
		}
		m_indices.insert(m_indices.end(), arrTempFaces.begin(), arrTempFaces.end());
	}
	else
	{
		vector<U32> arrTempFaces;
		arrTempFaces.reserve(m_indices.size());
		for(U32 i=0; i < m_indices.size(); i++)
			arrTempFaces.push_back(m_indices[i] + idxStart);
		m_indices.insert(m_indices.end(), arrTempFaces.begin(), arrTempFaces.end());
	}


	//Add Side Walls
	for(U32 i=0; i<idxStart - 1; i++) {
		addTriangle(vec3u32(i, i + 1, idxStart + i));
		addTriangle(vec3u32(i + 1, idxStart + i + 1, idxStart + i));
	}

	//Copy Normals
	if(countNormals() > 0) {
		vector<float> normals(m_normals);
		m_normals.insert(m_normals.end(), normals.begin(), normals.end());
	}

	//Copy Colors
	if(countColor() > 0) {
		vector<float> colors(m_colors);
		m_colors.insert(m_colors.end(), colors.begin(), colors.end());
	}

	//Copy TecCoords
	if(countTexCoords() > 0) {
		vector<float> texcoords(m_texCoords);
		m_texCoords.insert(m_texCoords.end(), texcoords.begin(), texcoords.end());
	}

	assert(checkIndices());
}

void Geometry::transform(const mat44f& m) {
	vec3f p;
	U32 count = this->countVertices();
	for(U32 i=0; i<count; i++) {
		p.load(&m_vertices[i * m_stepVertex]);
		p = m.mapAffine(p);
		p.store(&m_vertices[i * m_stepVertex]);
	}
}

bool Geometry::addVertexAttribs(const vector<float>& arrAttribs, int step, MemoryBufferType attribKind) {

	switch(attribKind) {
	case(mbtPosition): {
		if(step != m_stepVertex)
			return false;
		m_vertices.insert(m_vertices.end(), arrAttribs.begin(), arrAttribs.end());
		break;
	}

	case(mbtColor): {
		if(step != m_stepColor)
			return false;
		m_colors.insert(m_colors.end(), arrAttribs.begin(), arrAttribs.end());
		break;
	}

	case(mbtNormal): {
		if(step != 3)
			return false;
		m_normals.insert(m_normals.end(), arrAttribs.begin(), arrAttribs.end());
		break;
	}

	case(mbtTexCoord): {
		if(step != m_stepTexCoord)
			return false;
		m_texCoords.insert(m_texCoords.end(), arrAttribs.begin(), arrAttribs.end());
		break;
        
    case(mbtFaceIndices):
        return false;
    case(mbtCount):
        return false;
	}
	}

	return true;
}

void Geometry::clearBuffer(MemoryBufferType btype) {
	switch(btype) {
	case(mbtPosition):
		m_vertices.resize(0);
	break;

	case(mbtColor):
		m_colors.resize(0);
	break;

	case(mbtNormal):
		m_normals.resize(0);
	break;

	case(mbtTexCoord):
		m_texCoords.resize(0);
	break;

    case(mbtFaceIndices):
    	m_indices.resize(0);
    break;
    case(mbtCount):
    break;
	}
}

void Geometry::addPerVertexColor(const vec4f& color, U32 ctVertices) {

	if(ctVertices == 0)
		ctVertices = countVertices();

	vector<float> colors;
	colors.reserve(ctVertices * m_stepColor);
	for(U32 i=0; i < ctVertices; i++) {
		colors.push_back(color.x);
		colors.push_back(color.y);
		colors.push_back(color.z);

		if(m_stepColor == 4)
			colors.push_back(color.w);
	}

	m_colors.insert(m_colors.end(), colors.begin(), colors.end());
}

bool Geometry::addFaceIndices(const vector<U32>& arrIndex, PS::GL::FaceType faceMode) {
	if(m_faceMode != faceMode)
		return false;

	m_indices.insert(m_indices.end(), arrIndex.begin(), arrIndex.end());
	return true;
}

//Operators
bool Geometry::copyFrom(const Geometry& other) {
	m_stepVertex = other.m_stepVertex;
	m_stepColor = other.m_stepColor;
	m_stepTexCoord = other.m_stepTexCoord;
	m_stepFace = other.m_stepFace;
	m_faceMode = other.m_faceMode;

	m_vertices.assign(other.m_vertices.begin(), other.m_vertices.end());
	m_normals.assign(other.m_normals.begin(), other.m_normals.end());
	m_colors.assign(other.m_colors.begin(), other.m_colors.end());
	m_texCoords.assign(other.m_texCoords.begin(), other.m_texCoords.end());
	m_indices.assign(other.m_indices.begin(), other.m_indices.end());
	m_vFlags.assign(other.m_vFlags.begin(), other.m_vFlags.end());

	return true;
}

bool Geometry::appendFrom(const Geometry& other) {
	if(!isCompatible(other)) {
		LogError("The right mesh is not compatible with the left mesh to merge.");
		return false;
	}

	//offset to be added to indices of the other mesh
	U32 ctVertexOffset = this->countVertices();

	if(other.countVertices() > 0)
		m_vertices.insert(m_vertices.end(), other.m_vertices.begin(), other.m_vertices.end());

	if(other.countColor() > 0)
		m_colors.insert(m_colors.end(), other.m_colors.begin(), other.m_colors.end());

	if(other.countNormals() > 0)
		m_normals.insert(m_normals.end(), other.m_normals.begin(), other.m_normals.end());

	if(other.countTexCoords() > 0)
		m_texCoords.insert(m_texCoords.end(), other.m_texCoords.begin(), other.m_texCoords.end());

	//Copy vFlags
	m_vFlags.insert(m_vFlags.end(), other.m_vFlags.begin(), other.m_vFlags.end());

	for(U32 i=0; i < other.m_indices.size(); i++)
		m_indices.push_back(other.m_indices[i] + ctVertexOffset);


	return false;
}

bool Geometry::read(const AnsiStr& strFilePath) {
    AnsiStr strExt = ExtractFileExt(strFilePath).toLower();
    if(strExt == "obj")
        return readObj(strFilePath);
    return false;
}

bool Geometry::write(const AnsiStr& strFilePath) const {
    AnsiStr strExt = ExtractFileExt(strFilePath).toLower();
    if(strExt == "obj")
        return writeObj(strFilePath);
    return false;
}

bool Geometry::readObj(const AnsiStr& strFilePath) {
	Mesh mesh(strFilePath.cptr());
	MeshNode* node = mesh.getNode(0);

	if(node == NULL)
		return false;

	int unitFace = node->getUnitFace();
	if(unitFace != 3 && unitFace != 4)
		return false;

	PS::GL::FaceType ftype = ftTriangles;
	if(node->getUnitFace() == 4)
		ftype = ftQuads;

	//read mesh info
	init(node->getUnitVertex(), node->getUnitColor(), node->getUnitTexCoord(), ftype);
	addVertexAttribs(node->vertices(), node->getUnitVertex(), mbtPosition);
	addVertexAttribs(node->normals(), 3, mbtNormal);
	addVertexAttribs(node->colors(), node->getUnitColor(), mbtColor);
	addVertexAttribs(node->texcoords(), node->getUnitTexCoord(), mbtTexCoord);
	addFaceIndices(node->faceElements(), ftype);

	return true;
}

bool Geometry::writeObj(const AnsiStr& strFilePath) const {

	MeshNode* pnode = new MeshNode();
	pnode->setVertexAttrib(m_vertices, mbtPosition, m_stepVertex);
	pnode->setVertexAttrib(m_normals, mbtNormal, 3);
	pnode->setVertexAttrib(m_colors, mbtColor, m_stepColor);
	pnode->setVertexAttrib(m_texCoords, mbtTexCoord, m_stepTexCoord);
	pnode->setFaceIndices(m_indices, m_stepFace);


	//store mesh
	Mesh* pmesh = new Mesh();
	pmesh->addNode(pnode);
	bool bres = pmesh->store(strFilePath.cptr());
	SAFE_DELETE(pmesh);

	return bres;
}

Geometry& Geometry::operator=(const Geometry& other) {
	this->copyFrom(other);
	return *this;
}

Geometry Geometry::operator+(const Geometry& other) const {
	Geometry result;
	result.copyFrom(const_cast<const Geometry&>(*this));
	result.appendFrom(other);
	return result;
}


bool Geometry::computeNormalsFromFaces()
{
	if(m_stepFace != 3)
		return false;

	//Per each vertex we need to compute all the neighboring faces
	vec3u32 face;
	vec3f v[3];
	vec3f n[3];
	vec3f nf;
	U32 ctFaces = countFaces();
	m_normals.resize(m_vertices.size());
	std::fill(m_normals.begin(), m_normals.end(), 0.0f);
	for(U32 i=0; i<ctFaces; i++) {
		face.load(&m_indices[i*3]);
		v[0] = vertexAt(face.x);
		v[1] = vertexAt(face.y);
		v[2] = vertexAt(face.z);

		//cross(b-a, c-a)
		//Face Normal
		nf = vec3f::cross(v[1] - v[0], v[2] - v[0]);
		n[0] = normalAt(face.x) + nf;
		n[1] = normalAt(face.y) + nf;
		n[2] = normalAt(face.z) + nf;

		n[0].store(&m_normals[face.x*3]);
		n[1].store(&m_normals[face.y*3]);
		n[2].store(&m_normals[face.z*3]);
	}

	//Normalize all normals
	U32 ctVertices = this->countVertices();
	for(U32 i=0; i<ctVertices; i++) {
		nf = normalAt(i);
		nf.normalize();
		nf.store(&m_normals[i*3]);
	}

	return true;
}

    bool Geometry::addLine(const vec3f& start, const vec3f& end) {
        if(m_faceMode >= ftTriangles) {
            LogError("The geometry is not setup for a line or point topology!");
            return false;
        }
        
        addVertex(start);
        addVertex(end);
        return true;
    }
    
    bool Geometry::addLines(const vector<vec3f>& vertices) {
        if(m_faceMode >= ftTriangles) {
            LogError("The geometry is not setup for a line or point topology!");
            return false;
        }
        
        for(U32 i=0; i<vertices.size(); i++)
            addVertex(vertices[i]);
        assert(checkIndices());
        return true;
    }
    
    bool Geometry::addCircle2D(int sectors,
                               float radius,
                               const vec3f& o) {
        if(m_faceMode >= ftTriangles) {
            LogError("The geometry is not setup for a line or point topology!");
            return false;
        }

        if(sectors <= 0 || radius < EPSILON)
            return false;
        
        //int idxStart = countVertices();
        float oneOverSector = 1.0f / static_cast<float>(sectors);
        for(int i=0; i<sectors; i++) {
            
            vec3f v = o + vec3f::mul(radius, vec3f(0.0f, cos(i * (TwoPi * oneOverSector)), sin(i * (TwoPi * oneOverSector))));
            this->addVertex(v);
            this->addNormal((v - o).normalized());
        }
        this->addVertex(o + vec3f(0.0f, radius, 0.0f));
        
        assert(checkIndices());
        return true;
    }

bool Geometry::addCircle3D(int sectors,
							float radius,
							const vec3f& o) {
    if(m_faceMode != ftTriangles) {
        LogError("The geometry is not setup for triangles topology!");
        return false;
    }

	if(sectors <= 0 || radius < EPSILON)
		return false;

	int idxStart = countVertices();
	this->addVertex(o);
	float oneOverSector = 1.0f / static_cast<float>(sectors);
	for(int i=0; i<sectors; i++) {
		this->addVertex(o + vec3f::mul(radius, vec3f(0.0f, cos(i * (TwoPi * oneOverSector)), sin(i * (TwoPi * oneOverSector)))));
		this->addTriangle(vec3u32(idxStart + i + 2, idxStart + i + 1, idxStart));
	}
	this->addVertex(o + vec3f(0.0f, radius, 0.0f));

	assert(checkIndices());
	return true;
}

bool Geometry::addCone(int sectors,
				 	 	  float radius,
				 	 	  float width,
				 	 	  const vec3f& o) {
	addVertex(o + vec3f(width, 0, 0));
	int idxStart = countVertices()-1;
	bool res = addCircle3D(sectors, radius, o);
	if(!res)
		return false;

	int idxEnd = countVertices()-1;
	for(int i=idxStart+1; i < idxEnd; i++) {
		addTriangle(vec3u32(i+1, idxStart, i));
	}

	return true;
}
    
void Geometry::addCube(const vec3f& lower, const vec3f& upper) {
    float l = lower.x; float r = upper.x;
	float b = lower.y; float t = upper.y;
	float n = lower.z; float f = upper.z;
    
	float vertices [][3] = {{l, b, f}, {l, t, f}, {r, t, f},
        {r, b, f}, {l, b, n}, {l, t, n},
        {r, t, n}, {r, b, n}};
    
    
    U32 indices[24] = {1, 2, 3, 0, 7, 6, 5, 4,
        7, 4, 0, 3, 5, 6, 2, 1,
        6, 7, 3, 2, 1, 0, 4, 5};
    
    //Offset for triangles
    int idxStart = countVertices();
    
    //Add Vertices
    vec3f center = (lower + upper) * 0.5f;
    for(int i=0; i<8; i++) {
        vec3f v = vec3f(&vertices[i][0]);
        vec3f n = (v - center).normalized();
        addVertex(v);
        addNormal(n);
    }
    
    //Add Face Indices
    vec4u32 quad;
    if(m_faceMode == ftQuads) {
		for(int i=0; i<6; i++) {
			quad.load(&indices[i*4]);
			quad = quad + vec4u32(idxStart);
			quad = vec4u32(quad.w, quad.z, quad.y, quad.x);
			addQuad(quad);
		}
    }
    else
    {
		for(int i=0; i<6; i++) {
			quad.load(&indices[i*4]);
			quad = quad + vec4u32(idxStart);
			addTriangle(vec3u32(quad.x, quad.z, quad.y));
			addTriangle(vec3u32(quad.x, quad.w, quad.z));
		}
    }

}
    
    void Geometry::addCube(const vec3f& center, float side) {
        vec3f lo = center - vec3f(0.5 * side);
        vec3f hi = center + vec3f(0.5 * side);
        addCube(lo, hi);
    }
    
    void Geometry::addSphere(float radius, int hseg, int vseg) {
    	float vSegInv = 1.0f / (float)vseg;
    	float hSegInv = 1.0f / (float)hseg;
    	int idxStart = countVertices();

    	//Loop Over
    	for(int v=0; v<vseg+1; v++) {
    		float p = DEGTORAD((float)v * vSegInv * 180.0f);

    		for(int h=0; h<hseg; h++) {
    			float o = DEGTORAD((float)h * hSegInv * 360.0f);

    			vec3f v1;
    			v1.x = radius * sin(p) * sin(o);
    			v1.z = radius * sin(p) * cos(o);
    			v1.y = radius * cos(p);

    			vec3f n = v1.normalized();

    			//Add Vertex
    			addVertex(v1);
    			addNormal(n);
    		}
    	}

    	//Indices
    	int t[4];
    	for(int v=0; v<vseg; v++) {
    		for(int h=0; h<hseg; h++) {
    			t[0] = h + v*hseg;
    			t[1] = t[0] + 1;
    			t[2] = h + (v+1)*hseg;
    			t[3] = t[2] + 1;

    			if(h == hseg-1) {
    				t[1] = v*hseg;
    				t[3] = (v+1)*hseg;
    			}

    			//Triangles
    			addTriangle(vec3u32(idxStart) + vec3u32(t[0], t[2], t[1]));
    			addTriangle(vec3u32(idxStart) + vec3u32(t[1], t[2], t[3]));
    		}
    	}
    }

    void Geometry::addCylinder(float radius, float height, int sectors, bool base, bool roof) {
    	U32 idxStartBase = countVertices();
    	double oneOverSector = 1.0 / static_cast<double>(sectors);

    	vec3f origin(0.0f);


    	addVertex(origin);
    	addTexCoord(vec2f(0.0, 0.0));
    	U32 count = 1;
    	for(int i=0; i<sectors+1; i++) {
    		double angle = double(i) * TwoPi * oneOverSector;
    		vec3f v = vec3f::mul(radius, vec3f(cos(angle), 0.0, sin(angle)));

    		addVertex(v);
    		addTexCoord(vec2f( double(i) * oneOverSector, 0.0f));
    		count++;
    	}

    	U32 idxStartRoof = countVertices();
    	for(U32 i=idxStartBase; i < idxStartRoof; i++) {
    		addVertex(vertexAt(i) + vec3f(0, height, 0));
    		addTexCoord(texcoordAt(i) + vec2f(0.0f, 1.0f));
    	}

    	//triangulate base
    	if(base) {
    		for(U32 i = 1; i < count; i++) {
    			if(i == (count - 1))
    				addTriangle(vec3u32(idxStartBase, idxStartBase + i, idxStartBase + 1));
    			else
    				addTriangle(vec3u32(idxStartBase, idxStartBase + i, idxStartBase + i + 1));
    		}
    	}

    	if(roof) {
    		for(U32 i = 1; i < count; i++) {
    			if(i == (count - 1))
    				addTriangle(vec3u32(idxStartRoof, idxStartRoof + i, idxStartRoof + 1));
    			else
    				addTriangle(vec3u32(idxStartRoof, idxStartRoof + i, idxStartRoof + i + 1));
    		}
    	}


    	//add walls
    	for(U32 i = 1; i < count - 1; i++) {
			addTriangle(vec3u32(idxStartRoof + i, idxStartRoof + i + 1, idxStartBase + i));
			addTriangle(vec3u32(idxStartRoof + i + 1, idxStartBase + i + 1, idxStartBase + i));
    	}

    	computeNormalsFromFaces();
    }

    void Geometry::addTetrahedra(vec3f v[4]) {
    	int idxStart = countVertices();

    	for(int i=0; i<4; i++)
    		addVertex(v[i]);

		float det = vec3f::dot(v[1] - v[0], vec3f::cross(v[2] - v[0], v[3] - v[0]));
		if (det >= 0) {
			addTriangle(vec3u32(1, 2, 3) + vec3u32(idxStart));
			addTriangle(vec3u32(2, 0, 3) + vec3u32(idxStart));
			addTriangle(vec3u32(3, 0, 1) + vec3u32(idxStart));
			addTriangle(vec3u32(1, 0, 2) + vec3u32(idxStart));
		} else {
			addTriangle(vec3u32(3, 2, 1) + vec3u32(idxStart));
			addTriangle(vec3u32(3, 0, 2) + vec3u32(idxStart));
			addTriangle(vec3u32(1, 0, 3) + vec3u32(idxStart));
			addTriangle(vec3u32(2, 0, 1) + vec3u32(idxStart));
		}
    }

    void Geometry::addTetrahedra(const vector<float>& vertices, const vector<U32>& tets) {
    	int idxStart = countVertices();

    	U32 ctVertices = vertices.size() / 3;
    	U32 ctTets = tets.size() / 4;
    	addVertexAttribs(vertices, 3, mbtPosition);

    	vec3f v[4];
    	for(U32 i=0; i<ctTets; i++) {

    		vec4u32 e = vec4u32(&tets[i*4]);
    		v[0] = vec3f(&vertices[e.x * 3]);
    		v[1] = vec3f(&vertices[e.y * 3]);
    		v[2] = vec3f(&vertices[e.z * 3]);
    		v[3] = vec3f(&vertices[e.w * 3]);

    		e = e + vec4u32(idxStart);

    		float det = vec3f::dot(v[1] - v[0], vec3f::cross(v[2] - v[0], v[3] - v[0]));
    		if(det >= 0) {
    			addTriangle(vec3u32(e[1], e[2], e[3]));
    			addTriangle(vec3u32(e[2], e[0], e[3]));
    			addTriangle(vec3u32(e[3], e[0], e[1]));
    			addTriangle(vec3u32(e[1], e[0], e[2]));
    		} else {
    			addTriangle(vec3u32(e[3], e[2], e[1]));
    			addTriangle(vec3u32(e[3], e[0], e[2]));
    			addTriangle(vec3u32(e[1], e[0], e[3]));
    			addTriangle(vec3u32(e[2], e[0], e[1]));
    		}
    	}
    }

    bool Geometry::addRingStripAroundXAxis(int sectors,
    									   float radius,
										   float thickness,
										   const vec3f& origin) {
        if(sectors <= 0 || radius < thickness)
            return false;

        int idxStart = countVertices();
    	float oneOverSector = 1.0f / static_cast<float>(sectors);
    	float halfThickness = 0.5 * thickness;

    	for(int i=0; i<sectors; i++) {

    		float vcos = radius * cos(i * (TwoPi * oneOverSector));
    		float vsin = radius * sin(i * (TwoPi * oneOverSector));
    		vec3f v1 = origin + vec3f(0.0, vcos, vsin);
    		vec3f v2 = origin + vec3f(thickness, vcos, vsin);

    		//surface
    		addVertex(v1);
    		addVertex(v2);

    		if(i > 0) {
    			int base = idxStart + (i - 1) * 2;
    			addTriangle(vec3u32(base + 0, base + 1, base + 2));
    			addTriangle(vec3u32(base + 2, base + 1, base + 3));
    		}
    	}

    	//add triangles
    	int lastQuad[4];
    	lastQuad[0] = idxStart + (sectors - 1) * 2;
    	lastQuad[1] = idxStart + (sectors - 1) * 2 + 1;
    	lastQuad[2] = idxStart;
    	lastQuad[3] = idxStart + 1;
		addTriangle(vec3u32(lastQuad[0], lastQuad[1], lastQuad[2]));
		addTriangle(vec3u32(lastQuad[2], lastQuad[1], lastQuad[3]));

        return true;
    }
    
    bool Geometry::addDisc(int sectors,
                           int xsections,
                           float radius,
                           float thickness,
                           const vec3f& o) {
        if(sectors <= 0 || radius < EPSILON)
            return false;
        
        int idxStart = countVertices();
        this->addVertex(o);
        float oneOverSector = 1.0f / static_cast<float>(sectors);
        for(int i=0; i<sectors; i++) {
            this->addVertex(o + vec3f::mul(radius, vec3f(0.0f, cos(i * (TwoPi * oneOverSector)), sin(i * (TwoPi * oneOverSector)))));
            this->addTriangle(vec3u32(idxStart + i + 2, idxStart + i + 1, idxStart));
        }
        this->addVertex(o + vec3f(0.0f, radius, 0.0f));
        
        assert(checkIndices());

        
        return false;
    }


bool Geometry::checkIndices() const {
	U32 count = countVertices();
	for(U32 i=0; i<m_indices.size(); i++) {
		if(m_indices[i] >= count)
			return false;
	}

	return true;
}

    AABB Geometry::aabb() const {
        vec3f lo(FLT_MAX);
        vec3f hi(FLT_MIN);
        vec3f p;
        for(int i=0; i < countVertices(); i++) {
            p = vertexAt(i);
            lo = vec3f::minP(lo, p);
            hi = vec3f::maxP(hi, p);
        }
        
        return AABB(lo, hi);
    }
}
}

