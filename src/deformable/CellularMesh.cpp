/*
 * WingedEdgeTetMesh.cpp
 *
 *  Created on: May 6, 2014
 *      Author: pourya
 */

#include <base/FileDirectory.h>
#include <base/Logger.h>
#include <base/MathBase.h>
#include <base/String.h>
#include <base/StringBase.h>
#include <base/Vec.h>
#include <deformable/CellularMesh.h>
#include <graphics/AABB.h>
#include <GL/glew.h>
//#include "graphics/selectgl.h"
//#include <algorithm>
#include <cassert>
//#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iterator>
#include <map>
#include <utility>
#include <vector>

using namespace std;
using namespace PS;
using namespace PS::FILESTRINGUTILS;

using namespace PS;
using namespace PS::MESH;

CellMesh::CellMesh() {
	init();
}

CellMesh::CellMesh(U32 ctVertices, double* vertices, U32 ctElements, U32* elements) {
	init();
	setup(ctVertices, vertices, ctElements, elements);
}

CellMesh::CellMesh(const CellMesh& other) {
	init();

	vector<double> vertices;
	vector<U32> elements;

	U32 ctNodes = other.countNodes();
	U32 ctElements = other.countElements();
	vertices.resize(ctNodes * 3);
	elements.resize(ctElements * 4);

	//nodes
	for(U32 i=0; i<ctNodes; i++) {
		NODE node = other.const_nodeAt(i);
		vertices[i * 3 + 0] = node.pos.x;
		vertices[i * 3 + 1] = node.pos.y;
		vertices[i * 3 + 2] = node.pos.z;
	}

	//elements
	for(U32 i=0; i<ctElements; i++) {
		CELL elem = other.const_elemAt(i);
		elements[i * 4 + 0] = elem.nodes[0];
		elements[i * 4 + 1] = elem.nodes[1];
		elements[i * 4 + 2] = elem.nodes[2];
		elements[i * 4 + 3] = elem.nodes[3];
	}

	setup(vertices, elements);
}

CellMesh::CellMesh(const vector<double>& vertices, const vector<U32>& elements) {
	init();
	setup(vertices, elements);
}


CellMesh::~CellMesh() {
	cleanup();
}


CellMesh* CellMesh::CreateOneTet() {
	vector<double> vertices;
	vector<U32> elements;

	vec3d points[4];
	points[0] = vec3d(-1, 0, 0);
	points[1] = vec3d(0, 0, -2);
	points[2] = vec3d(1, 0, 0);
	points[3] = vec3d(0, 2, -1);

	vertices.resize(4 * 3);
	for (int i = 0; i < 4; i++) {
		points[i].store(&vertices[i * 3]);
	}

	elements.resize(4);
	elements[0] = 0;
	elements[1] = 1;
	elements[2] = 2;
	elements[3] = 3;

	CellMesh* tet = new CellMesh(vertices, elements);
	return tet;
}

void CellMesh::init() {
	m_elemToShow = INVALID_INDEX;
	m_fOnNodeEvent = NULL;
	m_fOnEdgeEvent = NULL;
	m_fOnFaceEvent = NULL;
	m_fOnElementEvent = NULL;
}

void CellMesh::setOnNodeEventCallback(OnNodeEvent f) {
	m_fOnNodeEvent = f;
}

void CellMesh::setOnEdgeEventCallback(OnEdgeEvent f) {
	m_fOnEdgeEvent = f;
}

void CellMesh::setOnFaceEventCallback(OnFaceEvent f) {
	m_fOnFaceEvent = f;
}

void CellMesh::setOnElemEventCallback(OnElementEvent f) {
	m_fOnElementEvent = f;
}


bool CellMesh::setup(const vector<double>& vertices, const vector<U32>& elements) {
	U32 ctVertices = vertices.size() / 3;
	U32 ctElements = elements.size() / 4;

	return setup(ctVertices, &vertices[0], ctElements, &elements[0]);
}

bool CellMesh::setup(U32 ctVertices, const double* vertices, U32 ctElements, const U32* elements) {

	//cleanup to setup the mesh
	cleanup();

	//add all vertices first
	for(U32 i=0; i<ctVertices; i++) {
		NODE node;
		node.pos = node.restpos = vec3d(&vertices[i*3]);
		node.outHE = BaseHandle::INVALID;
		m_vNodes.push_back(node);
	}


	//insert element
	for(U32 i=0; i<ctElements; i++)
		insert_element(const_cast<U32 *>(&elements[i * 4]));


	//Compute AABB
	computeAABB();
	//checkMeshFaceDirections();
	this->printInfo();

	return true;
}

void CellMesh::cleanup() {
	m_vElements.resize(0);
	m_vFaces.resize(0);
	m_vHalfEdges.resize(0);
	m_vNodes.resize(0);
	m_mapFaces.clear();
	m_mapHalfEdgesIndex.clear();
}

