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
#include "base/FlatArray.h"
#include <map>

using namespace std;
using namespace PS::INTERSECTIONS;

namespace PS {

///////////////////////////////////////////////////////////////////////////
CuttableMesh::CuttableMesh(const VolMesh& hemesh) {
	resetTransform();
	if(TheShaderManager::Instance().has("phong")) {
        m_spEffect = SmartPtrSGEffect(new SGEffect(TheShaderManager::Instance().get("phong")));
    }

	//HEMesh
	m_lpVolMesh = new VolMesh(hemesh);
	m_lpSubD = new TetSubdivider(m_lpVolMesh);

	m_aabb = m_lpVolMesh->aabb();
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
	SAFE_DELETE(m_lpVolMesh);
}

void CuttableMesh::setup(int ctVertices, double* vertices, int ctElements, int* elements) {

	resetTransform();
	if(TheShaderManager::Instance().has("phong")) {
        m_spEffect = SmartPtrSGEffect(new SGEffect(TheShaderManager::Instance().get("phong")));
    }

	//HEMesh
	m_lpVolMesh = new VolMesh(ctVertices, vertices, ctElements, (U32*)elements);

	//Perform all tests
	LogInfo("Begin testing the halfedge mesh");
	TestVolMesh::tst_all(m_lpVolMesh);
	LogInfo("Test completed");

	//
	m_lpSubD = new TetSubdivider(m_lpVolMesh);

	m_aabb = m_lpVolMesh->aabb();
	m_aabb.expand(1.0);
	m_ctCompletedCuts = 0;
}

void CuttableMesh::clearCutContext() {
	m_mapCutEdges.clear();
	m_mapCutNodes.clear();
}

void CuttableMesh::draw() {

	drawBBox();

	//draw tetmesh
	if(m_lpVolMesh)
		m_lpVolMesh->draw();

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
	m_mapCutEdges.clear();
	std::map< U32, CutEdge > mapTempCutEdges;
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
	for (U32 i=0; i < m_lpVolMesh->countEdges(); i++) {

		EDGE e = m_lpVolMesh->const_edgeAt(i);


		ss0 = m_lpVolMesh->const_nodeAt(e.from).pos;
		ss1 = m_lpVolMesh->const_nodeAt(e.to).pos;

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
			ce.from = e.from;
			ce.to = e.to;

			//test
			vec3d temp = ce.e0 + (ce.e1 - ce.e0).normalized() * ce.t;
			assert( (ce.pos - temp).length() < EPSILON);

			//add to cut edges map
			mapTempCutEdges.insert( std::make_pair(i, ce));
		}
	}


	//blade
	vec3d blade0 = bladePath0[bladePath0.size() - 1];
	vec3d blade1 = bladePath1[bladePath1.size() - 1];
	const double edgelen2 = (blade1 - blade0).length2();

	int ctRemovedCutEdges = 0;
	m_mapCutNodes.clear();

