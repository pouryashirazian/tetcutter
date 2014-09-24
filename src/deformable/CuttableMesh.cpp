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
#include "deformable/VolMeshStats.h"
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
	m_quadstrips.resize(0);
}

void CuttableMesh::setup() {

	resetTransform();
	if(TheShaderManager::Instance().has("phong")) {
        m_spEffect = SmartPtrSGEffect(new SGEffect(TheShaderManager::Instance().get("phong")));
    }

	//Perform all tests
	TestVolMesh::tst_all(this);

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
	m_mapCutEdges.clear();
	m_mapCutNodes.clear();
}

void CuttableMesh::draw() {

	drawBBox();

	//draw volmesh
	if(m_spEffect)
		m_spEffect->bind();

	VolMesh::draw();

	if(m_spEffect)
		m_spEffect->unbind();

	//draw cut context
	int ctCutEdges = m_mapCutEdges.size();
	int ctCutNodes = m_mapCutNodes.size();
	if(ctCutNodes > 0 || ctCutEdges > 0) {
		glDisable(GL_LIGHTING);
		glPushAttrib(GL_ALL_ATTRIB_BITS);
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
		glDisable(GL_CULL_FACE);
		glDisable(GL_LIGHTING);
		glPushAttrib(GL_ALL_ATTRIB_BITS);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_BLEND);

		glColor4f(1.0, 0.0, 0.0, 0.3);
		glBegin(GL_QUAD_STRIP);
		for(U32 i=0; i < m_quadstrips.size(); i++) {
			glVertex3dv(m_quadstrips[i].cptr());
		}
		glEnd();

		glDisable(GL_BLEND);
		glPopAttrib();
		glEnable(GL_LIGHTING);
		glEnable(GL_CULL_FACE);
	}
}


int CuttableMesh::computeCutEdgesKernel(const vec3d sweptquad[4],
						  	  	  	  	std::map<U32, CutEdge>& mapCutEdges) {

	//if the swept surface is degenerate then return
	double area = (sweptquad[1] - sweptquad[0]).length2() * (sweptquad[2] - sweptquad[0]).length2();
	if(area < EPSILON)
		return -1;
	double l2 = (sweptquad[3] - sweptquad[2]).length2();
	if(l2 < EPSILON)
		return -1;


	//vars
	vec3d uvw, xyz, ss0, ss1;
	double t;

	vec3d tri1[3] = {sweptquad[0], sweptquad[2], sweptquad[1]};
	vec3d tri2[3] = {sweptquad[2], sweptquad[3], sweptquad[1]};


	//Cut-Edges
	U32 ctEdges = countEdges();
	int found = 0;
	for (U32 i=0; i < ctEdges; i++) {

		const EDGE& e = this->const_edgeAt(i);

		ss0 = this->const_nodeAt(e.from).pos;
		ss1 = this->const_nodeAt(e.to).pos;

		int res = IntersectSegmentTriangle(ss0, ss1, tri1, t, uvw, xyz);
		if(res == 0)
			res = IntersectSegmentTriangle(ss0, ss1, tri2, t, uvw, xyz);
		if(res > 0) {
			CutEdge ce;
			ce.idxOrgFrom = e.from;
			ce.idxOrgTo = e.to;
			ce.pos = xyz;
			ce.uvw = uvw;
			ce.t = t;

			//test
			vec3d temp = ss0 + (ss1 - ss0).normalized() * t;
			assert( (xyz - temp).length() < EPSILON);

			//add to cut edges map
			if(mapCutEdges.find(i) == mapCutEdges.end()) {
				mapCutEdges.insert( std::make_pair(i, ce));
				found++;
			}
			else {
				LogErrorArg1("Edge %d has already been cut!", i);
			}
		}
	}

	return found;
}

int CuttableMesh::computeCutNodesKernel(const vec3d& blade0,
									   const vec3d& blade1,
									   const vec3d sweptquad[4],
									   std::map<U32, CutEdge>& mapCutEdges,
									   std::map<U32, CutNode>& mapCutNodes) {
	//Radios of Influence in percent
	const double roi = 0.2;

	//blade
	const double edgelen2 = (blade1 - blade0).length2();
	int ctRemovedCutEdges = 0;
	vec3d ss0, ss1;

	//detect all cut-nodes and remove the cut-edges that emanate from a cut-node
	for (CUTEDGEITER it = mapCutEdges.begin(); it != mapCutEdges.end(); ++it) {

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
			mapCutNodes.insert(std::pair<U32, CutNode>(cn.idxNode, cn));

			//iterate over all incident edges
			vector<U32> incidentEdges;
			this->getNodeIncidentEdges(cn.idxNode, incidentEdges);

			for (U32 i = 0; i < incidentEdges.size(); i++) {
				mapCutEdges.erase(incidentEdges[i]);
				ctRemovedCutEdges++;
			}
		}
		//If the end of edge close to the swept surface remove all incident edges from Ec
		else if (d0 > d1 && t < roi) {
			CutNode cn;
			cn.idxNode = cutedge.to;
			cn.pos = ss1;
			mapCutNodes.insert(std::pair<U32, CutNode>(cn.idxNode, cn));

			//iterate over all incident edges
			vector<U32> incidentEdges;
			this->getNodeIncidentEdges(cn.idxNode, incidentEdges);

			for (U32 i = 0; i < incidentEdges.size(); i++) {
				mapCutEdges.erase(incidentEdges[i]);
				ctRemovedCutEdges++;
			}
		}
	}

	return ctRemovedCutEdges;
}