void CellMesh::printInfo() const {
	//print all nodes
	printf("NODES #%u\n", (U32)m_vNodes.size());
	for(U32 i=0; i < m_vNodes.size(); i++) {
		NODE n = m_vNodes[i];

		printf("NODE %u, outgoing edge: %d, pos: [%.3f, %.3f, %.3f]\n", i, n.outHE, n.pos.x, n.pos.y, n.pos.z);
	}
	printf("\n");

	//print all edges
	printf("HALFEDGES #%u\n", (U32)m_vHalfEdges.size());
	for(U32 i=0; i < m_vHalfEdges.size(); i++) {
		HEDGE e = m_vHalfEdges[i];

		printf("HEDGE %u, from: %d, to %d, next: %d, prev: %d, opposite: %d, face: %d\n", i, e.from, e.to, e.next, e.prev, e.opposite, e.face);
	}
	printf("\n");


	//print all face
	printf("FACES #%u\n", (U32)m_vFaces.size());
	for(U32 i=0; i < m_vFaces.size(); i++) {
		FACE face = m_vFaces[i];

		U32 vhandles[3];
		vhandles[0] = vertex_from_hedge(face.halfedge[0]);
		vhandles[1] = vertex_from_hedge(face.halfedge[1]);
		vhandles[2] = vertex_from_hedge(face.halfedge[2]);

		printf("FACE %u, Nodes [%d, %d, %d], HalfEdges [%d, %d, %d]\n", i,
				vhandles[0], vhandles[1], vhandles[2],
				face.halfedge[0], face.halfedge[1], face.halfedge[2]);
	}
	printf("\n");

	//print all elements
	printf("ELEMS #%u\n", (U32)m_vElements.size());
	for(U32 i=0; i < m_vElements.size(); i++) {
		CELL elem = m_vElements[i];
		printf("ELEM %d, NODE: [%u, %u, %u, %u], ", i ,elem.nodes[0], elem.nodes[1], elem.nodes[2], elem.nodes[3]);
		printf("FACE: [%u, %u, %u, %u], ", elem.faces[0], elem.faces[1], elem.faces[2], elem.faces[3]);

		//print edges
		printf("HalfEdges: [");
		for(int e=0; e<6; e++) {
			if(e < 5)
				printf("%u, ", elem.halfedge[e]);
			else
				printf("%u]\n", elem.halfedge[e]);
		}
	}
	printf("\n");

}

double CellMesh::computeDeterminant(U32 idxNodes[4]) const {
	vec3d v[4];

	for(int i=0; i<4; i++)
		v[i] = const_nodeAt(i).pos;
	return ComputeElementDeterminant(v);
}

double CellMesh::ComputeElementDeterminant(const vec3d v[4]) {
	return	vec3d::dot(v[1] - v[0], vec3d::cross(v[2] - v[0], v[3] - v[0]));
}

//add/remove
bool CellMesh::insert_element(const CELL& e) {

	//check the structure before adding it
	for(int i=0; i<4; i++) {
		if(!isFaceIndex(e.faces[i])) {
			LogErrorArg2("Unable to add element. Invalid face index found at: f[%d] = %d", i, e.faces[i]);
			return false;
		}
	}

	for(int i=0; i<4; i++) {
		if(!isNodeIndex(e.nodes[i])) {
			LogErrorArg2("Unable to add element. Invalid node index found at: n[%d] = %d", i, e.nodes[i]);
			return false;
		}
	}

	for(int i=0; i<6; i++) {
		if(!isHalfEdgeIndex(e.halfedge[i])) {
			LogErrorArg2("Unable to add element. Invalid halfedge index found at: he[%d] = %d", i, e.halfedge[i]);
			return false;
		}
	}

	//bump up refs for faces
	for(int i=0; i<4; i++) {
		m_vFaces[ e.faces[i] ].refs ++;
		m_vFaces[ e.faces[i] ].removed = false;
	}

	m_vElements.push_back(e);
	if(m_fOnElementEvent)
		m_fOnElementEvent(e, m_vElements.size() - 1, teAdded);

	return true;
}

