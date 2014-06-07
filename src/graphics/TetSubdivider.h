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

extern U32 g_elementTableCaseA[4][16];




class TetSubdivider : public SGNode {
public:
	TetSubdivider(HalfEdgeTetMesh* pMesh);
	virtual ~TetSubdivider();

	void draw();

	void setMesh(HalfEdgeTetMesh* pMesh) { m_lpHEMesh = pMesh;}
	int subdivide(U32 element, U8 cutEdgeCode, U8 cutNodeCode, double tEdges[6]);

	/*!
	 * generates case A where a node in separated from the rest of the element. 3 edges are cut.
	 * @element: element to be considered
	 * @node: the node to be separated
	 * @targetDist: distance to the node [0-1]
	 * @cutEdgeCode: output cutedge code
	 */
	int generateCaseA(U32 element, U8 node, double targetDist,
					  U8& cutEdgeCode, U8& cutNodeCode, double (&tEdges)[6]);


	bool writeLookUpTable();
protected:

	HalfEdgeTetMesh* m_lpHEMesh;

	std::map<U8, int> m_mapCutEdgeCodeToTableEntry;
};

}
}



#endif /* TETSUBDIVIDER_H_ */
