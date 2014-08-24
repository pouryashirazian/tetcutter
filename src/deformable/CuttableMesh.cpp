/*
 * TopologyModifier.cpp
 *
 *  Created on: Apr 13, 2014
 *      Author: pourya
 */

#include "CuttableMesh.h"
#include "graphics/selectgl.h"
#include "graphics/Intersections.h"
#include "deformable/test_VolMesh.h"
#include "base/Logger.h"
#include "base/FlatArray.h"
#include <map>

using namespace std;
using namespace PS::INTERSECTIONS;

namespace PS {

///////////////////////////////////////////////////////////////////////////
CuttableMesh::CuttableMesh(const VolMesh& volmesh): VolMesh(volmesh) {
	setup();
}

CuttableMesh::CuttableMesh(const vector<double>& vertices, const vector<U32>& elements): VolMesh(vertices, elements) {

	setup();
}

CuttableMesh::CuttableMesh(int ctVertices, double* vertices, int ctElements, int* elements) :
	VolMesh((U32)ctVertices, vertices, (U32)ctElements, reinterpret_cast<U32*>(elements)) {

	setup();
}

CuttableMesh::~CuttableMesh() {
	SAFE_DELETE(m_lpSubD);
	m_vSweepSurfaces.resize(0);
}

void CuttableMesh::setup() {

	resetTransform();
	if(TheShaderManager::Instance().has("phong")) {
        m_spEffect = SmartPtrSGEffect(new SGEffect(TheShaderManager::Instance().get("phong")));
    }

	//Perform all tests
	LogInfo("Begin testing the volumetric mesh");
	TestVolMesh::tst_all(this);
	LogInfo("Test completed");

	//Create subdivider
	m_lpSubD = new TetSubdivider();

	m_aabb = VolMesh::aabb();
	m_aabb.expand(1.0);
	m_ctCompletedCuts = 0;
	m_flagSplitMeshAfterCut = false;
	m_flagDetectCutNodes = false;
	m_flagDrawSweepSurf = false;
}

void CuttableMesh::clearCutContext() {
//	m_mapCutEdges.clear();
//	m_mapCutNodes.clear();
}

void CuttableMesh::draw() {

	drawBBox();

	//draw volmesh
	VolMesh::draw();

	//draw cut context
	int ctCutEdges = m_mapCutEdges.size();
	int ctCutNodes = m_mapCutNodes.size();
	if(ctCutNodes > 0 || ctCutEdges > 0) {
		glDisable(GL_LIGHTING);
		glPushAttrib(GL_ALL_ATTRIB_BITS);

			//Draw cut edges
			glColor3f(1.0, 0.0, 0.0);
			glLineWidth(8.0f);
			glBegin(GL_LINES);
				for(CUTEDGEITER it = m_mapCutEdges.begin(); it != m_mapCutEdges.end(); ++it) {
					U32 from = VolMesh::edge_from_node(it->first);
					U32 to = VolMesh::edge_to_node(it->first);

					glVertex3dv(const_nodeAt(from).pos.cptr());
					glVertex3dv(const_nodeAt(to).pos.cptr());
				}
			glEnd();


			//Draw nodes
			glPointSize(7.0f);
			glBegin(GL_POINTS);
			//Draw cutedges crossing
			for(CUTEDGEITER it = m_mapCutEdges.begin(); it != m_mapCutEdges.end(); ++it) {
				glColor3f(0.0, 0.0, 0.0);
				if(isNodeIndex(it->second.idxNP0))
					glVertex3dv(const_nodeAt(it->second.idxNP0).pos.cptr());

				glColor3f(0.0, 0.0, 1.0);
				glVertex3dv(it->second.pos.cptr());

				glColor3f(0.0, 0.0, 0.0);
				if(isNodeIndex(it->second.idxNP1))
					glVertex3dv(const_nodeAt(it->second.idxNP1).pos.cptr());
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

	if(m_flagDrawSweepSurf) {
		U32 ctCutSurf = m_vSweepSurfaces.size() / 12;

		glDisable(GL_CULL_FACE);
		glDisable(GL_LIGHTING);
		glPushAttrib(GL_ALL_ATTRIB_BITS);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_BLEND);

		glColor4f(1.0, 0.0, 0.0, 0.3);
		glBegin(GL_QUADS);

		//vec3d p[4];
		//draw cut surfaces
		for(U32 i=0; i < ctCutSurf; i++) {
			for(U32 j=0; j < 4; j++) {
				glVertex3dv(&m_vSweepSurfaces[i * 12 + j * 3]);
			}
		}
		glEnd();

		glDisable(GL_BLEND);
		glPopAttrib();
		glEnable(GL_LIGHTING);
		glEnable(GL_CULL_FACE);
	}
}

int CuttableMesh::cut(const vector<vec3d>& bladePath0,
					  const vector<vec3d>& bladePath1,
					  const vector<vec3d>& sweptSurface,
					  bool modifyMesh) {

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

	vec3d sweptSurfNormal = vec3d::cross(sweptSurface[1] - sweptSurface[0], sweptSurface[3] - sweptSurface[0]);
	sweptSurfNormal.normalize();

	//Radios of Influence in percent
	const double roi = 0.2;

	//Swept Surf
	printf("SWEPT SURF[0]: %.3f, %.3f, %.3f\n", sweptSurface[0].x, sweptSurface[0].y, sweptSurface[0].z);
	printf("SWEPT SURF[2]: %.3f, %.3f, %.3f\n", sweptSurface[2].x, sweptSurface[2].y, sweptSurface[2].z);

	//Cut-Edges
	for (U32 i=0; i < this->countEdges(); i++) {

		const EDGE& e = this->const_edgeAt(i);


		ss0 = this->const_nodeAt(e.from).pos;
		ss1 = this->const_nodeAt(e.to).pos;

		int res = IntersectSegmentTriangle(ss0, ss1, tri1, t, uvw, xyz);
		if(res == 0)
			res = IntersectSegmentTriangle(ss0, ss1, tri2, t, uvw, xyz);
		if(res > 0) {
			CutEdge ce;
			ce.pos = xyz;
			ce.uvw = uvw;
			ce.t = t;

			//test
			vec3d temp = ss0 + (ss1 - ss0).normalized() * t;
			assert( (xyz - temp).length() < EPSILON);

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
	if (m_flagDetectCutNodes) {
		for (CUTEDGEITER it = mapTempCutEdges.begin();
				it != mapTempCutEdges.end(); ++it) {

			const EDGE& cutedge = const_edgeAt(it->first);
			ss0 = const_nodeAt(cutedge.from).pos;
			ss1 = const_nodeAt(cutedge.to).pos;
			double d0 = pointLineDistance(blade0, blade1, edgelen2, ss0);
			double d1 = pointLineDistance(blade0, blade1, edgelen2, ss1);
			double denom = (ss1 - ss0).length();
			if (denom == 0)
				denom = 1;

			double t = 10.0;
			if (d0 < d1)
				t = (it->second.pos - ss0).length() / denom;
			else
				t = (it->second.pos - ss1).length() / denom;

			//If the start of edge is close to the swept surface remove all incident edges from Ec
			if (d0 < d1 && t < roi) {
				CutNode cn;
				cn.idxNode = cutedge.from;
				cn.pos = ss0;
				m_mapCutNodes.insert(std::pair<U32, CutNode>(cn.idxNode, cn));

				//iterate over all incident edges
				vector<U32> incidentEdges;
				this->getNodeIncidentEdges(cn.idxNode, incidentEdges);

				for (U32 i = 0; i < incidentEdges.size(); i++) {
					mapTempCutEdges.erase(incidentEdges[i]);
					ctRemovedCutEdges++;
				}
			}
			//If the end of edge close to the swept surface remove all incident edges from Ec
			else if (d0 > d1 && t < roi) {
				CutNode cn;
				cn.idxNode = cutedge.to;
				cn.pos = ss1;
				m_mapCutNodes.insert(std::pair<U32, CutNode>(cn.idxNode, cn));

				//iterate over all incident edges
				vector<U32> incidentEdges;
				this->getNodeIncidentEdges(cn.idxNode, incidentEdges);

				for (U32 i = 0; i < incidentEdges.size(); i++) {
					mapTempCutEdges.erase(incidentEdges[i]);
					ctRemovedCutEdges++;
				}
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

	//Find the list of all tets impacted
	vector<U32> vCutElements;
	vector<U8> vCutEdgeCodes;
	vector<U8> vCutNodeCodes;
	vCutElements.reserve(128);
	vCutEdgeCodes.reserve(128);
	vCutNodeCodes.reserve(128);

	for(U32 i=0; i < this->countCells(); i++) {
		const CELL& cell = this->const_cellAt(i);
		U8 cutEdgeCode = 0;
		U8 cutNodeCode = 0;

		//compute cutedge code
		for(int e=0; e < COUNT_CELL_EDGES; e++) {
			U32 edge = cell.edges[e];
			if(m_mapCutEdges.find(edge) != m_mapCutEdges.end()) {

				//check the edge
				if(!isEdgeOfCell(edge, i)) {
					LogErrorArg2("Edge %u does not belong to cell %u", edge, i);
					return -2;
				}

				cutEdgeCode |= (1 << e);
			}
		}

		//compute cut node code
		for(int e=0; e < COUNT_CELL_NODES; e++) {
			U32 node = cell.nodes[e];
			if(m_mapCutNodes.find(node) != m_mapCutNodes.end()) {
				cutNodeCode |= (1 << e);
			}
		}

		//if there is a cut in this cell
		if(cutEdgeCode != 0 || cutNodeCode != 0) {

			//push back all computed values
			vCutElements.push_back(i);
			vCutEdgeCodes.push_back(cutEdgeCode);
			vCutNodeCodes.push_back(cutNodeCode);

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
	//Return if we won't modify the mesh this time
//	if(!modifyMesh)
//		return -1;

	//Now that cutedgecodes and cutnodecodes are computed then subdivide the element
	LogInfoArg1("BEGIN CUTTING# %u", m_ctCompletedCuts+1);

	//cut all affected edges
	for(CUTEDGEITER it = m_mapCutEdges.begin(); it != m_mapCutEdges.end(); it++) {
		U32 idxNP0, idxNP1;

		if(!this->cut_edge(it->first, it->second.t, &idxNP0, &idxNP1)) {
			LogErrorArg2("Unable to cut edge %d, edgecutpoint t = %.3f.", it->first, it->second.t);
			return -1;
		}

		it->second.idxNP0 = idxNP0;
		it->second.idxNP1 = idxNP1;
	}

	U32 ctSubdividedTets = 0;
	U32 middlePoints[12];
	for(U32 i=0; i < vCutElements.size(); i++) {

		U8 cutEdgeCode = vCutEdgeCodes[i];
		U8 cutNodeCode = vCutNodeCodes[i];
		if(cutEdgeCode != 0 || cutNodeCode != 0) {

			const CELL& cell = this->const_cellAt(vCutElements[i]);


			for(int e=0; e < COUNT_CELL_EDGES; e++) {

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
			ctSubdividedTets += m_lpSubD->subdivide(this, vCutElements[i], cutEdgeCode, cutNodeCode, middlePoints);
		}
	}


	//increment completed cuts
	if(ctSubdividedTets > 0) {
		LogInfoArg2("END CUTTING# %u: subdivided elements count: %u.", m_ctCompletedCuts + 1, ctSubdividedTets);
		m_ctCompletedCuts ++;

		//store sweep surf
		vector<double> vFlatSurf;
		FlattenVec3<double>(sweptSurface, vFlatSurf);
		m_vSweepSurfaces.insert(m_vSweepSurfaces.end(), vFlatSurf.begin(), vFlatSurf.end());
	}
	else {
		LogWarningArg1("END CUTTING# %u: No elements are subdivided.", m_ctCompletedCuts + 1);
	}


	//clear cut context
	//clearCutContext();

	//collect all garbage
	this->garbage_collection();

	//Perform all tests
	TestVolMesh::tst_all(this);

	//split mesh parts
	if(m_flagSplitMeshAfterCut && (ctSubdividedTets > 0)) {
		splitParts(sweptSurface, 0.2);
	}

	//print mesh parts
	this->printParts();

	//recompute AABB and expand it to detect cuts
	m_aabb = this->computeAABB();
	m_aabb.expand(1.0);

	//Return number of tets cut
	return ctSubdividedTets;
}

vec3d CuttableMesh::vertexRestPosAt(U32 i) const {
	return this->const_nodeAt(i).restpos;
}

int CuttableMesh::findClosestVertex(const vec3d& query, double& dist, vec3d& outP) const {
	double minDist = GetMaxLimit<double>();
	int idxFound = -1;
	for(U32 i=0; i < this->countNodes(); i++) {
		vec3d p = this->const_nodeAt(i).pos;
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

bool CuttableMesh::splitParts(const vector<vec3d>& vSweeptSurf, double dist) {
	vec3d sweptSurfNormal = vec3d::cross(vSweeptSurf[1] - vSweeptSurf[0], vSweeptSurf[3] - vSweeptSurf[0]);
	sweptSurfNormal.normalize();

	vec3d dfront = sweptSurfNormal * dist;

	//compute centroid
	vec3d sweptSurfCentroid = vSweeptSurf[0];
	for(int i = 1; i < 4; i++)
		sweptSurfCentroid = sweptSurfCentroid + vSweeptSurf[i];
	sweptSurfCentroid = sweptSurfCentroid * 0.25;


	vector< vector<U32> > cellgroups;
	get_disjoint_parts(cellgroups);


	//set of nodes
	set<U32> setFrontNodes;
	set<U32> setBackNodes;

	for(U32 i = 0; i < cellgroups.size(); i++) {

		vector<U32> cells = cellgroups[i];

		U32 ctFront = 0;
		for(vector<U32>::const_iterator it = cells.begin(); it !=cells.end(); ++it) {
			if(isCellIndex(*it)) {
				vec3d x = computeCellCentroid(*it);

				x = x - sweptSurfCentroid;
				if(vec3d::dot(x, sweptSurfNormal) > 0)
					ctFront++;
			}
		}

		//categorize nodes based on front and back count
		if(ctFront == cells.size()) {
			for(vector<U32>::const_iterator it = cells.begin(); it !=cells.end(); it++) {
				const CELL& cell = const_cellAt(*it);
				for(int j=0; j < COUNT_CELL_NODES; j++)
					setFrontNodes.insert(cell.nodes[j]);
			}
		}
		else if(ctFront == 0) {
			//add to back
			for(vector<U32>::const_iterator it = cells.begin(); it !=cells.end(); it++) {
				const CELL& cell = const_cellAt(*it);
				for(int j=0; j < COUNT_CELL_NODES; j++)
					setBackNodes.insert(cell.nodes[j]);
			}
		}
	}

	vector<U32> frontNodes(setFrontNodes.begin(), setFrontNodes.end());
	vector<U32> backNodes(setBackNodes.begin(), setBackNodes.end());

	for(vector<U32>::const_iterator it = frontNodes.begin(); it != frontNodes.end(); it++) {
		NODE& node = nodeAt(*it);
		node.pos = node.pos + dfront;
	}

	for(vector<U32>::const_iterator it = backNodes.begin(); it != backNodes.end(); it++) {
		NODE& node = nodeAt(*it);
		node.pos = node.pos - dfront;
	}

	return true;
}

}