bool CellMesh::insert_element(U32 nodes[4]) {
	//Inserts a tetrahedra element to the halfedged mesh structure
	for(int i=0; i<4; i++) {
		if(!isNodeIndex(nodes[i])) {
			LogErrorArg1("Invalid node index passed in. %u", nodes[i]);
			return false;
		}
	}

	//face mask
	const int faceMaskPos[4][3] = { {1, 2, 3}, {2, 0, 3}, {3, 0, 1}, {1, 0, 2} };
	const int faceMaskNeg[4][3] = { {3, 2, 1}, {3, 0, 2}, {1, 0, 3}, {2, 0, 1} };

	//edge mask
	const int edgeMaskPos[6][2] = { {1, 2}, {2, 3}, {3, 1}, {2, 0}, {0, 3}, {0, 1} };
	//const int edgeMaskNeg[6][2] = { {3, 2}, {2, 1}, {1, 3}, {3, 0}, {0, 2}, {1, 0} };

	//maps for half edges
	std::map< HEdgeKey, HEDGE > mapHalfEdges;
	typedef std::map< HEdgeKey, HEDGE >::iterator HEITER;


	//determinant
	double det = computeDeterminant(nodes);

	//Add element nodes set only for now
	CELL elem;
	elem.posDet = (det >= 0);
	for(int i=0; i<4; i++)
		elem.nodes[i] = nodes[i];
	for(int i=0; i<4; i++)
		elem.faces[i] = INVALID_INDEX;
	for(int i=0; i<6; i++)
		elem.halfedge[i] = INVALID_INDEX;

	//Loop over faces. Per each tet 6 edges or 12 half-edges added
	for (int f = 0; f < 4; f++) {
		U32 fv[3];

		if (det >= 0) {
			fv[0] = nodes[faceMaskPos[f][0]];
			fv[1] = nodes[faceMaskPos[f][1]];
			fv[2] = nodes[faceMaskPos[f][2]];
		} else {
			fv[0] = nodes[faceMaskNeg[f][0]];
			fv[1] = nodes[faceMaskNeg[f][1]];
			fv[2] = nodes[faceMaskNeg[f][2]];
		}

		//Add all halfedges for this face
		for (int e = 0; e < 3; e++) {
			U32 from = fv[e];
			U32 to = fv[(e + 1) % 3];

			if(halfedge_exists(from, to) == false) {
				HEdgeKey key(from, to);
				HEDGE he;
				he.from = from;
				he.to = to;
				mapHalfEdges.insert(make_pair(key, he));
			}
			else {
				if(!halfedge_exists(to, from)) {
					LogErrorArg2("Opposite half edge not found for: <%d. %d>", from, to);
				}
			}
		}
	}

	//add final half edges in the order of (u, v) and (v, u)
	U32 from, to = INVALID_INDEX;
	U32 idxForward, idxBackward = INVALID_INDEX;

	while(mapHalfEdges.size() > 0) {
		//add forward
		HEITER forward = mapHalfEdges.begin();
		from = forward->second.from;
		to = forward->second.to;

		m_vHalfEdges.push_back(forward->second);
		idxForward = m_vHalfEdges.size() - 1;

		//insert the forward halfedge into map
		insertHEdgeIndexToMap(from, to, idxForward);

		//Remove from map
		mapHalfEdges.erase(forward);

		//add backward
		HEdgeKey keyBackward(to, from);
		HEITER backward = mapHalfEdges.find(keyBackward);
		if(backward != mapHalfEdges.end()) {
			m_vHalfEdges.push_back(backward->second);
			idxBackward = idxForward + 1;

			//insert the backward halfedge into map
			insertHEdgeIndexToMap(to, from, idxBackward);

			//Remove from map
			mapHalfEdges.erase(backward);

			//Set opposite indices
			m_vHalfEdges[idxForward].opposite = idxBackward;
			m_vHalfEdges[idxBackward].opposite = idxForward;
		}
		else {
			LogErrorArg2("Opposite edge not found for: <%d. %d>", from, to);
			return false;
		}
	}

	//add all faces now and finish setting information for all
	//loop over faces. Per each tet 6 edges or 12 half-edges added
	for (int f = 0; f < 4; f++) {
		U32 facenodes[3];

		if (det >= 0) {
			facenodes[0] = nodes[faceMaskPos[f][0]];
			facenodes[1] = nodes[faceMaskPos[f][1]];
			facenodes[2] = nodes[faceMaskPos[f][2]];
		} else {
			facenodes[0] = nodes[faceMaskNeg[f][0]];
			facenodes[1] = nodes[faceMaskNeg[f][1]];
			facenodes[2] = nodes[faceMaskNeg[f][2]];
		}

		//Set element face index
		elem.faces[f] = insert_face(facenodes);

		//test if the face key maps to index
		FaceKey key(facenodes[0], facenodes[1], facenodes[2]);
		MAPFACEITER it = m_mapFaces.find(key);
		if(it == m_mapFaces.end()) {
			LogError("unable to find the face via its indices!");
		}
		assert(it->second == elem.faces[f]);
	}

	//Set element half-edges
	for (int e = 0; e < 6; e++) {
		from = elem.nodes[edgeMaskPos[e][0]];
		to = elem.nodes[edgeMaskPos[e][1]];

		HEdgeKey key(from, to);
		MAPHEDGEINDEXITER it = m_mapHalfEdgesIndex.find(key);
		if (it != m_mapHalfEdgesIndex.end())
			elem.halfedge[e] = it->second;
		else {
			LogErrorArg2("Setting element halfedges failed! Unable to find edge <%d, %d>",
						from, to);
			return false;
		}
	}

	//Set outgoing edges for nodes
	for(U32 i=0; i < m_vHalfEdges.size(); i++) {
		HEDGE e = m_vHalfEdges[i];

		//set the outHalfEdge for all nodes in this edge
		if(m_vNodes[e.from].outHE == BaseHandle::INVALID)
			m_vNodes[e.from].outHE = i;
	}

	//call simple insert now
	return insert_element(elem);
}

void CellMesh::remove_element(U32 i) {
	assert(isElemIndex(i));

	//bump down refs for faces
	const CELL tet = const_elemAt(i);
	for(int j=0; j<4; j++) {
		m_vFaces[ tet.faces[j] ].refs --;

		//call face remove to decrement its refs
		remove_face( tet.faces[j] );
	}

	//mark element as removed then garbage collection will handle indices
	m_vElements[i].removed = true;
}

