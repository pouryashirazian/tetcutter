/*
 * ProfileCurve.h
 *
 *  Created on: Jun 23, 2014
 *      Author: pourya
 */

#ifndef PROFILECURVE_H_
#define PROFILECURVE_H_

#include "base/Vec.h"

using namespace PS;
using namespace PS::MATH;

namespace PS {
namespace MODEL {

class IProfileCurve {
public:
	IProfileCurve() {}

	virtual float length(float t) = 0;

	virtual vec3f position(float t) = 0;
};

}
}



#endif /* PROFILECURVE_H_ */
