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
#include <deformable/VolMesh.h>
#include <graphics/AABB.h>

#include "graphics/selectgl.h"
#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iterator>
#include <map>
#include <set>
#include <utility>
#include <vector>

using namespace std;
using namespace PS;
using namespace PS::FILESTRINGUTILS;

using namespace PS;
using namespace PS::MESH;

VolMesh::VolMesh() {
	init();
}

VolMesh::VolMesh(U32 ctVertices, double* vertices, U32 ctElements, U32* elements) {
	init();
	setup(ctVertices, vertices, ctElements, elements);
}

VolMesh::VolMesh(const VolMesh& other) {
	init();

	vector<double> vertices;
	vector<U32> elements;

	U32 ctNodes = other.countNodes();
	U32 ctElements = other.countCells();
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
		CELL elem = other.const_cellAt(i);
		elements[i * 4 + 0] = elem.nodes[0];
		elements[i * 4 + 1] = elem.nodes[1];
		elements[i * 4 + 2] = elem.nodes[2];
		elements[i * 4 + 3] = elem.nodes[3];
	}

	setup(vertices, elements);
}

VolMesh::VolMesh(const vector<double>& vertices, const vector<U32>& elements) {
	init();
	setup(vertices, elements);
}


VolMesh::~VolMesh() {
	cleanup();
}


VolMesh* VolMesh::CreateOneTet() {
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

	VolMesh* tet = new VolMesh(vertices, elements);
	return tet;
}

void VolMesh::init() {
	m_elemToShow = INVALID_INDEX;
	m_fOnNodeEvent = NULL;
	m_fOnEdgeEvent = NULL;
	m_fOnFaceEvent = NULL;
	m_fOnElementEvent = NULL;
}

void VolMesh::setOnNodeEventCallback(OnNodeEvent f) {
	m_fOnNodeEvent = f;
}

void VolMesh::setOnEdgeEventCallback(OnEdgeEvent f) {
	m_fOnEdgeEvent = f;
}

void VolMesh::setOnFaceEventCallback(OnFaceEvent f) {
	m_fOnFaceEvent = f;
}

void VolMesh::setOnElemEventCallback(OnCellEvent f) {
	m_fOnElementEvent = f;
}


bool VolMesh::setup(const vector<double>& vertices, const vector<U32>& elements) {
	U32 ctVertices = vertices.size() / 3;
	U32 ctElements = elements.size() / 4;

	return setup(ctVertices, &vertices[0], ctElements, &elements[0]);
}

bool VolMesh::setup(U32 ctVertices, const double* vertices, U32 ctElements, const U32* elements) {

	//cleanup to setup the mesh
	cleanup();

	//add all vertices first
	for(U32 i=0; i<ctVertices; i++) {
		NODE node;
		node.pos = node.restpos = vec3d(&vertices[i*3]);
		insert_node(node);
	}


	//insert element
	for(U32 i=0; i<ctElements; i++)
		insert_cell(const_cast<U32 *>(&elements[i * 4]));


	//Compute AABB
	computeAABB();
	//checkMeshFaceDirections();
	this->printInfo();

	return true;
}

void VolMesh::cleanup() {
	m_vCells.resize(0);
	m_vFaces.resize(0);
	m_vEdges.resize(0);
	m_vNodes.resize(0);
	m_mapFaces.clear();
	m_mapEdgesIndex.clear();
}

