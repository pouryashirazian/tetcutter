/*
 * TetSubdivider.cpp
 *
 *  Created on: May 31, 2014
 *      Author: pourya
 */
#include "TetSubdivider.h"
#include "base/Logger.h"

using namespace PS;

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



TetSubdivider::TetSubdivider(HalfEdgeTetMesh* pMesh) {
	m_lpHEMesh = pMesh;

	m_mapCutEdgeCodeToTableEntry[56] = 0;
	m_mapCutEdgeCodeToTableEntry[37] = 1;
	m_mapCutEdgeCodeToTableEntry[11] = 2;
	m_mapCutEdgeCodeToTableEntry[22] = 3;
}

TetSubdivider::~TetSubdivider() {
}

int TetSubdivider::generateCaseA(U32 element, U8 node, double targetDist,
									   U8& cutEdgeCode, U8& cutNodeCode, double (&tEdges)[6]) {

	if(node >= 4)
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

			if(edge.from == node)
				tEdges[i] = targetDist;
			else
				tEdges[i] = 1.0 - targetDist;

			res ++;
		}
	}

	return res;

}

int TetSubdivider::subdivide(U32 element, U8 cutEdgeCode, U8 cutNodeCode, double tEdges[6]) {
	//Here an element is subdivided to 4 sub elements depending on the codes
	U8 ctCutEdges = 0;

	//6 edges per tet
	for(int i=0; i < 6; i++)
		ctCutEdges += ((cutEdgeCode & (1 << i)) != 0);

	U8 ctCutNodes = 0;

	//4 nodes per tet
	for(int i=0; i < 4; i++)
		ctCutNodes += ((cutNodeCode & (1 << i)) != 0);

	printf("Number of cutEdges: %d, cutNodes: %d \n", ctCutEdges, ctCutNodes);


	//fill the array of virtual nodes
	U32 vnodes[16];
	for(int i=0; i<16; i++)
		vnodes[i] = HalfEdgeTetMesh::INVALID_INDEX;
	const HalfEdgeTetMesh::ELEM& one = m_lpHEMesh->elemAt(element);

	//1st 4 nodes come from the original element
	for(int i=0; i<4; i++)
		vnodes[i] = one.nodes[i];

	//Mask to map edges indices to new generated node indices
	const int mapEdgeToMiddleNodes[12] = {4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};

	if(ctCutEdges > 0) {

		//loop over edges
		for(int i=0; i < 6; i++) {
			bool isCut = ((cutEdgeCode & (1 << i)) != 0);

			if(isCut) {

				U32 idxNP0, idxNP1;
				U32 idxEdgeToCut = m_lpHEMesh->edge_from_halfedge(one.halfedge[i]);
				if(!m_lpHEMesh->cut_edge(idxEdgeToCut, tEdges[i], &idxNP0, &idxNP1)) {
					LogErrorArg2("Unable to cut edge %d of element %d", i, element);
				}

				//generated nodes
				vnodes[ mapEdgeToMiddleNodes[i*2 + 0] ] = idxNP0;
				vnodes[ mapEdgeToMiddleNodes[i*2 + 1] ] = idxNP1;
			}
		}
	}


	//Case A: 3 cut edges. cutEdgeCodes = { 11, 22, 37, 56 }
	if(ctCutEdges == 3) {
		//Remove the original element
		m_lpHEMesh->remove_element(element);

		//find the local table entry to handle this case A
		int entry = m_mapCutEdgeCodeToTableEntry[cutEdgeCode];

		//report
		LogInfoArg3("Detected case is type A: cutEdgeCode: %u, cutNodeCode: %u, entry: %u", cutEdgeCode, cutNodeCode, entry);

		//generate new tets
		for(int e = 0; e < 4; e++) {
			U32 n[4];
			for(int i = 0; i < 4; i++)
				n[i] = vnodes[ g_elementTableCaseA[entry][e * 4 + i] ];

			m_lpHEMesh->insert_element(n);
		}
	}
	else {
		LogErrorArg3("This case is not handled yet! cutEdgeCode = %u, cutNodeCode = %u, ctCutEdges = %u",
					 cutEdgeCode, cutNodeCode, ctCutEdges);
	}


	return cutEdgeCode;
}


void TetSubdivider::draw() {

	if(m_lpHEMesh)
		m_lpHEMesh->draw();
}

}
}

