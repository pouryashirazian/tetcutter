/*
 * TopologyModifier.cpp
 *
 *  Created on: Apr 13, 2014
 *      Author: pourya
 */

#include "CuttableMesh.h"
#include "graphics/selectgl.h"
#include "graphics/Intersections.h"
#include "deformable/test_HalfEdgeTetMesh.h"
#include "base/Logger.h"
#include <map>

using namespace std;
using namespace PS::INTERSECTIONS;

namespace PS {

///////////////////////////////////////////////////////////////////////////
CuttableMesh::CuttableMesh(const HalfEdgeTetMesh& hemesh) {
	resetTransform();
	if(TheShaderManager::Instance().has("phong")) {
        m_spEffect = SmartPtrSGEffect(new SGEffect(TheShaderManager::Instance().get("phong")));
    }

	//HEMesh
	m_lpHEMesh = new HalfEdgeTetMesh(hemesh);
	m_lpSubD = new TetSubdivider(m_lpHEMesh);

	m_aabb = m_lpHEMesh->aabb();
	m_ctCompletedCuts = 0;

}

CuttableMesh::CuttableMesh(const vector<double>& vertices, const vector<U32>& elements)
 /* : TetMesh(vertices.size() / 3, const_cast<double *>(&vertices[0]), elements.size() / 4, (int*) &elements[0]) */ {

	setup(vertices.size() / 3, const_cast<double*>(&vertices[0]), elements.size() / 4, (int*) &elements[0]);
}

CuttableMesh::CuttableMesh(int ctVertices, double* vertices, int ctElements, int* elements)
/*	: TetMesh(ctVertices, vertices, ctElements, elements) */ {

	setup(ctVertices, vertices, ctElements, elements);
}

CuttableMesh::~CuttableMesh() {
	SAFE_DELETE(m_lpSubD);
	SAFE_DELETE(m_lpHEMesh);
}

void CuttableMesh::setup(int ctVertices, double* vertices, int ctElements, int* elements) {

	resetTransform();
	if(TheShaderManager::Instance().has("phong")) {
        m_spEffect = SmartPtrSGEffect(new SGEffect(TheShaderManager::Instance().get("phong")));
    }

	//HEMesh
	m_lpHEMesh = new HalfEdgeTetMesh(ctVertices, vertices, ctElements, (U32*)elements);

	//Perform all tests
	LogInfo("Begin testing the halfedge mesh");
	TestHalfEdgeTestMesh::tst_all(m_lpHEMesh);
	LogInfo("Test completed");

	//
	m_lpSubD = new TetSubdivider(m_lpHEMesh);

	m_aabb = m_lpHEMesh->aabb();
	m_ctCompletedCuts = 0;
}

void CuttableMesh::clearCutContext() {
	m_mapCutEdges.clear();
	m_mapCutNodes.clear();
}

void CuttableMesh::draw() {

	drawBBox();

	//draw tetmesh
	if(m_lpHEMesh)
		m_lpHEMesh->draw();

	//draw cut context
	int ctCutEdges = m_mapCutEdges.size();
	int ctCutNodes = m_mapCutNodes.size();
	if(ctCutNodes > 0 || ctCutEdges > 0) {
		glDisable(GL_LIGHTING);
		glPushAttrib(GL_ALL_ATTRIB_BITS);

			//Draw cut edges
			glColor3f(0.7, 0.7, 0.7);
			glLineWidth(6.0f);
			glBegin(GL_LINES);
				for(CUTEDGEITER it = m_mapCutEdges.begin(); it != m_mapCutEdges.end(); ++it) {
					glVertex3dv(it->second.e0.cptr());
					glVertex3dv(it->second.e1.cptr());
				}
			glEnd();


			//Draw nodes
			glPointSize(7.0f);
			glColor3f(0.0, 0.0, 1.0);
			glBegin(GL_POINTS);
			//Draw cutedges crossing
			for(CUTEDGEITER it = m_mapCutEdges.begin(); it != m_mapCutEdges.end(); ++it) {
				glVertex3dv(it->second.pos.cptr());
			}
			glEnd();

			glColor3f(0, 0, 0);
			glBegin(GL_POINTS);
			//Draw cutnodes
			for(CUTNODEITER it = m_mapCutNodes.begin(); it != m_mapCutNodes.end(); ++it) {
				glVertex3dv(it->second.pos.cptr());
			}
			glEnd();

		glPopAttrib();
		glEnable(GL_LIGHTING);

	}
}

int CuttableMesh::cut(const vector<vec3d>& bladePath0,
					  const vector<vec3d>& bladePath1,
					  vec3d sweptSurface[4], bool modifyMesh) {

	//if the swept surface is degenerate then return
	double area = (sweptSurface[1] - sweptSurface[0]).length2() * (sweptSurface[2] - sweptSurface[1]).length2();
	if(area < EPSILON)
		return -1;
	double l2 = (sweptSurface[3] - sweptSurface[2]).length2();
	if(l2 < EPSILON)
		return -1;



	//1.Compute all cut-edges
	//2.Compute cut nodes and remove all incident edges to cut nodes from cut edges
	//3.split cut edges and compute the reference position of the split point
	//4.duplicate cut nodes and incident edges

	// iterate over all edges

	vec3d uvw, xyz, ss0, ss1;
	double t;

	vec3d tri1[3] = {sweptSurface[0], sweptSurface[1], sweptSurface[2]};
	vec3d tri2[3] = {sweptSurface[0], sweptSurface[2], sweptSurface[3]};

	//Radios of Influence in percent
	const double roi = 0.2;

	//Swept Surf
	printf("SWEPT SURF[0]: %.3f, %.3f, %.3f\n", sweptSurface[0].x, sweptSurface[0].y, sweptSurface[0].z);
	printf("SWEPT SURF[2]: %.3f, %.3f, %.3f\n", sweptSurface[2].x, sweptSurface[2].y, sweptSurface[2].z);

	//Cut-Edges
	for (U32 i=0; i < m_lpHEMesh->countEdges(); i++) {

		U32 hei = m_lpHEMesh->halfedge_from_edge(i, 0);
		HalfEdgeTetMesh::HEDGE he = m_lpHEMesh->const_halfedgeAt(hei);


		ss0 = m_lpHEMesh->const_nodeAt(he.from).pos;
		ss1 = m_lpHEMesh->const_nodeAt(he.to).pos;

		int res = IntersectSegmentTriangle(ss0, ss1, tri1, t, uvw, xyz);
		if(res == 0)
			res = IntersectSegmentTriangle(ss0, ss1, tri2, t, uvw, xyz);
		if(res > 0) {
			CutEdge ce;
			ce.pos = xyz;
			ce.uvw = uvw;
			ce.e0 = ss0;
			ce.e1 = ss1;
			ce.t = t;
			ce.idxE0 = he.from;
			ce.idxE1 = he.to;
			ce.idxEdge = i;

			//test
			vec3d temp = ce.e0 + (ce.e1 - ce.e0).normalized() * ce.t;
			assert( (ce.pos - temp).length() < EPSILON);

			m_mapCutEdges[ce.idxEdge] = ce;
		}
	}


	//blade
	vec3d blade0 = bladePath0[bladePath0.size() - 1];
	vec3d blade1 = bladePath1[bladePath1.size() - 1];
	const double edgelen2 = (blade1 - blade0).length2();

	std::map< U32, CutEdge > finalCutEdges;
	finalCutEdges.insert(m_mapCutEdges.begin(), m_mapCutEdges.end());

	int ctRemovedCutEdges = 0;
	m_mapCutNodes.clear();

	//detect all cut-nodes and remove the cut-edges that emanate from a cut-node
	CUTEDGEITER it = m_mapCutEdges.begin();
	for(it = m_mapCutEdges.begin(); it != m_mapCutEdges.end(); ++it ) {

		ss0 = it->second.e0;
		ss1 = it->second.e1;
		double d0 = pointLineDistance(blade0, blade1, edgelen2, ss0);
		double d1 = pointLineDistance(blade0, blade1, edgelen2, ss1);
		double denom = (ss1 - ss0).length();
		if(denom == 0)
			denom = 1;

		double t = 10.0;
		if(d0 < d1)
			t = (it->second.pos - ss0).length() / denom;
		else
			t = (it->second.pos - ss1).length() / denom;

		//If the start of edge is close to the swept surface remove all incident edges from Ec
		if(d0 < d1 && t < roi) {
			CutNode cn;
			cn.idxNode = it->second.idxE0;
			cn.pos = it->second.e0;
			m_mapCutNodes.insert( std::pair<U32, CutNode>(cn.idxNode, cn) );

			//iterate over all incoming half-edges
			vector<U32> incomingHE;
			m_lpHEMesh->getIncomingHalfEdges(cn.idxNode, incomingHE);

			for(U32 i=0; i < incomingHE.size(); i++) {
				finalCutEdges.erase(m_lpHEMesh->edge_from_halfedge(incomingHE[i]));
				ctRemovedCutEdges++;
			}
		}
		//If the end of edge close to the swept surface remove all incident edges from Ec
		else if(d0 > d1 && t < roi) {
			CutNode cn;
			cn.idxNode = it->second.idxE1;
			cn.pos = it->second.e1;
			m_mapCutNodes.insert( std::pair<U32, CutNode>(cn.idxNode, cn) );

			//iterate over all incoming half-edges
			vector<U32> incomingHE;
			m_lpHEMesh->getIncomingHalfEdges(cn.idxNode, incomingHE);

			for(U32 i=0; i < incomingHE.size(); i++) {
				finalCutEdges.erase(m_lpHEMesh->edge_from_halfedge(incomingHE[i]));
				ctRemovedCutEdges++;
			}
		}
	}

	//Copy
	m_mapCutEdges.clear();
	m_mapCutEdges.insert(finalCutEdges.begin(), finalCutEdges.end());
	if(m_mapCutNodes.size() > 0)
		printf("Cut nodes count %d.\n", (int)m_mapCutNodes.size());
	if(m_mapCutEdges.size() > 0)
		printf("Cut edges count %d. removed %d\n", (int)m_mapCutEdges.size(), ctRemovedCutEdges);

	//Return if the tool has not left the body
	if(!modifyMesh)
		return -1;

	//Find the list of all tets impacted
	vector<U32> vCutElements;
	vector<U8> vCutEdgeCodes;
	vector<U8> vCutNodeCodes;
	vector<double> vCutParams;
	vCutElements.reserve(128);
	vCutEdgeCodes.reserve(128);
	vCutNodeCodes.reserve(128);
	vCutParams.reserve(128 * 6);

	//count of tets cut
	int ctCutTet = 0;

	for(U32 i=0; i < m_lpHEMesh->countElements(); i++) {
		const HalfEdgeTetMesh::ELEM& tet = m_lpHEMesh->const_elemAt(i);
		U8 cutEdgeCode = 0;
		U8 cutNodeCode = 0;
		double tedges[6];

		//compute cutedge code
		for(int e=0; e < 6; e++) {
			tedges[e] = 0.0;
			U32 edge = m_lpHEMesh->edge_from_halfedge(tet.halfedge[e]);
			if(m_mapCutEdges.find(edge) != m_mapCutEdges.end()) {
				cutEdgeCode |= (1 << e);
				tedges[e] = m_mapCutEdges[edge].t;
			}
		}

		//compute cut node code
		for(int e=0; e < 4; e++) {
			U32 node = tet.nodes[e];
			if(m_mapCutNodes.find(node) != m_mapCutNodes.end()) {
				cutNodeCode |= (1 << e);
			}
		}

		if(cutEdgeCode != 0 || cutNodeCode != 0)
			ctCutTet++;

		//push back all computed values
		vCutElements.push_back(i);
		vCutEdgeCodes.push_back(cutEdgeCode);
		vCutNodeCodes.push_back(cutNodeCode);

		//push back param t
		for(int e=0; e < 6; e++)
			vCutParams.push_back(tedges[e]);
	}

	//	int edgeMaskPos[6][2] = { {1, 2}, {2, 3}, {3, 1}, {2, 0}, {0, 3}, {0, 1} };
	//	int edgeMaskNeg[6][2] = { {3, 2}, {2, 1}, {1, 3}, {3, 0}, {0, 2}, {1, 0} };
	//	int faceMaskPos[4][3] = { {1, 2, 3}, {2, 0, 3}, {3, 0, 1}, {1, 0, 2} };
	//	int faceMaskNeg[4][3] = { {3, 2, 1}, {3, 0, 2}, {1, 0, 3}, {2, 0, 1} };

	//Now that cutedgecodes and cutnodecodes are computed then subdivide the element
	LogInfoArg1("BEGIN CUTTING# %u", m_ctCompletedCuts+1);

	U32 ctElementsCut = 0;
	for(U32 i=0; i < vCutElements.size(); i++) {
		if(vCutEdgeCodes[i] != 0 || vCutNodeCodes[i] != 0) {

			//subdivide the element
			ctElementsCut += m_lpSubD->subdivide(vCutElements[i], vCutEdgeCodes[i], vCutNodeCodes[i], &vCutParams[i * 6], true);
		}
	}

	//increment completed cuts
	if(ctElementsCut > 0) {
		LogInfoArg2("END CUTTING# %u: subdivided elements count: %u.", m_ctCompletedCuts + 1, ctElementsCut);
		m_ctCompletedCuts ++;
	}
	else {
		LogWarningArg1("END CUTTING# %u: No elements are subdivided.", m_ctCompletedCuts + 1);
	}


	//clear cut context
	clearCutContext();

	//remove unref
	//m_lpHEMesh->remove_unreferenced();

	//Perform all tests
	//TestHalfEdgeTestMesh::tst_all(m_lpHEMesh);

	//recompute AABB
	m_aabb = m_lpHEMesh->computeAABB();

	//Return number of tets cut
	return ctCutTet;
}

void CuttableMesh::displace(double * u) {
	m_lpHEMesh->displace(u);
	m_aabb = m_lpHEMesh->aabb();
}


U32 CuttableMesh::countVertices() const {
	return m_lpHEMesh->countNodes();
}

vec3d CuttableMesh::vertexRestPosAt(U32 i) const {
	return m_lpHEMesh->const_nodeAt(i).restpos;
}

vec3d CuttableMesh::vertexAt(U32 i) const {
	double v[3];
//	TetMesh::getVertex(i)->convertToArray(v);
	return vec3d(v);
}

int CuttableMesh::findClosestVertex(const vec3d& query, double& dist, vec3d& outP) const {
	double minDist = GetMaxLimit<double>();
	int idxFound = -1;
	for(U32 i=0; i < m_lpHEMesh->countNodes(); i++) {
		vec3d p = m_lpHEMesh->const_nodeAt(i).pos;
		double dist2 = (query - p).length2();
		if (dist2 < minDist) {
			minDist = dist2;
			outP = p;
			idxFound = i;
		}
	}

	//Distance
	dist = sqrt(minDist);
	return idxFound;
}

double CuttableMesh::pointLineDistance(const vec3d& v1, const vec3d& v2, const vec3d& p) {
	const double d2 = (v2 - v1).length2();
	if(d2 == 0.0) return vec3d::distance(p, v1);
	return pointLineDistance(v1, v2, d2, p);
}

double CuttableMesh::pointLineDistance(const vec3d& v1, const vec3d& v2,
									   const double len2, const vec3d& p, double* outT) {
	// Consider the line extending the segment, parameterized as v1 + t (v2 - v1).
	// We find projection of point p onto the line.
	// It falls where t = [(p-1) . (v2-v1)] / |v2-v1|^2
	const double t = vec3d::dot(p - v1, v2 - v1) / len2;
	if(outT)
		*outT = t;
	if (t < 0.0)
		return vec3d::distance(p, v1);
	else if (t > 1.0)
		return vec3d::distance(p, v2);

	// Projection falls on the segment
	const vec3d projection = v1 + (v2 - v1) * t;
	return vec3d::distance(p, projection);
}


CuttableMesh* CuttableMesh::CreateOneTetra() {
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

	CuttableMesh* tet = new CuttableMesh(vertices, elements);
	return tet;
}

CuttableMesh* CuttableMesh::CreateTwoTetra() {
	vector<double> vertices;
	vector<U32> elements;

	vector<vec3d> vPoints;
	vPoints.push_back(vec3d(-1, 0, 0));
	vPoints.push_back(vec3d(0, 0, -2));
	vPoints.push_back(vec3d(1, 0, 0));
	vPoints.push_back(vec3d(0, 2, -1));
	vPoints.push_back(vec3d(0, 0, 2));

	vertices.resize(vPoints.size() * 3);
	for (U32 i = 0; i < vPoints.size(); i++) {
		vPoints[i].store(&vertices[i * 3]);
	}

	elements.resize(8);
	elements[0] = 0;
	elements[1] = 1;
	elements[2] = 2;
	elements[3] = 3;

	elements[4] = 0;
	elements[5] = 2;
	elements[6] = 3;
	elements[7] = 4;

	CuttableMesh* tet = new CuttableMesh(vertices, elements);
	return tet;
}

}


