/*
 * TetSubdivider.cpp
 *
 *  Created on: May 31, 2014
 *      Author: pourya
 */
#include "TetSubdivider.h"
#include "base/Logger.h"
#include <set>

using namespace PS;
using namespace std;

using namespace PS;
using namespace PS::MESH;

namespace PS {
namespace MESH {

//elements to be generated for case A
U32 g_elementTableCaseA[4][16] = {
		//Node 0: cutEdgeCode = 56, edges cut = 3, 4, 5
		{0, 12, 10, 14, 3, 13, 11, 15, 3, 1, 15, 11, 1, 2, 3, 11},

		//Node 1: cutEdgeCode = 37, edges cut = 0, 2, 5
		{1, 4, 8, 15, 3, 9, 5, 14, 0, 3, 14, 5, 0, 2, 3, 5},

		//Node 2: cutEdgeCode = 11, edges cut = 0, 1, 3
		{2, 5, 6, 11, 3, 7, 10, 4, 3, 1, 4, 10, 0, 1, 3, 10},

		//Node 3: cutEdgeCode = 22, edges cut = 1, 2, 4
		{3, 13, 9, 7, 1, 8, 12, 6, 1, 2, 12, 6, 0, 1, 2, 12},
};

//elements to be generated for case B
U32 g_elementTableCaseB[3][6][4] = {
		//case 0,
		{ {2, 6, 11, 1}, {8, 11, 15, 1}, {6, 11, 8, 1}, {3, 7, 9, 10}, {3, 10, 0, 9}, {0, 14, 10, 9}},

		//case 1,
		{ {3, 7, 13, 15}, {15, 1, 4, 7}, {15, 1, 3, 7}, {0, 12, 14, 6}, {2, 5, 6, 14}, {0, 14, 6, 2}},

		//case 2
		{ {4, 10, 12, 0}, {0, 8, 12, 4}, {0, 8, 1, 4}, {3, 9, 13, 5}, {2, 5, 11, 3}, {3, 13, 5, 11}}
};

}
}

TetSubdivider::TetSubdivider() {

	//case A
	m_mapCutEdgeCodeToTableEntry[56] = 0;
	m_mapCutEdgeCodeToTableEntry[37] = 1;
	m_mapCutEdgeCodeToTableEntry[11] = 2;
	m_mapCutEdgeCodeToTableEntry[22] = 3;

	//case B
	m_mapCutEdgeCodeToTableEntry[46] = 0;
	m_mapCutEdgeCodeToTableEntry[51] = 1;
	m_mapCutEdgeCodeToTableEntry[29] = 2;

	//map cutcase to alphabet
	m_mapCutCaseToAlpha[cutA] = 'A';
	m_mapCutCaseToAlpha[cutB] = 'B';
	m_mapCutCaseToAlpha[cutC] = 'C';
	m_mapCutCaseToAlpha[cutD] = 'D';
	m_mapCutCaseToAlpha[cutE] = 'E';
	m_mapCutCaseToAlpha[cutX] = 'X';
	m_mapCutCaseToAlpha[cutY] = 'Y';
	m_mapCutCaseToAlpha[cutZ] = 'Z';
	m_mapCutCaseToAlpha[cutUnknown] = 'U';

}

TetSubdivider::~TetSubdivider() {
}

int TetSubdivider::generateCaseA(VolMesh* pmesh, U32 idxCell, U8 node, double targetDistPercentage,
							     U8& cutEdgeCode, U8& cutNodeCode) {

	if(node >= 4)
		return -1;
	if(targetDistPercentage < 0.0 || targetDistPercentage > 1.0)
		return -1;

	if(!pmesh->isCellIndex(idxCell))
		return -1;

	cutEdgeCode = 0;
	cutNodeCode = 0;

	//
	double tEdges[6];
	vector<vec3d> sweptSurf;
	sweptSurf.resize(4);
	U32 middlePoints[12];
	for(int i = 0; i < 12; i++)
		middlePoints[i] = VolMesh::INVALID_INDEX;


	int res = 0;
	const CELL& cell = pmesh->const_cellAt(idxCell);

	for(int i=0; i<6; i++) {
		tEdges[i] = 0.0;

		const EDGE& edge = pmesh->edgeAt( cell.edges[i] );

		if(edge.from == node || edge.to == node) {
			cutEdgeCode |= (1 << i);

			double dist = vec3d::distance(pmesh->const_nodeAt(edge.from).pos, pmesh->const_nodeAt(edge.to).pos);

			if(edge.from == node)
				tEdges[i] = targetDistPercentage * dist;
			else
				tEdges[i] = (1.0 - targetDistPercentage) * dist;

			//generated middle points
			U32 idxNP0 = 0;
			U32 idxNP1 = 0;
			pmesh->cut_edge(cell.edges[i], tEdges[i], &idxNP0, &idxNP1);
			middlePoints[ i * 2 ] = idxNP0;
			middlePoints[ i * 2 + 1 ] = idxNP1;

			//swept surf
			sweptSurf[res] = pmesh->const_nodeAt(idxNP0).pos;
			res++;
		}
	}

	//init sweptSurf
	if(res == 3)
		sweptSurf[3] = sweptSurf[2];

	//subdivide
	return subdivide(pmesh, idxCell, cutEdgeCode, cutNodeCode, middlePoints);
}