//inserting a face uniquely
U32 CellMesh::insert_face(U32 nodes[3]) {

	//first check if the face is already available
	FaceKey key(nodes[0], nodes[1], nodes[2]);
	MAPFACEITER fit = m_mapFaces.find(key);
	if(fit != m_mapFaces.end()) {
		return fit->second;
	}

	//since we do not have that face we will add it
	FACE face;
	U32 from, to = BaseHandle::INVALID;

	//Add all half-edges for this face and increment half-edge refs
	for (int e = 0; e < 3; e++) {
		from = nodes[e];
		to = nodes[(e + 1) % 3];

		HEdgeKey key(from, to);
		MAPHEDGEINDEXITER it = m_mapHalfEdgesIndex.find(key);
		if (it != m_mapHalfEdgesIndex.end()) {
			m_vHalfEdges[it->second].refs ++;
			face.halfedge[e] = it->second;
		}
		else {
			LogErrorArg2("Setting face edges failed! Unable to find edge <%d, %d>",
						 from, to);

			return BaseHandle::INVALID;
		}
	}

	//add the face now
	m_vFaces.push_back(face);
	U32 idxFace = m_vFaces.size() - 1;
	m_mapFaces.insert(std::make_pair(key, idxFace));

	//set prev, next, face for half edges
	for (int e = 0; e < 3; e++) {
		if (e == 0)
			m_vHalfEdges[face.halfedge[e]].prev = face.halfedge[2];
		else
			m_vHalfEdges[face.halfedge[e]].prev = face.halfedge[e - 1];

		m_vHalfEdges[face.halfedge[e]].next = face.halfedge[(e + 1) % 3];
		m_vHalfEdges[face.halfedge[e]].face = idxFace;
	}

	return idxFace;
}

U32 CellMesh::insert_node(const NODE& n) {
	m_vNodes.push_back(n);
	return m_vNodes.size() - 1;
}

void CellMesh::remove_face(U32 i) {
	assert(isFaceIndex(i));

	//bump down refs for half-edges
	const FACE face = const_faceAt(i);
	for(int j=0; j < 3; j++) {
		m_vHalfEdges[ face.halfedge[j] ].refs --;
	}

	//mark face as removed
	m_vFaces[i].removed = true;
}


void CellMesh::garbage_collection() {

	//acquire lock to mesh
	tst_keys();

	//elements
	U32 i = 0;
	U32 ctRemovedElements = 0;
	while(i < countElements()) {
		CELL tet = elemAt(i);
		if(tet.removed) {
			m_vElements.erase(m_vElements.begin() + i);
			ctRemovedElements++;
		}
		else
			i++;
	}

	//faces

	//scan faces and extract correct indices to apply after delete
	vector<U32> vFaceIndexAfter;
	vFaceIndexAfter.resize(countFaces());
	U32 ctCorrectFaces = 0;
	for(i = 0; i < countFaces(); i++) {

		vFaceIndexAfter[i] = ctCorrectFaces;
		const FACE face = const_faceAt(i);

		if(face.removed && (face.refs == 0))
		{
			//remove face from the keys
			U32 nodes[3];
			nodes[0] = vertex_from_hedge(face.halfedge[0]);
			nodes[1] = vertex_from_hedge(face.halfedge[1]);
			nodes[2] = vertex_from_hedge(face.halfedge[2]);
			FaceKey key(nodes[0], nodes[1], nodes[2]);
			MAPFACEITER it = m_mapFaces.find(key);
			if(it == m_mapFaces.end()) {
				printf("No key recorded for face with nodes [%u, %u, %u]\n", nodes[0], nodes[1], nodes[2]);
			}
			else {
				if(it->second == i) {
					printf("Found! Erasing the key for face with nodes [%u, %u, %u]\n", nodes[0], nodes[1], nodes[2]);
					m_mapFaces.erase(key);
				}
				else {
					printf("KEYERROR: the key for face with nodes [%u, %u, %u] does not match its index: %u, found: %u\n",
							nodes[0], nodes[1], nodes[2], i, it->second);
				}
			}
		}
		else
			ctCorrectFaces++;
	}

	//apply correct face indices to Half Edges
	for(i = 0; i < countHalfEdges(); i++) {
		U32 temp = m_vHalfEdges[i].face;
		m_vHalfEdges[i].face = vFaceIndexAfter[temp];
	}

	//apply correct face indices to elements
	for(i = 0; i < countElements(); i++) {
		for(U32 j = 0; j < 4; j++) {
			U32 temp = m_vElements[i].faces[j];
			m_vElements[i].faces[j] = vFaceIndexAfter[temp];
		}
	}


	//remove redundant faces now
	U32 ctRemovedFaces = 0;
	i = 0;
	while(i < countFaces()) {
		FACE face = const_faceAt(i);
		if(face.removed && (face.refs == 0)) {
//			printf("Erasing face with nodes [%u, %u, %u]\n", nodes[0], nodes[1], nodes[2]);

			m_vFaces.erase(m_vFaces.begin() + i);
			ctRemovedFaces++;
		}
		else
			i++;
	}


	//Remove redundant half-edges now
	U32 ctRemovedHalfEdges = 0;

	LogInfoArg3("garbage collection removed: Elements# %u, Faces# %u, HalfEdges# %u",
				ctRemovedElements, ctRemovedFaces, ctRemovedHalfEdges);

	//release lock
}

bool CellMesh::getFaceNodes(U32 idxFace, U32 (&nodes)[3]) const {
	if(!isFaceIndex(idxFace))
		return false;

	FACE face = const_faceAt(idxFace);
	for(int i=0; i < 3; i++)
		nodes[i] = vertex_from_hedge(face.halfedge[i]);

	return true;
}

