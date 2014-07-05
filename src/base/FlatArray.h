/*
 * FlatArray.h
 *
 *  Created on: Jul 5, 2014
 *      Author: pourya
 */

#ifndef FLATARRAY_H_
#define FLATARRAY_H_

#include "Vec.h"
#include <vector>

using namespace std;
using namespace PS::MATH;

namespace PS {

template<typename T>
void FlattenVec2(const vector< Vec2<T> >& vBefore, vector<T>& vAfter) {

	vAfter.resize(vBefore.size() * 2);
	for(U32 i=0; i < vBefore.size(); i++) {
		vBefore[i].store(&vAfter[i * 2]);
	}
}

template<typename T>
void FlattenVec3(const vector< Vec3<T> >& vBefore, vector<T>& vAfter) {

	vAfter.resize(vBefore.size() * 3);
	for(U32 i=0; i < vBefore.size(); i++) {
		vBefore[i].store(&vAfter[i * 3]);
	}
}

template<typename T>
void FlattenVec4(const vector< Vec4<T> >& vBefore, vector<T>& vAfter) {

	vAfter.resize(vBefore.size() * 4);
	for(U32 i=0; i < vBefore.size(); i++) {
		vBefore[i].store(&vAfter[i * 4]);
	}
}

}




#endif /* FLATARRAY_H_ */