int TetSubdivider::generateCaseB(VolMesh* pmesh, U32 idxCell, U8 enteringface,
								 U8& cutEdgeCode, U8& cutNodeCode) {

	if(!pmesh->isCellIndex(idxCell))
		return -1;
	if(enteringface < 0 || enteringface > 2)
		return -1;

	cutEdgeCode = cutNodeCode = 0;
	U8 edges[4];
	//face: entering edges --> exiting edges
	//0: 1, 2 --> 3, 5
	//1: 1, 4 --> 0, 5
	//2: 2, 4 --> 0, 3
	if(enteringface == 0) {
		edges[0] = 1;
		edges[1] = 2;
		edges[2] = 3;
		edges[3] = 5;
	}
	else if(enteringface == 1) {
		//edges = {1, 4, 0, 5};
		edges[0] = 1;
		edges[1] = 4;
		edges[2] = 0;
		edges[3] = 5;
	}
	else if(enteringface == 2) {
		//edges = {2, 4, 0, 3};
		edges[0] = 2;
		edges[1] = 4;
		edges[2] = 0;
		edges[3] = 3;
	}

	//Mask to map edges indices to new generated node indices
	//const int mapEdgeToMiddleNodes[12] = {4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
	const CELL& cell = pmesh->const_cellAt(idxCell);
	U32 middlePoints[12];
	for(int i = 0; i < 12; i++)
		middlePoints[i] = VolMesh::INVALID_INDEX;

	vector<vec3d> sweptSurf;
	sweptSurf.resize(4);

	for(int i=0; i<4; i++) {
		cutEdgeCode |= (1 << edges[i]);

		U32 idxNP0 = 0;
		U32 idxNP1 = 0;

		pmesh->cut_edge(cell.edges[ edges[i] ], 0.8, &idxNP0, &idxNP1);
		middlePoints[ edges[i] * 2 ] = idxNP0;
		middlePoints[ edges[i] * 2 + 1 ] = idxNP1;

		//swept surf
		sweptSurf[i] = pmesh->const_nodeAt(idxNP0).pos;
	}


	//subdivide
	return subdivide(pmesh, idxCell, cutEdgeCode, cutNodeCode, middlePoints);
}

char TetSubdivider::toAlpha(CUTCASE c) {
	return m_mapCutCaseToAlpha[c];
}

TetSubdivider::CUTCASE TetSubdivider::IdentifyCutCase(bool isCutComplete, U8 cutEdgeCode, U8 cutNodeCode) {
	U8 countCutEdges = 0;
	U8 countCutNodes = 0;
	return IdentifyCutCase(isCutComplete, cutEdgeCode, cutNodeCode, countCutEdges, countCutNodes);
}

TetSubdivider::CUTCASE TetSubdivider::IdentifyCutCase(bool isCutComplete, U8 cutEdgeCode,
													  U8 cutNodeCode, U8& countCutEdges, U8& countCutNodes) {
	countCutEdges = 0;

	//6 edges per tet
	for(int i=0; i < 6; i++)
		countCutEdges += ((cutEdgeCode & (1 << i)) != 0);

	countCutNodes = 0;
	//4 nodes per tet
	for(int i=0; i < 4; i++)
		countCutNodes += ((cutNodeCode & (1 << i)) != 0);

	if(countCutNodes == 0) {
		if(isCutComplete) {
			if(countCutEdges == 3)
				return cutA;
			else if(countCutEdges == 4)
				return cutB;
		}
		else {
			if(countCutEdges == 1)
				return cutC;
			else if(countCutEdges == 2)
				return cutD;
			else if(countCutEdges == 3)
				return cutE;
		}
	}
	else {
		if(countCutEdges == 2 && countCutNodes == 1)
			return cutX;
		else if(countCutEdges == 1 && countCutNodes == 2)
			return cutY;
		else if(countCutEdges == 0 && countCutNodes == 3)
			return cutZ;
	}

	return cutUnknown;
}

