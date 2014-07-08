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

namespace PS {
namespace FEM {

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

		{ {4, 10, 12, 0}, {0, 8, 12, 4}, {0, 8, 1, 4}, {3, 9, 13, 5}, {2, 5, 11, 3}, {3, 13, 5, 11}}

};



TetSubdivider::TetSubdivider(HalfEdgeTetMesh* pMesh) {
	m_lpHEMesh = pMesh;


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

int TetSubdivider::generateCaseA(U32 element, U8 node, double targetDistPercentage,
									   U8& cutEdgeCode, U8& cutNodeCode, double (&tEdges)[6]) {

	if(node >= 4)
		return -1;
	if(targetDistPercentage < 0.0 || targetDistPercentage > 1.0)
		return -1;

	if(!m_lpHEMesh->isElemIndex(element))
		return -1;

	cutEdgeCode = 0;
	cutNodeCode = 0;

	int res = 0;
	HalfEdgeTetMesh::ELEM elem = m_lpHEMesh->elemAt(element);
	for(int i=0; i<6; i++) {
		tEdges[i] = 0.0;

		HalfEdgeTetMesh::EDGE edge = m_lpHEMesh->edgeAt( m_lpHEMesh->edge_from_halfedge(elem.halfedge[i]) );

		if(edge.from == node || edge.to == node) {
			cutEdgeCode |= (1 << i);

			double dist = vec3d::distance(m_lpHEMesh->nodeAt(edge.from).pos, m_lpHEMesh->nodeAt(edge.to).pos);

			if(edge.from == node)
				tEdges[i] = targetDistPercentage * dist;
			else
				tEdges[i] = (1.0 - targetDistPercentage) * dist;

			res ++;
		}
	}

	return res;

}

int TetSubdivider::generateCaseB(U32 element, U8 enteringface, U8& cutEdgeCode,
					  U8& cutNodeCode, double (&tEdges)[6]) {

	if(!m_lpHEMesh->isElemIndex(element))
		return -1;
	if(enteringface < 0 || enteringface > 2)
		return -1;

	cutEdgeCode = cutNodeCode = 0;
	for(int i=0; i<6; i++)
		tEdges[i] = 0.0;

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


	for(int i=0; i<4; i++) {
		cutEdgeCode |= (1 << edges[i]);

		tEdges[ edges[i] ] = 0.8;
	}

	return 1;
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

int TetSubdivider::subdivide(U32 element, U8 cutEdgeCode, U8 cutNodeCode, U32 middlePoints[12], bool dosplit) {
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
	printf("Element: %u, Cut type %c: cutEdgeCode: %u, cutNodeCode: %u\n", element, m_mapCutCaseToAlpha[cutcase], cutEdgeCode, cutNodeCode);


	//fill the array of virtual nodes
	U32 vnodes[16];
	for(int i=0; i<16; i++)
		vnodes[i] = HalfEdgeTetMesh::INVALID_INDEX;
	const HalfEdgeTetMesh::ELEM& tet = m_lpHEMesh->elemAt(element);

	//1st 4 nodes come from the original element
	for(int i=0; i<4; i++)
		vnodes[i] = tet.nodes[i];

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
		m_lpHEMesh->remove_element(element);

		//find the local table entry to handle this case A
		int entry = m_mapCutEdgeCodeToTableEntry[cutEdgeCode];


		//generate 4 new tets
		for(int e = 0; e < 4; e++) {
			U32 n[4];

			for(int i = 0; i < 4; i++)
				n[i] = vnodes[ g_elementTableCaseA[entry][e * 4 + i] ];


			if(dosplit && e == 0) {
				vec3d pos[4];

				for(int k=0; k < 4; k++)
					pos[k] = m_lpHEMesh->nodeAt(n[k]).pos;

				vec3d centroid = pos[0];
				for(int k=1; k < 4; k++)
					centroid = centroid + pos[k];
				centroid = centroid * 0.25;

				vec3d norm = (pos[0] - centroid).normalized();
				double dist = (pos[0] - centroid).length() * 0.3;

				for(int k = 0; k<4; k++)
					m_lpHEMesh->nodeAt( n[ k ]).pos = pos[k] + norm * dist;

			}

			m_lpHEMesh->insert_element(n);
		}
	}
	//Case B: 4 cut edges. cutEdgeCodes = { 46, 51, 29 }
	else if(cutcase == cutB) {
		//Remove the original element
		m_lpHEMesh->remove_element(element);

		//find the local table entry to handle this case B
		int entry = m_mapCutEdgeCodeToTableEntry[cutEdgeCode];

		//generate 6 new tets
		for(int e = 0; e < 6; e++) {

			U32 n[4];
			for(int i = 0; i < 4; i++)
				n[i] = vnodes[ g_elementTableCaseB[entry][e][i] ];

			m_lpHEMesh->insert_element(n);
		}

		if(dosplit) {
			set<U32> splittedNodes1;
			set<U32> splittedNodes2;
			vec3d cent1, cent2;

			//insert nodes for the first 3 tets
			for(int e=0; e<3; e++) {
				for(int i = 0; i < 4; i++) {
					splittedNodes1.insert( vnodes[ g_elementTableCaseB[entry][e][i] ]);
				}
			}

			//insert nodes for the last 3 tets
			for(int e=3; e<6; e++) {
				for(int i = 0; i < 4; i++) {
					splittedNodes2.insert( vnodes[ g_elementTableCaseB[entry][e][i] ]);
				}
			}

			if(splittedNodes1.size() != 6 || splittedNodes2.size() != 6) {
				LogError("Splitted nodes are not sets of 6!");
				return 0;
			}

			vec3d arrP1[6];
			int i = 0;
			for(set<U32>::const_iterator it = splittedNodes1.begin(); it != splittedNodes1.end(); it++) {
				arrP1[i] = m_lpHEMesh->nodeAt( *it ).pos;
				if(i == 0)
					cent1 = arrP1[i];
				else
					cent1 = cent1 + arrP1[i];
				i++;
			}
			cent1 = cent1 * (1.0 / 6.0);

			//second set
			vec3d arrP2[6];
			i = 0;
			for(set<U32>::const_iterator it = splittedNodes2.begin(); it != splittedNodes2.end(); it++) {
				arrP2[i] = m_lpHEMesh->nodeAt( *it ).pos;
				if(i == 0)
					cent2 = arrP2[i];
				else
					cent2 = cent2 + arrP2[i];
				i++;
			}
			cent2 = cent2 * (1.0 / 6.0);

			//compute norm and dist
			vec3d norm = (cent1 - cent2).normalized();
			double dist = (cent1 - cent2).length() * 0.3;

			//splitted nodes
			i = 0;
			for(set<U32>::const_iterator it = splittedNodes1.begin(); it != splittedNodes1.end(); it++) {
				m_lpHEMesh->nodeAt( *it ).pos = arrP1[i] + norm * dist;
				i++;
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

	if(m_lpHEMesh)
		m_lpHEMesh->draw();
}

}
}

