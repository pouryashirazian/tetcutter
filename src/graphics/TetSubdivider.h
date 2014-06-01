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
	TetSubdivider();
	TetSubdivider(const vector<double>& vertices, const vector<U32>& elements);
	virtual ~TetSubdivider();

	void draw();
	int subdivide(int element, U8 cutEdgeCode, U8 cutNodeCode);

	static TetSubdivider* CreateOneTet();
private:
	HalfEdgeTetMesh* m_lpHEMesh;
};

}
}



#endif /* TETSUBDIVIDER_H_ */