CELL& CellMesh::elemAt(U32 i) {
	assert(isElemIndex(i));
	return m_vElements[i];
}

FACE& CellMesh::faceAt(U32 i) {
	assert(isFaceIndex(i));
	return m_vFaces[i];
}

HEDGE& CellMesh::halfedgeAt(U32 i) {
	assert(isHalfEdgeIndex(i));
	return m_vHalfEdges[i];
}

EDGE CellMesh::edgeAt(U32 i) const {
	assert(isEdgeIndex(i));
	EDGE e(const_halfedgeAt(i * 2), const_halfedgeAt(i * 2 + 1));
	return e;
}

NODE& CellMesh::nodeAt(U32 i) {
	assert(isNodeIndex(i));
	return m_vNodes[i];
}


//access
const CELL& CellMesh::const_elemAt(U32 i) const {
	assert(isElemIndex(i));
	return m_vElements[i];
}

const FACE& CellMesh::const_faceAt(U32 i) const {
	assert(isFaceIndex(i));
	return m_vFaces[i];
}

const HEDGE& CellMesh::const_halfedgeAt(U32 i) const {
	assert(isHalfEdgeIndex(i));
	return m_vHalfEdges[i];
}

const NODE& CellMesh::const_nodeAt(U32 i) const {
	assert(isNodeIndex(i));
	return m_vNodes[i];
}

void CellMesh::displace(double * u) {
	if(u == NULL)
		return;

	for(U32 i=0; i < countNodes(); i++) {
		m_vNodes[i].pos = m_vNodes[i].pos + vec3d(&u[i * 3]);
	}

	computeAABB();
}

bool CellMesh::insertHEdgeIndexToMap(U32 from, U32 to, U32 idxHE) {
	if(from == to) {
		LogErrorArg2("Can not register a half-edge with the same from and to nodes! [%u, %u]", from, to);
		return false;
	}

	if(!isHalfEdgeIndex(idxHE)) {
		LogErrorArg1("Can not register an invalid half-edge index: %u", idxHE);
		return false;
	}

	HEdgeKey key(from, to);
	m_mapHalfEdgesIndex.insert( std::make_pair( key, idxHE) );
	return true;
}

bool CellMesh::removeHEdgeIndexFromMap(U32 from, U32 to) {
	HEdgeKey key(from, to);
	MAPHEDGEINDEXITER it = m_mapHalfEdgesIndex.find(key);
	if(it == m_mapHalfEdgesIndex.end())
		return false;
	else {
		m_mapHalfEdgesIndex.erase(it);
		return true;
	}
}

bool CellMesh::insertFaceIndexToMap(U32 nodes[3], U32 idxFace) {

	if(!isFaceIndex(idxFace)) {
		LogErrorArg1("Can not register an invalid face index: %u", idxFace);
		return false;
	}

	FaceKey key(nodes[0], nodes[1], nodes[2]);
	m_mapFaces.insert( std::make_pair(key, idxFace));
	return true;
}

bool CellMesh::removeFaceIndexFromMap(U32 nodes[3]) {
	FaceKey key(nodes[0],  nodes[1], nodes[2]);
	MAPFACEITER it = m_mapFaces.find(key);
	if(it == m_mapFaces.end())
		return false;
	else {
		m_mapFaces.erase(it);
		return true;
	}
}

HEdgeKey CellMesh::computeHEdgeKey(U32 idxHEdge) const {
	assert(isHalfEdgeIndex(idxHEdge));

	const HEDGE& he = const_halfedgeAt(idxHEdge);
	HEdgeKey key(he.from, he.to);
	return key;
}

FaceKey CellMesh::computeFaceKey(U32 idxFace) const {
	assert(isFaceIndex(idxFace));

	U32 nodes[3];
	assert(getFaceNodes(idxFace, nodes));
	FaceKey key(nodes[0], nodes[1], nodes[2]);
	return key;
}


bool CellMesh::halfedge_exists(U32 from, U32 to) const {
	HEdgeKey key(from, to);
	return (m_mapHalfEdgesIndex.find( key ) != m_mapHalfEdgesIndex.end());
}

U32 CellMesh::halfedge_handle(U32 from, U32 to) {
	HEdgeKey key(from, to);
	MAPHEDGEINDEXITER it = m_mapHalfEdgesIndex.find(key);
	if(it != m_mapHalfEdgesIndex.end())
		return it->second;
	else
		return INVALID_INDEX;
}


