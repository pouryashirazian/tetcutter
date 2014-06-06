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

class TetSubdivider : public SGNode {
public:
	TetSubdivider(HalfEdgeTetMesh* pMesh);
	virtual ~TetSubdivider();

	void draw();
	int subdivide(U32 element, U8 cutEdgeCode, U8 cutNodeCode, double tEdges[6]);

	int cutEdgesToSplitNode(U32 element, U8 node, double targetDist,
							U8& cutEdgeCode, U8& cutNodeCode, double (&tEdges)[6]);
private:
	HalfEdgeTetMesh* m_lpHEMesh;
};

}
}



#endif /* TETSUBDIVIDER_H_ */