void VolMesh::printInfo() const {
	//print all nodes
	printf("NODES #%u\n", (U32)m_vNodes.size());
	for(U32 i=0; i < m_vNodes.size(); i++) {
		NODE n = m_vNodes[i];

		printf("NODE %u, incident edges#: NA, pos: [%.3f, %.3f, %.3f]\n", i, n.pos.x, n.pos.y, n.pos.z);
	}
	printf("\n");

	//print all edges
	printf("HALFEDGES #%u\n", (U32)m_vEdges.size());
	for(U32 i=0; i < m_vEdges.size(); i++) {
		EDGE e = m_vEdges[i];

		printf("HEDGE %u, from: %d, to %d\n", i, e.from, e.to);
	}
	printf("\n");


	//print all face
	printf("FACES #%u\n", (U32)m_vFaces.size());
	U32 vhandles[3];
	for(U32 i=0; i < m_vFaces.size(); i++) {
		FACE face = m_vFaces[i];
		getFaceNodes(i, vhandles);

		printf("FACE %u, Nodes [%d, %d, %d], HalfEdges [%d, %d, %d]\n", i,
				vhandles[0], vhandles[1], vhandles[2],
				face.edges[0], face.edges[1], face.edges[2]);
	}
	printf("\n");

	//print all elements
	printf("ELEMS #%u\n", (U32)m_vCells.size());
	for(U32 i=0; i < m_vCells.size(); i++) {
		CELL elem = m_vCells[i];
		printf("ELEM %d, NODE: [%u, %u, %u, %u], ", i ,elem.nodes[0], elem.nodes[1], elem.nodes[2], elem.nodes[3]);
		printf("FACE: [%u, %u, %u, %u], ", elem.faces[0], elem.faces[1], elem.faces[2], elem.faces[3]);

		//print edges
		printf("HalfEdges: [");
		for(int e=0; e<6; e++) {
			if(e < 5)
				printf("%u, ", elem.edges[e]);
			else
				printf("%u]\n", elem.edges[e]);
		}
	}
	printf("\n");

}

double VolMesh::computeDeterminant(U32 idxNodes[4]) const {
	vec3d v[4];

	for(int i=0; i<4; i++)
		v[i] = const_nodeAt(i).pos;
	return ComputeElementDeterminant(v);
}

double VolMesh::ComputeElementDeterminant(const vec3d v[4]) {
	return	vec3d::dot(v[1] - v[0], vec3d::cross(v[2] - v[0], v[3] - v[0]));
}

//add/remove
bool VolMesh::insert_cell(const CELL& cell) {

	//check the structure before adding it
	for(int i=0; i<4; i++) {
		if(!isFaceIndex(cell.faces[i])) {
			LogErrorArg2("Unable to add element. Invalid face index found at: f[%d] = %d", i, cell.faces[i]);
			return false;
		}
	}

	for(int i=0; i<4; i++) {
		if(!isNodeIndex(cell.nodes[i])) {
			LogErrorArg2("Unable to add element. Invalid node index found at: n[%d] = %d", i, cell.nodes[i]);
			return false;
		}
	}

	for(int i=0; i<6; i++) {
		if(!isEdgeIndex(cell.edges[i])) {
			LogErrorArg2("Unable to add element. Invalid halfedge index found at: he[%d] = %d", i, cell.edges[i]);
			return false;
		}
	}

	m_vCells.push_back(cell);
	U32 idxCell = countCells() - 1;

	//update
	for(int i=0; i < 4; i++)
		m_incident_cells_per_face[ cell.faces[i] ].push_back(idxCell);

	if(m_fOnElementEvent)
		m_fOnElementEvent(cell, m_vCells.size() - 1, teAdded);

	return true;
}

