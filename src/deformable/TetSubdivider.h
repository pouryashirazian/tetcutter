/*
 * TetSubdivider.h
 *
 *  Created on: May 31, 2014
 *      Author: pourya
 */

#ifndef TETSUBDIVIDER_H_
#define TETSUBDIVIDER_H_

#include "HalfEdgeTetMesh.h"

namespace PS {
namespace FEM {

//case A
extern U32 g_elementTableCaseA[4][16];

//case B
extern U32 g_elementTableCaseB[3][6][4];



class TetSubdivider : public SGNode {
public:
	enum CUTCASE {cutA, cutB, cutC, cutD, cutE, cutX, cutY, cutZ, cutUnknown};


public:
	TetSubdivider(HalfEdgeTetMesh* pMesh);
	virtual ~TetSubdivider();

	void draw();

	void setMesh(HalfEdgeTetMesh* pMesh) { m_lpHEMesh = pMesh;}
	CUTCASE identifyCutCase(bool isCutComplete, U8 cutEdgeCode, U8 cutNodeCode, U8& countCutEdges, U8& countCutNodes);

	int subdivide(U32 element, U8 cutEdgeCode, U8 cutNodeCode, double tEdges[6], bool dosplit = true);



	/*!
	 * generates case A where a node in separated from the rest of the element. 3 edges are cut.
	 * @element: element to be considered
	 * @node: the node to be separated
	 * @targetDist: distance to the node [0-1]
	 * @cutEdgeCode: output cutedge code
	 */
	int generateCaseA(U32 element, U8 node, double targetDistPercentage,
					  U8& cutEdgeCode, U8& cutNodeCode, double (&tEdges)[6]);

	/*!
	 * generates case B where an element is sliced into two sections by cutting its 4 edges.
	 * There 3 different case that can produce this state
	 * @element: tetrahedral element to be considered
	 * @face: start face which can be 0, 1 or 2 (end face is 3)
	 * @cutEdgeCode: output cutedge code
	 * @cutNodeCode: output cutnode code
	 * @tEdges: the distance over the edges where the cuts are happening
	 */
	int generateCaseB(U32 element, U8 enteringface, U8& cutEdgeCode,
					  U8& cutNodeCode, double (&tEdges)[6]);


	bool writeLookUpTable();
protected:

	HalfEdgeTetMesh* m_lpHEMesh;

	std::map<U8, int> m_mapCutEdgeCodeToTableEntry;
};

}
}



#endif /* TETSUBDIVIDER_H_ */