int TetSubdivider::subdivide(VolMesh* pmesh, U32 idxCell,
							 U8 cutEdgeCode, U8 cutNodeCode,
							 U32 middlePoints[12]) {
	//Here an element is subdivided to 4 sub elements depending on the codes
	U8 ctCutEdges = 0;
	U8 ctCutNodes = 0;

	TetSubdivider::CUTCASE cutcase = IdentifyCutCase(true, cutEdgeCode, cutNodeCode, ctCutEdges, ctCutNodes);
	if(cutcase > cutB) {
		LogErrorArg3("This case is not handled yet! cutEdgeCode = %u, ctCutEdges = %u, ctCutNodes = %u",
					 cutEdgeCode, ctCutEdges, ctCutNodes);
		return 0;
	}

	//report
	printf("Cell: %u, Cut type %c:%d, cutEdgeCode: %u, cutNodeCode: %u\n",
			idxCell, m_mapCutCaseToAlpha[cutcase],
			m_mapCutEdgeCodeToTableEntry[cutEdgeCode],
			cutEdgeCode, cutNodeCode);


	//fill the array of virtual nodes
	U32 vnodes[16];
	for(int i=0; i<16; i++)
		vnodes[i] = BaseHandle::INVALID;
	const CELL& cell = pmesh->const_cellAt(idxCell);

	//1st 4 nodes come from the original element
	for(int i=0; i<4; i++)
		vnodes[i] = cell.nodes[i];

	//Mask to map edges indices to new generated node indices
	const int mapEdgeToMiddleNodes[12] = {4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};

	if(ctCutEdges > 0) {

		//loop over edges
		for(int i=0; i < 6; i++) {
			bool isCut = ((cutEdgeCode & (1 << i)) != 0);
			if(isCut) {
				//generated nodes
				vnodes[ mapEdgeToMiddleNodes[i*2 + 0] ] = middlePoints[i * 2 + 0];
				vnodes[ mapEdgeToMiddleNodes[i*2 + 1] ] = middlePoints[i * 2 + 1];
			}
		}
	}


	//Case A: 3 cut edges. cutEdgeCodes = { 11, 22, 37, 56 }
	if(cutcase == cutA) {
		//Remove the original element
		pmesh->schedule_remove_cell(idxCell);

		//find the local table entry to handle this case A
		int entry = m_mapCutEdgeCodeToTableEntry[cutEdgeCode];


		//generate 4 new tets
		for(int e = 0; e < 4; e++) {
			U32 n[4];

			for(int i = 0; i < 4; i++)
				n[i] = vnodes[ g_elementTableCaseA[entry][e * 4 + i] ];

			if(!pmesh->insert_cell(n)) {
				LogErrorArg1("Failed to add element# %d", e);
			}
		}
	}
	//Case B: 4 cut edges. cutEdgeCodes = { 46, 51, 29 }
	else if(cutcase == cutB) {
		//Remove the original element
		pmesh->schedule_remove_cell(idxCell);

		//find the local table entry to handle this case B
		int entry = m_mapCutEdgeCodeToTableEntry[cutEdgeCode];

		//generate 6 new tets
		for(int e = 0; e < 6; e++) {

			U32 n[4];
			for(int i = 0; i < 4; i++)
				n[i] = vnodes[ g_elementTableCaseB[entry][e][i] ];

			if(!pmesh->insert_cell(n)) {
				LogErrorArg1("Failed to add element# %d", e);
			}
		}
	}

	//Case X: 2 cut edges and 1 cut node
//	else if(ctCutEdges == 2 && ctCutNodes == 1) {
//
//	}
//	case Y
//	else if(ctCutEdges == 1 && ctCutNodes == 2) {
//
//	}
//	case Z
//	else if(ctCutEdges == 0 && ctCutNodes == 3) {
//
//	}


	return 1;
}


void TetSubdivider::draw() {

}