bool VolMesh::insert_cell(U32 nodes[4]) {
	//Inserts a tetrahedra element to the halfedged mesh structure
	for(int i=0; i<4; i++) {
		if(!isNodeIndex(nodes[i])) {
			LogErrorArg1("Invalid node index passed in. %u", nodes[i]);
			return false;
		}
	}

	//face mask
	const int faceMaskPos[4][3] = { {1, 2, 3}, {2, 0, 3}, {3, 0, 1}, {1, 0, 2} };
	//const int faceMaskNeg[4][3] = { {3, 2, 1}, {3, 0, 2}, {1, 0, 3}, {2, 0, 1} };

	//edge mask
	const int edgeMaskPos[6][2] = { {1, 2}, {2, 3}, {3, 1}, {2, 0}, {0, 3}, {0, 1} };
	//const int edgeMaskNeg[6][2] = { {3, 2}, {2, 1}, {1, 3}, {3, 0}, {0, 2}, {1, 0} };

	//maps for half edges
	std::map< EdgeKey, EDGE > mapEdges;
	typedef std::map< EdgeKey, EDGE >::iterator HEITER;

	//Add element nodes set only for now
	CELL cell;
	for(int i=0; i<4; i++)
		cell.nodes[i] = nodes[i];
	for(int i=0; i<4; i++)
		cell.faces[i] = INVALID_INDEX;
	for(int i=0; i<6; i++)
		cell.edges[i] = INVALID_INDEX;

	//Loop over faces. Per each tet 6 edges or 12 half-edges added
	for (int f = 0; f < 4; f++) {
		U32 fv[3];

		fv[0] = nodes[faceMaskPos[f][0]];
		fv[1] = nodes[faceMaskPos[f][1]];
		fv[2] = nodes[faceMaskPos[f][2]];

		//Add all edges for this face
		for (int e = 0; e < 3; e++) {
			U32 from = fv[e];
			U32 to = fv[(e + 1) % 3];

			if(edge_exists(from, to) == false) {
				EdgeKey key(from, to);
				EDGE edge;
				edge.from = from;
				edge.to = to;
				mapEdges.insert(make_pair(key, edge));
			}
		}
	}

	//add final edges
	U32 from, to = INVALID_INDEX;

	while(mapEdges.size() > 0) {
		//add forward
		HEITER forward = mapEdges.begin();
		from = forward->second.from;
		to = forward->second.to;

		insert_edge(forward->second);

		//Remove from map
		mapEdges.erase(forward);
	}

	//add all faces now and finish setting information for all
	//loop over faces. Per each tet 6 edges or 12 half-edges added
	for (int f = 0; f < 4; f++) {
		U32 facenodes[3];

		facenodes[0] = nodes[faceMaskPos[f][0]];
		facenodes[1] = nodes[faceMaskPos[f][1]];
		facenodes[2] = nodes[faceMaskPos[f][2]];

		//Set element face index
		cell.faces[f] = insert_face(facenodes);

		//test if the face key maps to index
		FaceKey key(facenodes[0], facenodes[1], facenodes[2]);
		MAPFACEITER it = m_mapFaces.find(key);
		if(it == m_mapFaces.end()) {
			LogError("unable to find the face via its indices!");
		}
		assert(it->second == cell.faces[f]);
	}

	//Set element edges
	for (int e = 0; e < 6; e++) {
		from = cell.nodes[edgeMaskPos[e][0]];
		to = cell.nodes[edgeMaskPos[e][1]];

		EdgeKey key(from, to);
		MAPHEDGEINDEXITER it = m_mapEdgesIndex.find(key);
		if (it != m_mapEdgesIndex.end())
			cell.edges[e] = it->second;
		else {
			LogErrorArg2("Setting element edges failed! Unable to find edge <%d, %d>",
						from, to);
			return false;
		}
	}

	//call simple insert now
	return insert_cell(cell);
}

void VolMesh::remove_cell(U32 i) {
	assert(isCellIndex(i));

	//bump down refs for faces
	const CELL tet = const_cellAt(i);
	for(int j=0; j<4; j++) {
		//call face remove to decrement its refs
		remove_face( tet.faces[j] );
	}
}

U32 VolMesh::insert_node(const NODE& n) {
	m_vNodes.push_back(n);
	m_incident_edges_per_node.resize(countNodes());

	return countNodes() - 1;
}

U32 VolMesh::insert_edge(const EDGE& e) {
	m_vEdges.push_back(e);
	m_incident_faces_per_edge.resize(countEdges());
	U32 idxEdge = countEdges() - 1;

	//update incident edges per vertex
	m_incident_edges_per_node[e.from].push_back(idxEdge);
	m_incident_edges_per_node[e.to].push_back(idxEdge);


	//insert the forward halfedge into map
	insertEdgeIndexToMap(e.from, e.to, idxEdge);

	return idxEdge;
}

//inserting a face uniquely
U32 VolMesh::insert_face(U32 nodes[3]) {

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

		EdgeKey key(from, to);
		MAPHEDGEINDEXITER it = m_mapEdgesIndex.find(key);
		if (it != m_mapEdgesIndex.end()) {
			face.edges[e] = it->second;
		}
		else {
			LogErrorArg2("Setting face edges failed! Unable to find edge <%d, %d>",
						 from, to);

			return BaseHandle::INVALID;
		}
	}

	//add the face now
	m_vFaces.push_back(face);
	U32 idxFace = countFaces() - 1;
	insertFaceIndexToMap(nodes, idxFace);

	m_incident_cells_per_face.resize(countFaces());

	//update
	for(int i=0; i < FACE_SIDES; i++)
		m_incident_faces_per_edge[face.edges[i]].push_back(idxFace);

	return idxFace;
}


void VolMesh::remove_face(U32 i) {
	assert(isFaceIndex(i));

	//mark face as removed
	//m_vFaces[i].removed = true;
}


