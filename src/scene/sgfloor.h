#ifndef SGFLOOR_H
#define SGFLOOR_H

#include "sgmesh.h"

using namespace ps;
using namespace ps::opengl;

namespace ps {
namespace scene {

class SGFloor : public SGMesh {

public:
	SGFloor(int rows = 32, int cols = 32, float step = 1.0f);
	virtual ~SGFloor();

	void draw();
};

}
}



#endif /* GROUNDMARTRIX_H_ */
