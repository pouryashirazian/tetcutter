/*
 * WingedEdgeTetMesh.cpp
 *
 *  Created on: May 6, 2014
 *      Author: pourya
 */

#include "HalfEdgeTetMesh.h"
#include "base/Logger.h"
#include "base/String.h"
#include "base/FileDirectory.h"
#include <fstream>
#include <map>
#include <utility>
#include <algorithm>
#include "graphics/selectgl.h"

using namespace std;
using namespace PS;
using namespace PS::FILESTRINGUTILS;

namespace PS {
namespace FEM {



HalfEdgeTetMesh::HalfEdgeTetMesh() {

}

HalfEdgeTetMesh::HalfEdgeTetMesh(U32 ctVertices, double* vertices, U32 ctElements, U32* elements) {
	setup(ctVertices, vertices, ctElements, elements);
}

HalfEdgeTetMesh::HalfEdgeTetMesh(const vector<double>& vertices, const vector<U32>& elements) {
	setup(vertices, elements);
}


HalfEdgeTetMesh::~HalfEdgeTetMesh() {
	cleanup();
}

bool HalfEdgeTetMesh::setup(const vector<double>& vertices, const vector<U32>& elements) {
	U32 ctVertices = vertices.size() / 3;
	U32 ctElements = elements.size() / 4;

	return setup(ctVertices, &vertices[0], ctElements, &elements[0]);
}

bool HalfEdgeTetMesh::setup(U32 ctVertices, const double* vertices, U32 ctElements, const U32* elements) {

	//add all vertices first
	for(U32 i=0; i<ctVertices; i++) {
		HalfEdgeTetMesh::NODE node;
		node.pos = node.restpos = vec3d(&vertices[i*3]);
		node.outHE = INVALID_INDEX;
		m_vNodes.push_back(node);
	}

	//Face Mask
	int faceMaskPos[4][3] = { {1, 2, 3}, {2, 0, 3}, {3, 0, 1}, {1, 0, 2} };
	int faceMaskNeg[4][3] = { {3, 2, 1}, {3, 0, 2}, {1, 0, 3}, {2, 0, 1} };

//	int edgeMaskPos[6][2] = { {1, 2}, {2, 3}, {3, 1}, {2, 0}, {0, 3}, {0, 1} };
//	int edgeMaskNeg[6][2] = { {3, 2}, {2, 1}, {1, 3}, {3, 0}, {0, 2}, {1, 0} };

	//add all edges and elements
	std::map< pair<U32, U32>, HEDGE > mapHalfEdges;
	typedef std::map< pair<U32, U32>, HEDGE >::iterator HEITER;

	vector<double> vDeterminants;
	vDeterminants.resize(ctElements);

	m_vElements.reserve(ctElements);
	for(U32 i=0; i<ctElements; i++) {

		vec4u32 n = vec4u32(&elements[i * 4]);
		vec3d v[4];
		for(int j=0; j<4; j++)
			v[j] = vec3d(&vertices[ 3 * n[j] ]);

		//compute determinant
		vDeterminants[i] = vec3d::dot(v[1] - v[0], vec3d::cross(v[2] - v[0], v[3] - v[0]));

		//Add element nodes set only for now
		HalfEdgeTetMesh::ELEM elem;
		elem.posDet = (vDeterminants[i] >= 0);
		n.store(&elem.nodes[0]);
		for(int j=0; j<4; j++)
			elem.faces[j] = INVALID_INDEX;
		m_vElements.push_back(elem);

		//Loop over faces. Per each tet 6 edges or 12 half-edges added
		for(int f = 0; f < 4; f++) {
			U32 fv[3];

			if(vDeterminants[i] >= 0) {
				fv[0] = n[ faceMaskPos[f][0] ];
				fv[1] = n[ faceMaskPos[f][1] ];
				fv[2] = n[ faceMaskPos[f][2] ];
			}
			else {
				fv[0] = n[ faceMaskNeg[f][0] ];
				fv[1] = n[ faceMaskNeg[f][1] ];
				fv[2] = n[ faceMaskNeg[f][2] ];
			}

			//Add all halfedges for this face
			for(int e=0; e<3; e++) {
				HalfEdgeTetMesh::HEDGE he;
				he.from = fv[e];
				he.to = fv[ (e + 1) % 3 ];
				mapHalfEdges.insert( make_pair( std::make_pair(he.from, he.to), he));
			}
		}
	}

	//add final half edges in the order of (u, v) and (v, u)
	m_vHalfEdges.reserve(mapHalfEdges.size());
	std::map< pair<U32, U32>, U32 > mapHalfEdgesIndex;
	typedef std::map< pair<U32, U32>, U32 >::iterator HEINDEXITER;

	U32 from, to = -1;
	U32 idxForward, idxBackward = -1;

	while(mapHalfEdges.size() > 0) {
		//add forward
		HEITER forward = mapHalfEdges.begin();
		from = forward->second.from;
		to = forward->second.to;

		m_vHalfEdges.push_back(forward->second);
		idxForward = m_vHalfEdges.size() - 1;
		mapHalfEdgesIndex.insert( make_pair( std::make_pair(from, to), idxForward));


		//Remove from map
		mapHalfEdges.erase(forward);

		//add backward
		HEITER backward = mapHalfEdges.find(std::make_pair(to, from));
		if(backward != mapHalfEdges.end()) {
			m_vHalfEdges.push_back(backward->second);
			idxBackward = idxForward + 1;
			mapHalfEdgesIndex.insert( make_pair( std::make_pair(to, from), idxBackward));

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
	std::map< FaceKey, U32 > mapFaces;
	typedef std::map< FaceKey, U32 >::iterator FITER;

	//edgemask
	int edgeMaskPos[6][2] = { {1, 2}, {2, 3}, {3, 1}, {2, 0}, {0, 3}, {0, 1} };
	int edgeMaskNeg[6][2] = { {3, 2}, {2, 1}, {1, 3}, {3, 0}, {0, 2}, {1, 0} };

	m_vFaces.reserve(ctElements * 4);
	for(U32 i=0; i<ctElements; i++) {
		vec4u32 n = vec4u32(&elements[i * 4]);

		//Loop over faces. Per each tet 6 edges or 12 half-edges added
		for(int f = 0; f < 4; f++) {
			U32 fv[3];

			if(vDeterminants[i] >= 0) {
				fv[0] = n[ faceMaskPos[f][0] ];
				fv[1] = n[ faceMaskPos[f][1] ];
				fv[2] = n[ faceMaskPos[f][2] ];
			}
			else {
				fv[0] = n[ faceMaskNeg[f][0] ];
				fv[1] = n[ faceMaskNeg[f][1] ];
				fv[2] = n[ faceMaskNeg[f][2] ];
			}

			//add faces
			HalfEdgeTetMesh::FACE face;

			//Add all halfedges for this face
			for(int e=0; e<3; e++) {
				from = fv[e];
				to = fv[ (e + 1) % 3 ];

				HEINDEXITER it = mapHalfEdgesIndex.find(std::make_pair(from, to));
				if(it != mapHalfEdgesIndex.end())
					face.halfedge[e] = it->second;
				else {
					LogErrorArg2("Setting face edges failed! Unable to find edge <%d, %d>", from, to);
					return false;
				}
			}

			//Resolve unique faces
			//printf("Face order before: %d, %d, %d\n", fv[0], fv[1], fv[2]);
			FaceKey::order_lo2hi(fv[0], fv[1], fv[2]);
			//printf("Face order after: %d, %d, %d\n", fv[0], fv[1], fv[2]);
			FaceKey key(fv, m_vHalfEdges.size());
			FITER fit = mapFaces.find(key);

			//if face not found then add it
			U32 idxFace = INVALID_INDEX;
			if(fit == mapFaces.end()) {
				m_vFaces.push_back(face);
				idxFace = m_vFaces.size() - 1;
				mapFaces.insert( std::make_pair(key, idxFace));

				//set prev, next, face for half edges
				for(int e=0; e<3; e++) {
					if(e == 0)
						m_vHalfEdges[ face.halfedge[e] ].prev = face.halfedge[2];
					else
						m_vHalfEdges[ face.halfedge[e] ].prev = face.halfedge[e - 1];
					m_vHalfEdges[ face.halfedge[e] ].next = face.halfedge[(e + 1) % 3];
					m_vHalfEdges[ face.halfedge[e] ].face = idxFace;
				}
			}
			else
				idxFace = fit->second;

			//Set element face index and halfedges
			m_vElements[i].faces[f] = idxFace;

			U32 from, to = INVALID_INDEX;
			for(int e=0; e < 6; e++) {
				if(m_vElements[i].posDet) {
					from = m_vElements[i].nodes[ edgeMaskPos[e][0] ];
					to = m_vElements[i].nodes[ edgeMaskPos[e][1] ];
				}
				else {
					from = m_vElements[i].nodes[ edgeMaskNeg[e][0] ];
					to = m_vElements[i].nodes[ edgeMaskNeg[e][1] ];
				}

				HEINDEXITER it = mapHalfEdgesIndex.find(std::make_pair(from, to));
				if(it != mapHalfEdgesIndex.end())
					m_vElements[i].halfedge[e] = it->second;
				else {
					LogErrorArg2("Setting element halfedges failed! Unable to find edge <%d, %d>", from, to);
					return false;
				}
			}

		}
	}

	//Set outgoing edges for nodes
	for(U32 i=0; i < m_vHalfEdges.size(); i++) {
		HalfEdgeTetMesh::HEDGE e = m_vHalfEdges[i];

		//set the outHalfEdge for all nodes in this edge
		if(m_vNodes[e.from].outHE == INVALID_INDEX)
			m_vNodes[e.from].outHE = i;
		if(m_vNodes[e.to].outHE == INVALID_INDEX)
			m_vNodes[e.to].outHE = i;
	}

	//Compute AABB
	computeAABB();
	//checkMeshFaceDirections();

	this->printInfo();
	return true;
}

void HalfEdgeTetMesh::cleanup() {
	m_vElements.resize(0);
	m_vFaces.resize(0);
	m_vHalfEdges.resize(0);
	m_vNodes.resize(0);
}

void HalfEdgeTetMesh::printInfo() const {
	//print all nodes
	printf("NODES #%u\n", (U32)m_vNodes.size());
	for(U32 i=0; i < m_vNodes.size(); i++) {
		HalfEdgeTetMesh::NODE n = m_vNodes[i];

		printf("NODE outgoing edge: %d, pos: [%.3f, %.3f, %.3f]\n", n.outHE, n.pos.x, n.pos.y, n.pos.z);
	}

	//print all edges
	printf("HALFEDGES #%u\n", (U32)m_vHalfEdges.size());
	for(U32 i=0; i < m_vHalfEdges.size(); i++) {
		HalfEdgeTetMesh::HEDGE e = m_vHalfEdges[i];

		printf("HEDGE %d, from: %d, to %d, next: %d, prev: %d, opposite: %d, face: %d\n", i, e.from, e.to, e.next, e.prev, e.opposite, e.face);
	}

	//print all face
	printf("FACES #%u\n", (U32)m_vFaces.size());
	for(U32 i=0; i < m_vFaces.size(); i++) {
		HalfEdgeTetMesh::FACE face = m_vFaces[i];
		printf("FACE %d, [%d, %d, %d]\n", i, face.halfedge[0], face.halfedge[1], face.halfedge[2]);
	}

	//print all elements
	printf("ELEMS #%u\n", (U32)m_vElements.size());
	for(U32 i=0; i < m_vElements.size(); i++) {
		HalfEdgeTetMesh::ELEM elem = m_vElements[i];
		printf("ELEM %d, NODE: [%u, %u, %u, %u]", i ,elem.nodes[0], elem.nodes[1], elem.nodes[2], elem.nodes[3]);
		printf(" FACE: [%u, %u, %u, %u]\n", elem.faces[0], elem.faces[1], elem.faces[2], elem.faces[3]);
	}

}

double HalfEdgeTetMesh::computeDeterminant(U32 idxNodes[4]) const {
	vec3d v[4];

	for(int i=0; i<4; i++)
		v[i] = const_nodeAt(i).pos;
	return computeElementDeterminant(v);
}

double HalfEdgeTetMesh::computeElementDeterminant(const vec3d v[4]) {
	return	vec3d::dot(v[1] - v[0], vec3d::cross(v[2] - v[0], v[3] - v[0]));
}

//add/remove
int HalfEdgeTetMesh::insert_element(const ELEM& e) {

	//check the structure before adding it
	for(int i=0; i<4; i++) {
		if(!isFaceIndex(e.faces[i])) {
			LogErrorArg2("Unable to add element. Invalid face index found at: f[%d] = %d", i, e.faces[i]);
			return -1;
		}
	}

	for(int i=0; i<4; i++) {
		if(!isNodeIndex(e.nodes[i])) {
			LogErrorArg2("Unable to add element. Invalid node index found at: n[%d] = %d", i, e.nodes[i]);
			return -1;
		}
	}

	for(int i=0; i<6; i++) {
		if(!isHalfEdgeIndex(e.halfedge[i])) {
			LogErrorArg2("Unable to add element. Invalid halfedge index found at: he[%d] = %d", i, e.halfedge[i]);
			return -1;
		}
	}

	m_vElements.push_back(e);
	return (int)(m_vElements.size() - 1);
}

int HalfEdgeTetMesh::insert_element(U32 nodes[4]) {
	//Inserts a tetrahedra element to the halfedged mesh structure
	for(int i=0; i<4; i++) {
		if(!isNodeIndex(nodes[i])) {
			LogErrorArg1("Invalid node index passed in. %u", nodes[i]);
			return -1;
		}
	}

	//face mask
	const int faceMaskPos[4][3] = { {1, 2, 3}, {2, 0, 3}, {3, 0, 1}, {1, 0, 2} };
	const int faceMaskNeg[4][3] = { {3, 2, 1}, {3, 0, 2}, {1, 0, 3}, {2, 0, 1} };

	//edge mask
	const int edgeMaskPos[6][2] = { {1, 2}, {2, 3}, {3, 1}, {2, 0}, {0, 3}, {0, 1} };
	const int edgeMaskNeg[6][2] = { {3, 2}, {2, 1}, {1, 3}, {3, 0}, {0, 2}, {1, 0} };

	//maps for half edges
	std::map< pair<U32, U32>, HEDGE > mapHalfEdges;
	typedef std::map< pair<U32, U32>, HEDGE >::iterator HEITER;

	std::map< pair<U32, U32>, U32 > mapHalfEdgesIndex;
	typedef std::map< pair<U32, U32>, U32 >::iterator HEINDEXITER;


	//determinant
	double det = computeDeterminant(nodes);

	//Add element nodes set only for now
	HalfEdgeTetMesh::ELEM elem;
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
			HalfEdgeTetMesh::HEDGE he;
			he.from = fv[e];
			he.to = fv[(e + 1) % 3];
			mapHalfEdges.insert(make_pair(std::make_pair(he.from, he.to), he));
		}
	}

	//add final half edges in the order of (u, v) and (v, u)
	vector<HEDGE> arrAppendingHEdges;
	//m_vHalfEdges.reserve(mapHalfEdges.size());

	U32 from, to = -1;
	U32 idxForward, idxBackward = -1;

	while(mapHalfEdges.size() > 0) {
		//add forward
		HEITER forward = mapHalfEdges.begin();
		from = forward->second.from;
		to = forward->second.to;

		m_vHalfEdges.push_back(forward->second);
		idxForward = m_vHalfEdges.size() - 1;
		mapHalfEdgesIndex.insert( make_pair( std::make_pair(from, to), idxForward));


		//Remove from map
		mapHalfEdges.erase(forward);

		//add backward
		HEITER backward = mapHalfEdges.find(std::make_pair(to, from));
		if(backward != mapHalfEdges.end()) {
			m_vHalfEdges.push_back(backward->second);
			idxBackward = idxForward + 1;
			mapHalfEdgesIndex.insert( make_pair( std::make_pair(to, from), idxBackward));

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
	std::map< FaceKey, U32 > mapFaces;
	typedef std::map< FaceKey, U32 >::iterator FITER;

	/*

	m_vFaces.reserve(ctElements * 4);
	for(U32 i=0; i<ctElements; i++) {
		vec4u32 n = vec4u32(&elements[i * 4]);

		//Loop over faces. Per each tet 6 edges or 12 half-edges added
		for(int f = 0; f < 4; f++) {
			U32 fv[3];

			if(vDeterminants[i] >= 0) {
				fv[0] = n[ faceMaskPos[f][0] ];
				fv[1] = n[ faceMaskPos[f][1] ];
				fv[2] = n[ faceMaskPos[f][2] ];
			}
			else {
				fv[0] = n[ faceMaskNeg[f][0] ];
				fv[1] = n[ faceMaskNeg[f][1] ];
				fv[2] = n[ faceMaskNeg[f][2] ];
			}

			//add faces
			HalfEdgeTetMesh::FACE face;

			//Add all halfedges for this face
			for(int e=0; e<3; e++) {
				from = fv[e];
				to = fv[ (e + 1) % 3 ];

				HEINDEXITER it = mapHalfEdgesIndex.find(std::make_pair(from, to));
				if(it != mapHalfEdgesIndex.end())
					face.halfedge[e] = it->second;
				else {
					LogErrorArg2("Setting face edges failed! Unable to find edge <%d, %d>", from, to);
					return false;
				}
			}

			//Resolve unique faces
			//printf("Face order before: %d, %d, %d\n", fv[0], fv[1], fv[2]);
			FaceKey::order_lo2hi(fv[0], fv[1], fv[2]);
			//printf("Face order after: %d, %d, %d\n", fv[0], fv[1], fv[2]);
			FaceKey key(fv, m_vHalfEdges.size());
			FITER fit = mapFaces.find(key);

			//if face not found then add it
			U32 idxFace = INVALID_INDEX;
			if(fit == mapFaces.end()) {
				m_vFaces.push_back(face);
				idxFace = m_vFaces.size() - 1;
				mapFaces.insert( std::make_pair(key, idxFace));

				//set prev, next, face for half edges
				for(int e=0; e<3; e++) {
					if(e == 0)
						m_vHalfEdges[ face.halfedge[e] ].prev = face.halfedge[2];
					else
						m_vHalfEdges[ face.halfedge[e] ].prev = face.halfedge[e - 1];
					m_vHalfEdges[ face.halfedge[e] ].next = face.halfedge[(e + 1) % 3];
					m_vHalfEdges[ face.halfedge[e] ].face = idxFace;
				}
			}
			else
				idxFace = fit->second;

			//Set element face index and halfedges
			m_vElements[i].faces[f] = idxFace;

			U32 from, to = INVALID_INDEX;
			for(int e=0; e < 6; e++) {
				if(m_vElements[i].posDet) {
					from = m_vElements[i].nodes[ edgeMaskPos[e][0] ];
					to = m_vElements[i].nodes[ edgeMaskPos[e][1] ];
				}
				else {
					from = m_vElements[i].nodes[ edgeMaskNeg[e][0] ];
					to = m_vElements[i].nodes[ edgeMaskNeg[e][1] ];
				}

				HEINDEXITER it = mapHalfEdgesIndex.find(std::make_pair(from, to));
				if(it != mapHalfEdgesIndex.end())
					m_vElements[i].halfedge[e] = it->second;
				else {
					LogErrorArg2("Setting element halfedges failed! Unable to find edge <%d, %d>", from, to);
					return false;
				}
			}

		}
	}

	//Set outgoing edges for nodes
	for(U32 i=0; i < m_vHalfEdges.size(); i++) {
		HalfEdgeTetMesh::HEDGE e = m_vHalfEdges[i];

		//set the outHalfEdge for all nodes in this edge
		if(m_vNodes[e.from].outHE == INVALID_INDEX)
			m_vNodes[e.from].outHE = i;
		if(m_vNodes[e.to].outHE == INVALID_INDEX)
			m_vNodes[e.to].outHE = i;
	}
*/

	return -1;
}

void HalfEdgeTetMesh::remove_element(U32 i) {
	m_vElements.erase(m_vElements.begin() + i);
}

HalfEdgeTetMesh::ELEM& HalfEdgeTetMesh::elemAt(U32 i) {
	assert(isElemIndex(i));
	return m_vElements[i];
}

HalfEdgeTetMesh::FACE& HalfEdgeTetMesh::faceAt(U32 i) {
	assert(isFaceIndex(i));
	return m_vFaces[i];
}

HalfEdgeTetMesh::HEDGE& HalfEdgeTetMesh::halfedgeAt(U32 i) {
	assert(isHalfEdgeIndex(i));
	return m_vHalfEdges[i];
}

HalfEdgeTetMesh::EDGE HalfEdgeTetMesh::edgeAt(U32 i) const {
	assert(isEdgeIndex(i));
	EDGE e(const_halfedgeAt(i * 2), const_halfedgeAt(i * 2 + 1));
	return e;
}

HalfEdgeTetMesh::NODE& HalfEdgeTetMesh::nodeAt(U32 i) {
	assert(isElemIndex(i));
	return m_vNodes[i];
}


//access
const HalfEdgeTetMesh::ELEM& HalfEdgeTetMesh::const_elemAt(U32 i) const {
	assert(isElemIndex(i));
	return m_vElements[i];
}

const HalfEdgeTetMesh::FACE& HalfEdgeTetMesh::const_faceAt(U32 i) const {
	assert(isFaceIndex(i));
	return m_vFaces[i];
}

const HalfEdgeTetMesh::HEDGE& HalfEdgeTetMesh::const_halfedgeAt(U32 i) const {
	assert(isHalfEdgeIndex(i));
	return m_vHalfEdges[i];
}

const HalfEdgeTetMesh::NODE& HalfEdgeTetMesh::const_nodeAt(U32 i) const {
	assert(isNodeIndex(i));
	return m_vNodes[i];
}

void HalfEdgeTetMesh::displace(double * u) {
	if(u == NULL)
		return;

	for(U32 i=0; i < countNodes(); i++) {
		m_vNodes[i].pos = m_vNodes[i].pos + vec3d(&u[i * 3]);
	}

	computeAABB();
}


bool HalfEdgeTetMesh::split_edge(int idxEdge, double t) {
	if(!isEdgeIndex(idxEdge))
		return false;

	if(t < 0.0 || t > 1.0)
		return false;

	//Find halfedge indices from edge index
	U32 idxHE0 = idxEdge*2;
	U32 idxHE1 = idxHE0 + 1;

	HEDGE& he0 = halfedgeAt(idxHE0);
	HEDGE& he1 = halfedgeAt(idxHE1);

	U32 from = vertex_from_hedge(idxHE0);
	U32 to = vertex_to_hedge(idxHE0);

	NODE n0 = const_nodeAt(from);
	NODE n1 = const_nodeAt(to);

	//add new point
	NODE np;
	np.pos = n0.pos + (n1.pos - n0.pos) * t;
	np.restpos = n0.restpos + (n1.restpos - n0.restpos) * t;
	np.outHE = idxHE1;
	m_vNodes.push_back(np);
	U32 idxNewNode = m_vNodes.size() - 1;

	//Add new halfedges
	HEDGE h2 = he0;
	HEDGE h3 = he1;
	h2.from = idxNewNode;
	h2.prev = idxHE0;
	h3.to 	= idxNewNode;
	h3.next = idxHE1;

	m_vHalfEdges.push_back(h2);
	U32 idxHE2 = m_vHalfEdges.size() - 1;
	m_vHalfEdges.push_back(h3);
	U32 idxHE3 = m_vHalfEdges.size() - 1;

	//update opposites for new edges
	m_vHalfEdges[idxHE2].opposite = idxHE3;
	m_vHalfEdges[idxHE3].opposite = idxHE2;

	//update old halfedges
	he0.to = idxNewNode;
	he0.next = idxHE2;
	he1.from = idxNewNode;
	he1.prev = idxHE3;
	m_vHalfEdges[idxHE0] = he0;
	m_vHalfEdges[idxHE1] = he1;


	return true;
}

bool HalfEdgeTetMesh::cut_edge(int idxEdge, double t, U32* poutIndexNP0, U32* poutIndexNP1) {
	if(!isEdgeIndex(idxEdge))
		return false;

	if(t < 0.0 || t > 1.0)
		return false;

	//Find halfedge indices from edge index
	U32 idxHE0 = idxEdge*2;
	U32 idxHE1 = idxHE0 + 1;

	HEDGE& he0 = halfedgeAt(idxHE0);
	HEDGE& he1 = halfedgeAt(idxHE1);

	U32 from = vertex_from_hedge(idxHE0);
	U32 to = vertex_to_hedge(idxHE0);

	NODE n0 = const_nodeAt(from);
	NODE n1 = const_nodeAt(to);

	//add two new point
	NODE np0;
	np0.pos = n0.pos + (n1.pos - n0.pos) * t;
	np0.restpos = n0.restpos + (n1.restpos - n0.restpos) * t;
	np0.outHE = idxHE1;
	m_vNodes.push_back(np0);
	U32 idxNP0 = m_vNodes.size() - 1;

	NODE np1 = np0;
	np1.outHE = INVALID_INDEX;
	m_vNodes.push_back(np1);
	U32 idxNP1 = m_vNodes.size() - 1;


	//Add new halfedges
	HEDGE he2 = he0;
	HEDGE he3 = he1;
	he2.from = idxNP1;
	he2.prev = INVALID_INDEX;

	he3.to 	= idxNP1;
	he3.next = INVALID_INDEX;

	m_vHalfEdges.push_back(he2);
	U32 idxHE2 = m_vHalfEdges.size() - 1;
	m_vHalfEdges.push_back(he3);
	U32 idxHE3 = m_vHalfEdges.size() - 1;

	//update opposites for new edges
	m_vHalfEdges[idxHE2].opposite = idxHE3;
	m_vHalfEdges[idxHE3].opposite = idxHE2;

	//update outHE for new nodes
	m_vNodes[idxNP0].outHE = idxHE1;
	m_vNodes[idxNP1].outHE = idxHE2;

	//update old halfedges
	he0.to = idxNP0;
	he0.next = INVALID_INDEX;
	he1.from = idxNP0;
	he1.prev = INVALID_INDEX;
	m_vHalfEdges[idxHE0] = he0;
	m_vHalfEdges[idxHE1] = he1;

	//Write output vertices
	if(poutIndexNP0)
		*poutIndexNP0 = idxNP0;

	if(poutIndexNP1)
		*poutIndexNP1 = idxNP1;

	return true;
}

int HalfEdgeTetMesh::getFirstRing(int idxNode, vector<U32>& ringNodes) const {
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

int HalfEdgeTetMesh::getIncomingHalfEdges(int idxNode, vector<U32>& incomingHE) const {
	assert(isNodeIndex(idxNode));

	HalfEdgeTetMesh::NODE n = const_nodeAt(idxNode);

	U32 inHE_orig = opposite_hedge(n.outHE);
	U32 inHE = inHE_orig;

	do {
		if(isHalfEdgeIndex(inHE))
			incomingHE.push_back(inHE);
		else {
			LogError("Not a valid halfedge index");
			break;
		}
		inHE = opposite_hedge(next_hedge(inHE));
	} while(inHE != inHE_orig);

	return (int)incomingHE.size();
}

int HalfEdgeTetMesh::getOutgoingHalfEdges(int idxNode, vector<U32>& outgoingHE) const {
	assert(isNodeIndex(idxNode));

	HalfEdgeTetMesh::NODE n = const_nodeAt(idxNode);
	U32 outHE_orig = n.outHE;
	U32 outHE = outHE_orig;

	do {
		if(isHalfEdgeIndex(outHE))
			outgoingHE.push_back(outHE);
		else {
			LogError("Not a valid halfedge index");
			break;
		}

		outHE = next_hedge(opposite_hedge(outHE));
	} while(outHE != outHE_orig);

	return (int)outgoingHE.size();
}


bool HalfEdgeTetMesh::checkMeshConnectivity() const {
	return false;
}

bool HalfEdgeTetMesh::checkMeshFaceDirections() const {

	U32 ctNonTriangle = 0;
	U32 ctNotCCW = 0;
	vec3d p[3];

	HalfEdgeTetMesh::HEDGE hedge;

	for(U32 i = 0; i < countFaces(); i++) {
		HalfEdgeTetMesh::FACE face = const_faceAt(i);
		U32 he0 = face.halfedge[0];
		U32 he1 = he0;
		U32 ctEdges = 0;

		do {
			hedge = const_halfedgeAt(he1);
			if(ctEdges < 3)
				p[ctEdges] = const_nodeAt(hedge.from).pos;

			he1 = hedge.next;
			ctEdges ++;
		}
		while(he0 != he1);

		if(ctEdges > 3) {
			LogErrorArg2("Face %d is not triangulated. Num Edges: %d", i, ctEdges);
			ctNonTriangle++;
		}
	}

	if(ctNonTriangle > 0 || ctNotCCW > 0)
		return false;
	else
		return true;
}

bool HalfEdgeTetMesh::readVegaFormat(const AnsiStr& strFP) {
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

bool HalfEdgeTetMesh::writeVegaFormat(const AnsiStr& strFP) const {
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
		ELEM elem = const_elemAt(i);
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


void HalfEdgeTetMesh::draw() {
	vec3d p0;
	HalfEdgeTetMesh::HEDGE hedge;
	int count = 0;

	glPushAttrib(GL_ALL_ATTRIB_BITS);
	glDisable(GL_LIGHTING);

	//Draw filled faces
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glColor3f(0.0f, 0.0f, 1.0f);
	glBegin(GL_TRIANGLES);
		for(U32 i=0; i<m_vFaces.size(); i++) {
			HalfEdgeTetMesh::FACE face = const_faceAt(i);
			HalfEdgeTetMesh::HEDGE he0 = const_halfedgeAt(face.halfedge[0]);
			HalfEdgeTetMesh::HEDGE he1 = const_halfedgeAt(face.halfedge[1]);
			HalfEdgeTetMesh::HEDGE he2 = const_halfedgeAt(face.halfedge[2]);
			vec3d p0 = const_nodeAt(he0.from).pos;
			vec3d p1 = const_nodeAt(he1.from).pos;
			vec3d p2 = const_nodeAt(he2.from).pos;

			glVertex3d(p0.x, p0.y, p0.z);
			glVertex3d(p1.x, p1.y, p1.z);
			glVertex3d(p2.x, p2.y, p2.z);
		}
	glEnd();

	/*
	glBegin(GL_POLYGON);
	for(U32 i=0; i<m_vFaces.size(); i++) {
		HalfEdgeTetMesh::FACE face = const_faceAt(i);
		U32 he0 = face.halfedge[0];
		U32 he1 = he0;
		count = 0;

		do {
			hedge = halfedgeAt(he1);
			p0 = const_nodeAt(hedge.from).pos;
			glVertex3d(p0.x, p0.y, p0.z);

			he1 = hedge.next;
			count ++;
		}
		while(he0 != he1);
	}
	glEnd();
	*/

	//Draw outlined faces
	glLineWidth(3.0f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glColor3f(0.0f, 0.0f, 0.0f);
	glBegin(GL_TRIANGLES);
		for(U32 i=0; i<m_vFaces.size(); i++) {
			HalfEdgeTetMesh::FACE face = const_faceAt(i);
			HalfEdgeTetMesh::HEDGE he0 = const_halfedgeAt(face.halfedge[0]);
			HalfEdgeTetMesh::HEDGE he1 = const_halfedgeAt(face.halfedge[1]);
			HalfEdgeTetMesh::HEDGE he2 = const_halfedgeAt(face.halfedge[2]);
			vec3d p0 = const_nodeAt(he0.from).pos;
			vec3d p1 = const_nodeAt(he1.from).pos;
			vec3d p2 = const_nodeAt(he2.from).pos;

			glVertex3d(p0.x, p0.y, p0.z);
			glVertex3d(p1.x, p1.y, p1.z);
			glVertex3d(p2.x, p2.y, p2.z);
		}
	glEnd();

	/*
	glBegin(GL_POLYGON);
	for(U32 i=0; i<m_vFaces.size(); i++) {
		HalfEdgeTetMesh::FACE face = const_faceAt(i);
		U32 he0 = face.halfedge[0];
		U32 he1 = he0;

		do {
			hedge = halfedgeAt(he1);
			p0 = const_nodeAt(hedge.from).pos;
			glVertex3d(p0.x, p0.y, p0.z);

			he1 = hedge.next;
		}
		while(he0 != he1);
	}
	glEnd();
	*/

	//Draw vertices
	glPointSize(5.0f);
	glColor3f(1.0f, 0.0f, 0.0f);
	glBegin(GL_POINTS);
	for(U32 i=0; i < m_vNodes.size(); i++) {
		HalfEdgeTetMesh::NODE n = const_nodeAt(i);
		glVertex3dv(n.pos.cptr());
	}
	glEnd();

	glEnable(GL_LIGHTING);
	glPopAttrib();
}


AABB HalfEdgeTetMesh::computeAABB() {
	double vMin[3], vMax[3];
	for(U32 i=0; i < countNodes(); i++) {
		HalfEdgeTetMesh::NODE n = const_nodeAt(i);
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


}
}
