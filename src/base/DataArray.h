/*
 * DataArray.h
 *
 *  Created on: Dec 25, 2013
 *      Author: pourya
 */

#ifndef DATAARRAY_H_
#define DATAARRAY_H_


#include "SIMDVecN.h"

namespace PS {

template <typename T, size_t length_>
struct PS_BEGIN_ALIGNED(PS_SIMD_FLEN) DataArray {
	T e[PS_SIMD_PADSIZE(length_)];

	//Total Size in Bytes
	size_t size() const {
		return (sizeof(T)) * PS_SIMD_PADSIZE(length_);
	}

	//Length of the array
	size_t length() const {
		return PS_SIMD_PADSIZE(length_);
	}

	//Copy From Another Array
	void copyFrom(const DataArray& rhs) {
		memcpy((void*)&e, (void*)&rhs.e, rhs.size());
	}

	T* ptr() {
		return &e[0];
	}

	const T* cptr() {
		return &e[0];
	}

	//Assignment Op
	DataArray& operator=(const DataArray& rhs) {
		this->copyFrom(rhs);
		return *this;
	}

    inline T& operator[](int index) {
    	return e[index];
    }

    inline const T& operator[](int index) const {
    	return e[index];
    }

} PS_END_ALIGNED(PS_SIMD_FLEN);

}
#endif /* DATAARRAY_H_ */
