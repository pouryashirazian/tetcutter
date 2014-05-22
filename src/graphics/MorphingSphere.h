/*
 * MorphingSphere.h
 *
 *  Created on: Mar 1, 2014
 *      Author: pourya
 */

#ifndef MORPHINGSPHERE_H_
#define MORPHINGSPHERE_H_


#include "SGMesh.h"

namespace PS {
namespace SG {


class MorphingSphere : public SGMesh {

public:
	MorphingSphere();
	MorphingSphere(float rho, int hseg, int vseg);
	virtual ~MorphingSphere();

protected:
	void setup(float rho, int hseg, int vseg);

};

}
}

#endif /* MORPHINGSPHERE_H_ */
