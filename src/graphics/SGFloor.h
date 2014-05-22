/*
 * GroundMartrix.h
 *
 *  Created on: Mar 10, 2013
 *      Author: pourya
 */

#ifndef GROUNDMARTRIX_H_
#define GROUNDMARTRIX_H_

#include "SGMesh.h"

using namespace PS;
using namespace PS::GL;

namespace PS {
namespace SG {

class SGFloor : public SGMesh {

public:
	SGFloor(int rows = 32, int cols = 32, float step = 1.0f);
	virtual ~SGFloor();

	void draw();
};

}
}



#endif /* GROUNDMARTRIX_H_ */