	//detect all cut-nodes and remove the cut-edges that emanate from a cut-node
	for(CUTEDGEITER it = mapTempCutEdges.begin(); it != mapTempCutEdges.end(); ++it ) {

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
			cn.idxNode = it->second.from;
			cn.pos = it->second.e0;
			m_mapCutNodes.insert( std::pair<U32, CutNode>(cn.idxNode, cn) );

			//iterate over all incident edges
			vector<U32> incidentEdges;
			m_lpVolMesh->getNodeIncidentEdges(cn.idxNode, incidentEdges);

			for(U32 i=0; i < incidentEdges.size(); i++) {
				mapTempCutEdges.erase(incidentEdges[i]);
				ctRemovedCutEdges++;
			}
		}
		//If the end of edge close to the swept surface remove all incident edges from Ec
		else if(d0 > d1 && t < roi) {
			CutNode cn;
			cn.idxNode = it->second.to;
			cn.pos = it->second.e1;
			m_mapCutNodes.insert( std::pair<U32, CutNode>(cn.idxNode, cn) );

			//iterate over all incident edges
			vector<U32> incidentEdges;
			m_lpVolMesh->getNodeIncidentEdges(cn.idxNode, incidentEdges);

			for(U32 i=0; i < incidentEdges.size(); i++) {
				mapTempCutEdges.erase(incidentEdges[i]);
				ctRemovedCutEdges++;
			}
		}
	}

	//Copy
	m_mapCutEdges.clear();
	m_mapCutEdges.insert(mapTempCutEdges.begin(), mapTempCutEdges.end());
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

	for(U32 i=0; i < m_lpVolMesh->countCells(); i++) {
		const CELL& cell = m_lpVolMesh->const_cellAt(i);
		U8 cutEdgeCode = 0;
		U8 cutNodeCode = 0;

		double tedges[6];

		//compute cutedge code
		for(int e=0; e < 6; e++) {
			tedges[e] = 0.0;
			U32 edge = cell.edges[e];
			if(m_mapCutEdges.find(edge) != m_mapCutEdges.end()) {
				cutEdgeCode |= (1 << e);
				tedges[e] = m_mapCutEdges[edge].t;
			}
		}

		//compute cut node code
		for(int e=0; e < 4; e++) {
			U32 node = cell.nodes[e];
			if(m_mapCutNodes.find(node) != m_mapCutNodes.end()) {
				cutNodeCode |= (1 << e);
			}
		}

		//if there is a cut in this cell
		if(cutEdgeCode != 0 || cutNodeCode != 0) {

			//increment cut tets
			ctCutTet++;

			//push back all computed values
			vCutElements.push_back(i);
			vCutEdgeCodes.push_back(cutEdgeCode);
			vCutNodeCodes.push_back(cutNodeCode);
			//push back param t
			for(int e=0; e < 6; e++)
				vCutParams.push_back(tedges[e]);

			//check if the codes are implemented already
			TetSubdivider::CUTCASE cc = m_lpSubD->IdentifyCutCase(true, cutEdgeCode, cutNodeCode);
			char chrCutCase = m_lpSubD->toAlpha(cc);
			if(chrCutCase != 'A' && chrCutCase != 'B') {
				LogErrorArg3("This cut contains a cut case which is not handled yet. case: %c, cutEdgeCode: %x, cutNodeCode: %x",
							 chrCutCase, cutEdgeCode, cutNodeCode);
				return -1;
			}
		}
	}

	//	int edgeMaskPos[6][2] = { {1, 2}, {2, 3}, {3, 1}, {2, 0}, {0, 3}, {0, 1} };
	//	int edgeMaskNeg[6][2] = { {3, 2}, {2, 1}, {1, 3}, {3, 0}, {0, 2}, {1, 0} };
	//	int faceMaskPos[4][3] = { {1, 2, 3}, {2, 0, 3}, {3, 0, 1}, {1, 0, 2} };
	//	int faceMaskNeg[4][3] = { {3, 2, 1}, {3, 0, 2}, {1, 0, 3}, {2, 0, 1} };

	//Now that cutedgecodes and cutnodecodes are computed then subdivide the element
	LogInfoArg1("BEGIN CUTTING# %u", m_ctCompletedCuts+1);

	//cut all affected edges
	for(CUTEDGEITER it = m_mapCutEdges.begin(); it != m_mapCutEdges.end(); it++) {
		U32 idxNP0, idxNP1;

		if(!m_lpVolMesh->cut_edge(it->first, it->second.t, &idxNP0, &idxNP1)) {
			LogErrorArg2("Unable to cut edge %d, edgecutpoint t = %.3f.", it->first, it->second.t);
			return -1;
		}

		it->second.idxNP0 = idxNP0;
		it->second.idxNP1 = idxNP1;
	}

	//
	U32 ctElementsCut = 0;
	U32 middlePoints[12];

	for(U32 i=0; i < vCutElements.size(); i++) {

		U8 cutEdgeCode = vCutEdgeCodes[i];
		U8 cutNodeCode = vCutNodeCodes[i];
		if(cutEdgeCode != 0 || cutNodeCode != 0) {

			const CELL& cell = m_lpVolMesh->const_cellAt(vCutElements[i]);


			for(int e=0; e < 6; e++) {

				CUTEDGEITER it = m_mapCutEdges.find(cell.edges[e]);

				//mid points
				middlePoints[e * 2 + 0] = VolMesh::INVALID_INDEX;
				middlePoints[e * 2 + 1] = VolMesh::INVALID_INDEX;

				if(it != m_mapCutEdges.end()) {

					middlePoints[e * 2 + 0] = it->second.idxNP0;
					middlePoints[e * 2 + 1] = it->second.idxNP1;
				}
			}
			//subdivide the element
			ctElementsCut += m_lpSubD->subdivide(vCutElements[i], cutEdgeCode, cutNodeCode, middlePoints, true);
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

	//collect all garbage
	//m_lpVolMesh->garbage_collection();

	//Perform all tests
	TestVolMesh::tst_all(m_lpVolMesh);


	//recompute AABB and expand it to detect cuts
	m_aabb = m_lpVolMesh->computeAABB();
	m_aabb.expand(1.0);

	//Return number of tets cut
	return ctCutTet;
}

void CuttableMesh::displace(double * u) {
	m_lpVolMesh->displace(u);
	m_aabb = m_lpVolMesh->aabb();
}


U32 CuttableMesh::countVertices() const {
	return m_lpVolMesh->countNodes();
}

vec3d CuttableMesh::vertexRestPosAt(U32 i) const {
	return m_lpVolMesh->const_nodeAt(i).restpos;
}

vec3d CuttableMesh::vertexAt(U32 i) const {
	double v[3];
//	TetMesh::getVertex(i)->convertToArray(v);
	return vec3d(v);
}

int CuttableMesh::findClosestVertex(const vec3d& query, double& dist, vec3d& outP) const {
	double minDist = GetMaxLimit<double>();
	int idxFound = -1;
	for(U32 i=0; i < m_lpVolMesh->countNodes(); i++) {
		vec3d p = m_lpVolMesh->const_nodeAt(i).pos;
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
	vector<double> vFlatVertices;
	vector<U32> vFlatElements;

	{
		vector<vec3d> vertices;
		vertices.push_back(vec3d(-1, 0, 0));
		vertices.push_back(vec3d(0, 0, -2));
		vertices.push_back(vec3d(1, 0, 0));
		vertices.push_back(vec3d(0, 2, -1));
		vertices.push_back(vec3d(0, 0, 2));
		FlattenVec3<double>(vertices, vFlatVertices);
	}

	{
		vector< Vec4<U32> > elements;
		elements.push_back( Vec4<U32>(0, 1, 2, 3) );
		elements.push_back( Vec4<U32>(0, 2, 3, 4) );
		FlattenVec4<U32>(elements, vFlatElements);
	}

	CuttableMesh* tet = new CuttableMesh(vFlatVertices, vFlatElements);
	return tet;
}

CuttableMesh* CuttableMesh::CreateTruthCube(int nx, int ny, int nz, double cellsize) {
	if(nx < 2 || ny < 2 || nz < 2) {
		LogError("Invalid input param to create a truth cube");
		return NULL;
	}

	vector< vec3d > vertices;
	vector< vec4u32 > elements;

	vec3d start = vec3d(- (double)(nx)/2.0, 0, - (double)(nz)/2.0) * (cellsize);

	//add all nodes
	for(int i=0; i < nx; i++) {
		for(int j=0; j < ny; j++) {
			for(int k=0; k < nz; k++) {
				vec3d v = start + vec3d(i, j, k) * cellsize;
				vertices.push_back(v);
			}
		}
	}

	//corner defs
	enum CellCorners {LBN = 0, LBF = 1, LTN = 2, LTF = 3, RBN = 4, RBF = 5, RTN = 6, RTF = 7};
	U32 cn[8];
	//add all elements
	for(int i=0; i < nx-1; i++) {
		for(int j=0; j < ny-1; j++) {
			for(int k=0; k < nz-1; k++) {
				//collect cell nodes
				cn[LBN] = i * ny * nz + j * nz + k;
				cn[LBF] = i * ny * nz + j * nz + k + 1;

				cn[LTN] = i * ny * nz + (j+1) * nz + k;
				cn[LTF] = i * ny * nz + (j+1) * nz + k + 1;

				cn[RBN] = (i+1) * ny * nz + j * nz + k;
				cn[RBF] = (i+1) * ny * nz + j * nz + k + 1;

				cn[RTN] = (i+1) * ny * nz + (j+1) * nz + k;
				cn[RTF] = (i+1) * ny * nz + (j+1) * nz + k + 1;

				//add elements
				elements.push_back(vec4u32(cn[LBN], cn[LTN], cn[RBN], cn[LBF]));
				elements.push_back(vec4u32(cn[RTN], cn[LTN], cn[LBF], cn[RBN]));
				elements.push_back(vec4u32(cn[RTN], cn[LTN], cn[LTF], cn[LBF]));
				elements.push_back(vec4u32(cn[RTN], cn[RBN], cn[LBF], cn[RBF]));
				elements.push_back(vec4u32(cn[RTN], cn[LBF], cn[LTF], cn[RBF]));
				elements.push_back(vec4u32(cn[RTN], cn[LTF], cn[RTF], cn[RBF]));
			}
		}
	}

	//build the final mesh
	vector<double> vFlatVertices;
	vector<U32> vFlatElements;
	FlattenVec3<double>(vertices, vFlatVertices);
	FlattenVec4<U32>(elements, vFlatElements);

	CuttableMesh* cube = new CuttableMesh(vFlatVertices, vFlatElements);
	return cube;
}

}


