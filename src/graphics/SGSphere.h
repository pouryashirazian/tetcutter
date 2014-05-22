/*
 * SGSphere.h
 *
 *  Created on: Mar 31, 2014
 *      Author: pourya
 */

#ifndef SGSPHERE_H_
#define SGSPHERE_H_


#include "SGMesh.h"

using namespace PS;
using namespace PS::GL;

namespace PS {
namespace SG {

class SGSphere : public SGMesh {
public:
	SGSphere(float radius = 1.0f, int hseg = 16, int vseg = 16);
	virtual ~SGSphere();

};

}
}




#endif /* SGSPHERE_H_ */