bool CellMesh::cut_edge(int idxEdge, double distance, U32* poutIndexNP0, U32* poutIndexNP1) {
	if(!isEdgeIndex(idxEdge))
		return false;

	//Find halfedge indices from edge index
	U32 idxHE0 = halfedge_from_edge(idxEdge, 0);
	U32 idxHE1 = halfedge_from_edge(idxEdge, 1);

	HEDGE& he0 = halfedgeAt(idxHE0);
	HEDGE& he1 = halfedgeAt(idxHE1);

	U32 from = vertex_from_hedge(idxHE0);
	U32 to = vertex_to_hedge(idxHE0);

	NODE n0 = const_nodeAt(from);
	NODE n1 = const_nodeAt(to);

	//add two new point
	NODE np0;
	np0.pos = n0.pos + (n1.pos - n0.pos).normalized() * distance;
	np0.restpos = n0.restpos + (n1.restpos - n0.restpos).normalized() * distance;
	np0.outHE = idxHE1;
	U32 idxNP0 = insert_node(np0);

	NODE np1 = np0;
	np1.outHE = INVALID_INDEX;
	U32 idxNP1 = insert_node(np1);


	//Add new halfedges
	//half-edges are cut but before adding new faces we keep the face connectivity of the edges
	HEDGE he2 = he0;
	HEDGE he3 = he1;
	he2.from = idxNP1;
	he2.prev = idxHE0;

	he3.to 	= idxNP1;
	he3.next = idxHE1;

	m_vHalfEdges.push_back(he2);
	U32 idxHE2 = m_vHalfEdges.size() - 1;
	m_vHalfEdges.push_back(he3);
	U32 idxHE3 = m_vHalfEdges.size() - 1;

	//update opposites for new edges
	m_vHalfEdges[idxHE2].opposite = idxHE3;
	m_vHalfEdges[idxHE3].opposite = idxHE2;

	//insert new half edges and remove old ones
	insertHEdgeIndexToMap(he2.from, he2.to, idxHE2);
	insertHEdgeIndexToMap(he3.from, he3.to, idxHE3);

	//update outHE for new nodes
	m_vNodes[idxNP0].outHE = idxHE1;
	m_vNodes[idxNP1].outHE = idxHE2;

	//update old halfedges.
	//this should force face updates otherwise invalid faces will occur!
	//half-edges are cut but before adding new faces we keep the face connectivity of the edges

	//facemap: remove faces adjacent to halfedges 0 and 1
	U32 nodes[3];
	getFaceNodes(he0.face, nodes);
	removeFaceIndexFromMap(nodes);

	getFaceNodes(he1.face, nodes);
	removeFaceIndexFromMap(nodes);

	//hedgemap: remove halfedges 0 and 1
	removeHEdgeIndexFromMap(he0.from, he0.to);
	removeHEdgeIndexFromMap(he1.from, he1.to);

	he0.to = idxNP0;
	he0.next = idxHE2;
	he1.from = idxNP0;
	he1.prev = idxHE3;
	m_vHalfEdges[idxHE0] = he0;
	m_vHalfEdges[idxHE1] = he1;

	//hedgemap: insert new half edges
	insertHEdgeIndexToMap(he0.from, he0.to, idxHE0);
	insertHEdgeIndexToMap(he1.from, he1.to, idxHE1);

	//update faces adjacent to half-edges 0 and 1
	getFaceNodes(he0.face, nodes);
	insertFaceIndexToMap(nodes, he0.face);

	getFaceNodes(he1.face, nodes);
	insertFaceIndexToMap(nodes, he1.face);


	//Write output vertices
	if(poutIndexNP0)
		*poutIndexNP0 = idxNP0;

	if(poutIndexNP1)
		*poutIndexNP1 = idxNP1;

	return true;
}

int CellMesh::getFirstRing(int idxNode, vector<U32>& ringNodes) const {
	U32 he = m_vNodes[idxNode].outHE;
	ringNodes.resize(0);

	U32 node = INVALID_INDEX;
	U32 orgNode = vertex_to_hedge(he);
	while(node != orgNode) {
		he = next_hedge(opposite_hedge(he));
		node = vertex_to_hedge(he);
		ringNodes.push_back(node);
	}

	return (int)ringNodes.size();
}

int CellMesh::getIncomingHalfEdges(int idxNode, vector<U32>& incomingHE) const {
	assert(isNodeIndex(idxNode));

	NODE n = const_nodeAt(idxNode);
	U32 inHE_orig = opposite_hedge(n.outHE);
	U32 inHE = inHE_orig;
	U32 count = 0;
	do {
		if(isHalfEdgeIndex(inHE))
			incomingHE.push_back(inHE);
		else {
			LogError("Not a valid halfedge index");
			break;
		}
		inHE = opposite_hedge(next_hedge(inHE));

		count++;
		if(count > 10) {
			LogWarningArg3("More than 10 incoming halfedges detected! Node: %u, original hedge: %u, current hedge: %u",
						   idxNode, inHE_orig, inHE);
		}
	} while(inHE != inHE_orig);

	return (int)incomingHE.size();
}

int CellMesh::getOutgoingHalfEdges(int idxNode, vector<U32>& outgoingHE) const {
	assert(isNodeIndex(idxNode));

	NODE n = const_nodeAt(idxNode);
	U32 outHE_orig = n.outHE;
	U32 outHE = outHE_orig;
	U32 count = 0;

	do {
		if(isHalfEdgeIndex(outHE))
			outgoingHE.push_back(outHE);
		else {
			LogError("Not a valid halfedge index");
			break;
		}

		outHE = next_hedge(opposite_hedge(outHE));

		count++;
		if(count > 10) {
			LogWarningArg3("More than 10 outgoing halfedges detected! Node: %u, original hedge: %u, current hedge: %u",
						   idxNode, outHE_orig, outHE);
		}

	} while(outHE != outHE_orig);

	return (int)outgoingHE.size();
}

void CellMesh::setElemToShow(U32 elem) {
	if(elem == m_elemToShow)
		return;
	LogInfoArg1("Requested element to shows: %u\n", elem);
	m_elemToShow = elem;
}