int CuttableMesh::cut(const vector<vec3d>& segments,
			 	 	  const vector<vec3d>& quadstrips,
					  bool modifyMesh) {

	if(segments.size() < 2)
		return CUT_ERR_INVALID_INPUT_ARG;
	if(quadstrips.size() < 4 || (quadstrips.size() % 2 != 0))
		return CUT_ERR_INVALID_INPUT_ARG;

	//1.Compute all cut-edges
	//2.Compute cut nodes and remove all incident edges to cut nodes from cut edges
	//3.split cut edges and compute the reference position of the split point
	//4.duplicate cut nodes and incident edges
	m_mapCutEdges.clear();
	m_mapCutNodes.clear();

	std::map< U32, CutEdge > mapTempCutEdges;
	std::map< U32, CutNode > mapTempCutNodes;

	U32 ctSegments = segments.size() - 1;
	U32 ctQuads = (quadstrips.size() - 2) / 2;
	assert(ctSegments == ctQuads);

	int ctRemovedCutEdges = 0;

	//scalpel segments
	vector<int> vPerSegmentCuts;
	vPerSegmentCuts.resize(ctSegments);
	for(U32 i = 0; i < ctSegments; i++) {

		vec3d s0 = segments[i];
		vec3d s1 = segments[i + 1];
		vPerSegmentCuts[i] = computeCutEdgesKernel(&quadstrips[i * 2], mapTempCutEdges);

		if (m_flagDetectCutNodes) {
			ctRemovedCutEdges += computeCutNodesKernel(s0, s1, &quadstrips[i * 2], mapTempCutEdges, mapTempCutNodes);
		}
	}

	//nothing has been cut!?
	if(mapTempCutEdges.size() == 0 && mapTempCutNodes.size() == 0)
		return 0;

	//Copy
	m_mapCutEdges.insert(mapTempCutEdges.begin(), mapTempCutEdges.end());
	m_mapCutNodes.insert(mapTempCutNodes.begin(), mapTempCutNodes.end());
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
				return CUT_ERR_UNHANDLED_CUT_STATE;
			}
		}
	}

	//	int edgeMaskPos[6][2] = { {1, 2}, {2, 3}, {3, 1}, {2, 0}, {0, 3}, {0, 1} };
	//	int edgeMaskNeg[6][2] = { {3, 2}, {2, 1}, {1, 3}, {3, 0}, {0, 2}, {1, 0} };
	//	int faceMaskPos[4][3] = { {1, 2, 3}, {2, 0, 3}, {3, 0, 1}, {1, 0, 2} };
	//	int faceMaskNeg[4][3] = { {3, 2, 1}, {3, 0, 2}, {1, 0, 3}, {2, 0, 1} };

	//Return if we won't modify the mesh this time
	if(!modifyMesh)
		return -1;

	//Now that cutedgecodes and cutnodecodes are computed then subdivide the element
	LogInfoArg1("BEGIN CUTTING# %u", m_ctCompletedCuts+1);

	//cut all affected edges
	for(CUTEDGEITER it = m_mapCutEdges.begin(); it != m_mapCutEdges.end(); it++) {
		U32 idxNP0, idxNP1;

		if(!this->cut_edge(it->first, it->second.t, &idxNP0, &idxNP1)) {
			LogErrorArg2("Unable to cut edge %d, edgecutpoint t = %.3f.", it->first, it->second.t);
			return CUT_ERR_UNABLE_TO_CUT_EDGE;
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

			//select the middle points for cut edges
			for(int e=0; e < COUNT_CELL_EDGES; e++) {
				CUTEDGEITER it = m_mapCutEdges.find(cell.edges[e]);

				//mid points
				middlePoints[e * 2 + 0] = VolMesh::INVALID_INDEX;
				middlePoints[e * 2 + 1] = VolMesh::INVALID_INDEX;

				//if edge is in the list of cutedges
				if(it != m_mapCutEdges.end()) {
					const EDGE& edge = const_edgeAt(cell.edges[e]);

					U32 idxOrgFrom = it->second.idxOrgFrom;
					U32 idxOrgTo = it->second.idxOrgTo;
					U32 idxNP0 = it->second.idxNP0;
					U32 idxNP1 = it->second.idxNP1;

					bool res1 = edge_exists(idxOrgFrom, idxNP0);
					bool res2 = edge_exists(idxNP1, idxOrgTo);

					if(!res1 || !res2)
						cerr << "invalid edge" << endl;
					assert(res1 && res2);

					middlePoints[e * 2 + 0] = idxNP0;
					middlePoints[e * 2 + 1] = idxNP1;
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
		m_quadstrips.clear();
		m_quadstrips.insert(m_quadstrips.end(), quadstrips.begin(), quadstrips.end());
	}
	else {
		LogWarningArg1("END CUTTING# %u: No elements are subdivided.", m_ctCompletedCuts + 1);
	}


	//clear cut context
	//clearCutContext();

	//collect all garbage
	garbage_collection();

	//Perform all tests
	TestVolMesh::tst_all(this);

	//split mesh parts
	if(m_flagSplitMeshAfterCut && (ctSubdividedTets > 0)) {

		for(U32 i = 0; i < ctSegments; i++) {
			if(vPerSegmentCuts[i] > 0)
				splitParts(&quadstrips[i * 2], 0.2);
		}
	}

	//print mesh parts
	//printParts();
	VolMeshStats::printAllStats(this);

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

bool CuttableMesh::splitParts(const vec3d sweptquad[4], double dist) {

	vec3d sweptSurfNormal = vec3d::cross(sweptquad[1] - sweptquad[0], sweptquad[2] - sweptquad[0]);
	sweptSurfNormal.normalize();

	vec3d dfront = sweptSurfNormal * dist;

	//compute centroid
	vec3d sweptSurfCentroid = sweptquad[0];
	for(int i = 1; i < 4; i++)
		sweptSurfCentroid = sweptSurfCentroid + sweptquad[i];
	sweptSurfCentroid = sweptSurfCentroid * 0.25;


	//find disjoint mesh-parts
	vector< vector<U32> > cellgroups;
	get_disjoint_parts(cellgroups);


	//set of nodes
	set<U32> setFrontNodes;
	set<U32> setBackNodes;

	//partition nodes to front and back of the sweep surf
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

	//move nodes to front
	for(vector<U32>::const_iterator it = frontNodes.begin(); it != frontNodes.end(); it++) {
		NODE& node = nodeAt(*it);
		node.pos = node.pos + dfront;
	}

	//move nodes to back
	for(vector<U32>::const_iterator it = backNodes.begin(); it != backNodes.end(); it++) {
		NODE& node = nodeAt(*it);
		node.pos = node.pos - dfront;
	}

	return true;
}

int CuttableMesh::convertDisjointPartsToMeshes(vector<CuttableMesh*>& vOutNewMeshes) {

	vOutNewMeshes.clear();
	vector< vector<U32> > parts;
	int count = get_disjoint_parts(parts);
	if(count < 2)
		return vOutNewMeshes.size();

	//
	vOutNewMeshes.reserve(parts.size());

	//
	vector<U32> vAllSplittedCells;


	//loop over parts
	for(U32 i=0; i < parts.size(); i++) {
		vector<U32> curpart = parts[i];

		if(curpart.size() == 0) {
			LogErrorArg1("splitted part %d is empty.", i);
			continue;
		}


		//maps old nodes to new nodes
		map<U32, U32> mapNodes;
		vector<vec3d> vNewNodes;
		vector<U32> vNewCells;

		for(vector<U32>::const_iterator c_it = curpart.begin(); c_it != curpart.end(); ++c_it) {
			vAllSplittedCells.push_back(*c_it);
			const CELL& cell = const_cellAt(*c_it);

			//add all new vertices
			for(U32 k=0; k < COUNT_CELL_NODES; k++) {

				//if the new nodes are not cached
				if(mapNodes.find(cell.nodes[k]) == mapNodes.end()) {
					vec3d v = const_nodeAt(cell.nodes[k]).pos;
					vNewNodes.push_back(v);
					U32 idxNew = vNewNodes.size() - 1;
					mapNodes.insert(std::make_pair(cell.nodes[k], idxNew));
				}

				//add all 4 element indices
				vNewCells.push_back(mapNodes[cell.nodes[k]]);
			}
		}

		//create the mesh
		vector<double> vFlatNodes;
		FlattenVec3<double>(vNewNodes, vFlatNodes);
		CuttableMesh* amesh = new CuttableMesh(vFlatNodes, vNewCells);

		AnsiStr strName = printToAStr("%s_cut%d_part%d", this->name().c_str(), countCompletedCuts(), i);
		amesh->setName(string(strName.cptr()));
		vOutNewMeshes.push_back(amesh);

		//clean for this part
		mapNodes.clear();
	}


	//schedule all cells for deletion
	for(vector<U32>::const_iterator c_it = vAllSplittedCells.begin(); c_it != vAllSplittedCells.end(); ++c_it)
		this->schedule_remove_cell(*c_it);
	this->garbage_collection();

	return vOutNewMeshes.size();
}


}