void VolMesh::garbage_collection() {

	//acquire lock to mesh
	tst_keys();

	//elements
	U32 i = 0;
	U32 ctRemovedElements = 0;
	while(i < countCells()) {
		CELL tet = elemAt(i);
		if(tet.removed) {
			m_vCells.erase(m_vCells.begin() + i);
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
			nodes[0] = vertex_from_edge(face.edges[0]);
			nodes[1] = vertex_from_edge(face.edges[1]);
			nodes[2] = vertex_from_edge(face.edges[2]);
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


	//apply correct face indices to elements
	for(i = 0; i < countCells(); i++) {
		for(U32 j = 0; j < 4; j++) {
			U32 temp = m_vCells[i].faces[j];
			m_vCells[i].faces[j] = vFaceIndexAfter[temp];
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

bool VolMesh::getFaceNodes(U32 idxFace, U32 (&nodes)[3]) const {
	if(!isFaceIndex(idxFace))
		return false;

	FACE face = const_faceAt(idxFace);

	std::set<U32> setFaceNodes;
	for(int i=0; i < 3; i++) {
		setFaceNodes.insert(from_node(face.edges[i]));
		setFaceNodes.insert(to_node(face.edges[i]));
	}

	assert(setFaceNodes.size() == 3);
	int i = 0;
	for(std::set<U32>::const_iterator it = setFaceNodes.begin(); it != setFaceNodes.end(); ++it) {
		nodes[i] = *it;
		i++;
	}

	return true;
}

CELL& VolMesh::elemAt(U32 i) {
	assert(isCellIndex(i));
	return m_vCells[i];
}

FACE& VolMesh::faceAt(U32 i) {
	assert(isFaceIndex(i));
	return m_vFaces[i];
}

EDGE& VolMesh::edgeAt(U32 i) {
	assert(isEdgeIndex(i));
	return m_vEdges[i];
}

NODE& VolMesh::nodeAt(U32 i) {
	assert(isNodeIndex(i));
	return m_vNodes[i];
}


//access
const CELL& VolMesh::const_cellAt(U32 i) const {
	assert(isCellIndex(i));
	return m_vCells[i];
}

const FACE& VolMesh::const_faceAt(U32 i) const {
	assert(isFaceIndex(i));
	return m_vFaces[i];
}

const EDGE& VolMesh::const_edgeAt(U32 i) const {
	assert(isEdgeIndex(i));
	return m_vEdges[i];
}

const NODE& VolMesh::const_nodeAt(U32 i) const {
	assert(isNodeIndex(i));
	return m_vNodes[i];
}

void VolMesh::displace(double * u) {
	if(u == NULL)
		return;

	for(U32 i=0; i < countNodes(); i++) {
		m_vNodes[i].pos = m_vNodes[i].pos + vec3d(&u[i * 3]);
	}

	computeAABB();
}

bool VolMesh::insertEdgeIndexToMap(U32 from, U32 to, U32 idxEdge) {
	if(from == to) {
		LogErrorArg2("Can not register an edge with the same from and to nodes! [%u, %u]", from, to);
		return false;
	}

	if(!isEdgeIndex(idxEdge)) {
		LogErrorArg1("Can not register an invalid edge index: %u", idxEdge);
		return false;
	}

	EdgeKey key(from, to);
	m_mapEdgesIndex.insert( std::make_pair( key, idxEdge) );
	return true;
}

bool VolMesh::removeEdgeIndexFromMap(U32 from, U32 to) {
	EdgeKey key(from, to);
	MAPHEDGEINDEXITER it = m_mapEdgesIndex.find(key);
	if(it == m_mapEdgesIndex.end())
		return false;
	else {
		m_mapEdgesIndex.erase(it);
		return true;
	}
}

bool VolMesh::insertFaceIndexToMap(U32 nodes[3], U32 idxFace) {

	if(!isFaceIndex(idxFace)) {
		LogErrorArg1("Can not register an invalid face index: %u", idxFace);
		return false;
	}

	FaceKey key(nodes[0], nodes[1], nodes[2]);
	m_mapFaces.insert( std::make_pair(key, idxFace));
	return true;
}

bool VolMesh::removeFaceIndexFromMap(U32 nodes[3]) {
	FaceKey key(nodes[0],  nodes[1], nodes[2]);
	MAPFACEITER it = m_mapFaces.find(key);
	if(it == m_mapFaces.end())
		return false;
	else {
		m_mapFaces.erase(it);
		return true;
	}
}

EdgeKey VolMesh::computeEdgeKey(U32 idxEdge) const {
	assert(isEdgeIndex(idxEdge));

	const EDGE& e = const_edgeAt(idxEdge);
	EdgeKey key(e.from, e.to);
	return key;
}

FaceKey VolMesh::computeFaceKey(U32 idxFace) const {
	assert(isFaceIndex(idxFace));

	U32 nodes[3];
	assert(getFaceNodes(idxFace, nodes));
	FaceKey key(nodes[0], nodes[1], nodes[2]);
	return key;
}


bool VolMesh::edge_exists(U32 from, U32 to) const {
	EdgeKey key(from, to);
	return (m_mapEdgesIndex.find( key ) != m_mapEdgesIndex.end());
}

U32 VolMesh::edge_handle(U32 from, U32 to) {
	EdgeKey key(from, to);
	MAPHEDGEINDEXITER it = m_mapEdgesIndex.find(key);
	if(it != m_mapEdgesIndex.end())
		return it->second;
	else
		return INVALID_INDEX;
}


bool VolMesh::cut_edge(int idxEdge, double distance, U32* poutIndexNP0, U32* poutIndexNP1) {
	if(!isEdgeIndex(idxEdge))
		return false;

	EDGE& edge = edgeAt(idxEdge);

	U32 from = from_node(idxEdge);
	U32 to = to_node(idxEdge);

	NODE n0 = const_nodeAt(from);
	NODE n1 = const_nodeAt(to);

	//add two new point
	NODE np0;
	np0.pos = n0.pos + (n1.pos - n0.pos).normalized() * distance;
	np0.restpos = n0.restpos + (n1.restpos - n0.restpos).normalized() * distance;
	U32 idxNP0 = insert_node(np0);

	NODE np1 = np0;
	U32 idxNP1 = insert_node(np1);


	//Add new halfedges
	//half-edges are cut but before adding new faces we keep the face connectivity of the edges
	HEDGE he2 = he0;
	HEDGE he3 = he1;
	he2.from = idxNP1;
	he3.to 	= idxNP1;


	m_vEdges.push_back(he2);
	U32 idxHE2 = m_vEdges.size() - 1;
	m_vEdges.push_back(he3);
	U32 idxHE3 = m_vEdges.size() - 1;

	//insert new half edges and remove old ones
	insertEdgeIndexToMap(he2.from, he2.to, idxHE2);
	insertEdgeIndexToMap(he3.from, he3.to, idxHE3);

	//update outHE for new nodes
	m_vNodes[idxNP0].outHE = idxHE1;
	m_vNodes[idxNP1].outHE = idxHE2;

	//update old halfedges.
	//this should force face updates otherwise invalid faces will occur!
	//half-edges are cut but before adding new faces we keep the face connectivity of the edges

	//facemap: remove faces adjacent to halfedges 0 and 1
//	U32 nodes[3];
//	getFaceNodes(he0.face, nodes);
//	removeFaceIndexFromMap(nodes);
//
//	getFaceNodes(he1.face, nodes);
//	removeFaceIndexFromMap(nodes);

	//hedgemap: remove halfedges 0 and 1
	removeEdgeIndexFromMap(he0.from, he0.to);
	removeEdgeIndexFromMap(he1.from, he1.to);

	he0.to = idxNP0;
	he1.from = idxNP0;
	m_vEdges[idxHE0] = he0;
	m_vEdges[idxHE1] = he1;

	//hedgemap: insert new half edges
	insertEdgeIndexToMap(he0.from, he0.to, idxHE0);
	insertEdgeIndexToMap(he1.from, he1.to, idxHE1);

	//update faces adjacent to half-edges 0 and 1
//	getFaceNodes(he0.face, nodes);
//	insertFaceIndexToMap(nodes, he0.face);
//
//	getFaceNodes(he1.face, nodes);
//	insertFaceIndexToMap(nodes, he1.face);


	//Write output vertices
	if(poutIndexNP0)
		*poutIndexNP0 = idxNP0;

	if(poutIndexNP1)
		*poutIndexNP1 = idxNP1;

	return true;
}

bool VolMesh::getNodeIncidentEdges(U32 idxNode, vector<U32>& incidentEdges) const {

	if(!isNodeIndex(idxNode))
		return false;

	incidentEdges.assign(m_incident_edges_per_node[idxNode].begin(), m_incident_edges_per_node[idxNode].end());
	return true;
}

bool VolMesh::getCellFacesFromMapHighCost(U32 idxCell, U32 (&faces)[4]) {
	if(!isCellIndex(idxCell))
		return false;

	//face mask
	static int faceMaskPos[4][3] = { {1, 2, 3}, {2, 0, 3}, {3, 0, 1}, {1, 0, 2} };

	CELL cell = const_cellAt(idxCell);
	FaceKey key;
	for(U32 i=0; i<4; i++) {
		key.setup(cell.nodes[faceMaskPos[i][0]], cell.nodes[faceMaskPos[i][1]], cell.nodes[faceMaskPos[i][2]]);
		faces[i] = m_mapFaces[key];
	}

	return true;
}

bool VolMesh::getCellEdgesFromMapHighCost(U32 idxCell, U32 (&edges)[6])  {
	//edge mask
	static int edgeMaskPos[6][2] = { {1, 2}, {2, 3}, {3, 1}, {2, 0}, {0, 3}, {0, 1} };

	CELL cell = const_cellAt(idxCell);
	EdgeKey key;
	for(U32 i=0; i<6; i++) {
		key.setup(cell.nodes[edgeMaskPos[i][0]], cell.nodes[edgeMaskPos[i][1]]);
		edges[i] = m_mapEdgesIndex[key];
	}

	return true;
}


void VolMesh::setElemToShow(U32 elem) {
	if(elem == m_elemToShow)
		return;
	LogInfoArg1("Requested element to shows: %u\n", elem);
	m_elemToShow = elem;
}


bool VolMesh::readVegaFormat(const AnsiStr& strFP) {
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

bool VolMesh::writeVegaFormat(const AnsiStr& strFP) const {
	if(countNodes() == 0 || countCells() == 0)
		return false;

	//Output veg file
	AnsiStr strVegFP = ChangeFileExt(strFP, ".veg");
	ofstream fpOut(strVegFP.cptr());

	//Include Node File
	fpOut << "# Vega Mesh File, Generated by FemBrain.\n";
	fpOut << "# " << countNodes() << " vertices, " << countCells() << " elements\n";
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
	fpOut << countCells() << " 4 0\n";

	for(U32 i=0; i < countCells(); i++)
	{
		CELL elem = const_cellAt(i);
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


void VolMesh::draw() {
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	glDisable(GL_LIGHTING);

	const U8 ctColors = 7;
	vec3d colors[ctColors] = {vec3d(0.5, 0.5, 0.5), vec3d(1, 0.3, 0), vec3d(0.3, 1, 0), vec3d(0, 0.3, 1),
					   	   	   vec3d(1, 1, 0), vec3d(0.5, 0.5, 0.5), vec3d(0, 1, 1)};

	//Draw filled faces
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDisable(GL_CULL_FACE);
	if(isCellIndex(m_elemToShow))
	{
		glColor3dv(colors[m_elemToShow % ctColors].cptr());
		drawElement(m_elemToShow);
	}
	else {
		for(U32 i=0; i< countCells(); i++)
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

	for(U32 i=0; i< countCells(); i++) {
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

void VolMesh::drawElement(U32 i) const {
	CELL elem = const_cellAt(i);

	glBegin(GL_TRIANGLES);
		for(U32 f=0; f<4; f++)
		{
			U32 nodes[3];
			FACE face = const_faceAt(elem.faces[f]);
			getFaceNodes(elem.faces[f], nodes);

			vec3d p0 = const_nodeAt(nodes[0]).pos;
			vec3d p1 = const_nodeAt(nodes[1]).pos;
			vec3d p2 = const_nodeAt(nodes[2]).pos;

			glVertex3dv(p0.cptr());
			glVertex3dv(p1.cptr());
			glVertex3dv(p2.cptr());
		}
	glEnd();
}


AABB VolMesh::computeAABB() {
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

bool VolMesh::tst_keys() {
	int ctErrors = 0;
	for(U32 i = 0; i < m_vFaces.size(); i++) {
		const FACE face = const_faceAt(i);

		U32 nodes[3];
		for(U32 j=0; j < 3; j++)
			nodes[j] = from_node(face.edges[j]);

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
					wrongnodes[j] = from_node(wrongface.edges[j]);
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