bool CellMesh::readVegaFormat(const AnsiStr& strFP) {
	if(!FileExists(strFP))
		return false;
	char chrLine[1024];
	ifstream fpIn(strFP.cptr());
	if(!fpIn.is_open())
		return false;

	vector<double> vertices;
	vector<U32> elements;

	U32 ctLine = 0;
	U32 idxVertex = 0;
	U32 idxElements = 0;

	enum READMODE {modeReadHeader, modeReadVertex, modeReadElements};

	READMODE mode = modeReadHeader;
	READMODE prevMode = mode;

	while(!fpIn.eof())
	{
		fpIn.getline(chrLine, 1024);

		AnsiStr strLine(chrLine);
		strLine.removeStartEndSpaces();
		strLine.trim();
		if(strLine.firstChar() == '#')
			continue;
		if(strLine.firstChar() == '*') {
			if(strLine == "*VERTICES")
				mode = modeReadVertex;
			else if(strLine == "*ELEMENTS")
				mode = modeReadElements;
			continue;
		}

		if(strLine.length() == 0)
			break;

		if(prevMode != mode) {
			if(mode == modeReadVertex && idxVertex == 0)
			{
				int ctVertices;
				int ctCoords;
				sscanf(strLine.cptr(), "%d %d", &ctVertices, &ctCoords);
				if(ctVertices <= 0 || ctCoords != 3) {
					fpIn.close();
					return false;
				}

				vertices.reserve(ctVertices * 3);
			}
			else if(mode == modeReadElements && idxElements == 0) {
				int ctElems;
				int ctPerElemNodes;
				sscanf(strLine.cptr(), "%d %d", &ctElems, &ctPerElemNodes);
				if(ctElems <= 0 || ctPerElemNodes != 4) {
					fpIn.close();
					return false;
				}

				elements.reserve(ctElems * 4);
			}
		}


		if(mode == modeReadVertex)
		{
			vec3d v;
			vector<AnsiStr> arrWords;
			int ctWords = strLine.decompose(' ', arrWords);
			if(ctWords >= 4)
			{
				//int temp = atoi(arrWords[0].cptr());
				v.x = atof(arrWords[1].cptr());
				v.y = atof(arrWords[2].cptr());
				v.z = atof(arrWords[3].cptr());
			}

			//add vertex to the array
			vertices.push_back(v.x);
			vertices.push_back(v.y);
			vertices.push_back(v.z);
			idxVertex++;
		}
		else if(mode == modeReadElements) {
			vec4u32 e;
			vector<AnsiStr> arrWords;
			int ctWords = strLine.decompose(' ', arrWords);
			if(ctWords >= 4)
			{
				//int temp = atoi(arrWords[0].cptr());
				e.x = atoi(arrWords[1].cptr());
				e.y = atoi(arrWords[2].cptr());
				e.z = atoi(arrWords[3].cptr());
				e.w = atoi(arrWords[4].cptr());
			}

			//add element to the array
			elements.push_back(e.x);
			elements.push_back(e.y);
			elements.push_back(e.z);
			elements.push_back(e.w);

			idxElements++;
		}

		prevMode = mode;
		ctLine++;
	}
	fpIn.close();


	//setup
	cleanup();
	return setup(vertices, elements);
}

bool CellMesh::writeVegaFormat(const AnsiStr& strFP) const {
	if(countNodes() == 0 || countElements() == 0)
		return false;

	//Output veg file
	AnsiStr strVegFP = ChangeFileExt(strFP, ".veg");
	ofstream fpOut(strVegFP.cptr());

	//Include Node File
	fpOut << "# Vega Mesh File, Generated by FemBrain.\n";
	fpOut << "# " << countNodes() << " vertices, " << countElements() << " elements\n";
	fpOut << "\n";
	fpOut << "*VERTICES\n";
	fpOut << countNodes() << " 3 0 0\n";

	for(U32 i=0; i < countNodes(); i++)
	{
		vec3d v = const_nodeAt(i).pos;

		//VEGA expects one based index for everything
		fpOut << i+1 << " " << v.x << " " << v.y << " " << v.z << "\n";
	}

	//Line Separator
	fpOut << "\n";
	fpOut << "*ELEMENTS\n";
	fpOut << "TET\n";
	fpOut << countElements() << " 4 0\n";

	for(U32 i=0; i < countElements(); i++)
	{
		CELL elem = const_elemAt(i);
		vec4u32 n = vec4u32(&elem.nodes[0]);

		//VEGA expects one based index for everything
		fpOut << i+1 << " " << n.x+1 << " " << n.y+1 << " " << n.z+1 << " " << n.w+1 << "\n";
	}

	//Add Default Material
	fpOut << "\n";
	fpOut << "*MATERIAL BODY\n";
	fpOut << "ENU, 1000, 10000000, 0.45\n";

	fpOut << "\n";
	fpOut << "*REGION\n";
	fpOut << "allElements, BODY\n";

	//Include Element File
	fpOut.close();

	return true;
}


void CellMesh::draw() {
	vec3d p0;
	HEDGE hedge;

	glPushAttrib(GL_ALL_ATTRIB_BITS);
	glDisable(GL_LIGHTING);

	const U8 ctColors = 7;
	vec3d colors[ctColors] = {vec3d(0.5, 0.5, 0.5), vec3d(1, 0.3, 0), vec3d(0.3, 1, 0), vec3d(0, 0.3, 1),
					   	   	   vec3d(1, 1, 0), vec3d(0.5, 0.5, 0.5), vec3d(0, 1, 1)};

	//Draw filled faces
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDisable(GL_CULL_FACE);
	if(isElemIndex(m_elemToShow))
	{
		glColor3dv(colors[m_elemToShow % ctColors].cptr());
		drawElement(m_elemToShow);
	}
	else {
		for(U32 i=0; i< countElements(); i++)
		{
			glColor3dv(colors[i % ctColors].cptr());
			drawElement(i);
		}
	}
	glEnable(GL_CULL_FACE);


	//Draw outlined faces
	glDisable(GL_CULL_FACE);
	glLineWidth(3.0f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glColor3f(0.0f, 0.0f, 0.0f);

	for(U32 i=0; i< countElements(); i++) {
		drawElement(i);
	}
	glEnable(GL_CULL_FACE);

	//Draw vertices
	glPointSize(5.0f);
	glColor3f(1.0f, 0.0f, 0.0f);
	glBegin(GL_POINTS);
	for(U32 i=0; i < m_vNodes.size(); i++) {
		NODE n = const_nodeAt(i);
		glVertex3dv(n.pos.cptr());
	}
	glEnd();

	glEnable(GL_LIGHTING);
	glPopAttrib();
}

void CellMesh::drawElement(U32 i) const {
	CELL elem = const_elemAt(i);
	if(elem.removed)
		return;

	glBegin(GL_TRIANGLES);
		for(U32 f=0; f<4; f++)
		{
			FACE face = const_faceAt(elem.faces[f]);
			HEDGE he0 = const_halfedgeAt(face.halfedge[0]);
			HEDGE he1 = const_halfedgeAt(face.halfedge[1]);
			HEDGE he2 = const_halfedgeAt(face.halfedge[2]);
			vec3d p0 = const_nodeAt(he0.from).pos;
			vec3d p1 = const_nodeAt(he1.from).pos;
			vec3d p2 = const_nodeAt(he2.from).pos;

			glVertex3dv(p0.cptr());
			glVertex3dv(p1.cptr());
			glVertex3dv(p2.cptr());
		}
	glEnd();
}


AABB CellMesh::computeAABB() {
	double vMin[3], vMax[3];
	for(U32 i=0; i < countNodes(); i++) {
		NODE n = const_nodeAt(i);
		vec3d p = n.pos;

		if(i == 0) {
			vMin[0] = vMax[0] = p[0];
			vMin[1] = vMax[1] = p[1];
			vMin[2] = vMax[2] = p[2];
		}
		else {
			vMin[0] = MATHMIN(vMin[0], p[0]);
			vMin[1] = MATHMIN(vMin[1], p[1]);
			vMin[2] = MATHMIN(vMin[2], p[2]);

			vMax[0] = MATHMAX(vMax[0], p[0]);
			vMax[1] = MATHMAX(vMax[1], p[1]);
			vMax[2] = MATHMAX(vMax[2], p[2]);
		}
	}

	//set AABB
	m_aabb.set(vec3f((float)vMin[0], (float)vMin[1], (float)vMin[2]),
			   vec3f((float)vMax[0], (float)vMax[1], (float)vMax[2]));
	return m_aabb;
}

bool CellMesh::tst_keys() {
	int ctErrors = 0;
	for(U32 i = 0; i < m_vFaces.size(); i++) {
		const FACE face = const_faceAt(i);

		U32 nodes[3];
		for(U32 j=0; j < 3; j++)
			nodes[j] = vertex_from_hedge(face.halfedge[j]);

		FaceKey key(nodes[0], nodes[1], nodes[2]);
		MAPFACEITER it = m_mapFaces.find(key);
		if(it == m_mapFaces.end()) {
			ctErrors ++;
			printf("ERROR: No key entry in the map for face %u, nodes: [%u, %u, %u]\n", i, nodes[0], nodes[1], nodes[2]);
		}
		else if(it->second != i) {
			ctErrors ++;
			U32 wrongnodes[3];

			for(U32 j=0; j < 3; j++)
				wrongnodes[j] = INVALID_INDEX;
			if(isFaceIndex(it->second)) {
				const FACE wrongface = const_faceAt(it->second);
				for(U32 j=0; j < 3; j++)
					wrongnodes[j] = vertex_from_hedge(wrongface.halfedge[j]);
			}


			printf("ERROR: Map key entry is wrong for face %u, nodes: [%u, %u, %u], points to face instead: %u nodes: [%u, %u, %u]\n",
					i, nodes[0], nodes[1], nodes[2], it->second, wrongnodes[0], wrongnodes[1], wrongnodes[2]);

			FaceKey wrongKey(wrongnodes[0], wrongnodes[1], wrongnodes[2]);
			if(wrongKey.key == key.key) {
				printf("ERROR: Hashing had a collision here!\n");
			}

		}
	}

	if(ctErrors == 0)
		LogInfoArg1("PASS: %s", __FUNCTION__);
	else
		LogInfoArg1("FAILED!: %s", __FUNCTION__);
	return (ctErrors == 0);
}


