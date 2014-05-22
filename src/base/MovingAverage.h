/*
 * PS_MovingAverage.h
 *
 *  Created on: Jul 12, 2011
 *      Author: pourya
 */

#ifndef PS_MOVINGAVERAGE_H_
#define PS_MOVINGAVERAGE_H_

#include <math.h>
#include <assert.h>
#include <vector>
#include "DataArray.h"

#define DEFAULT_WINDOW_SIZE 20
using namespace PS;

namespace PS{
namespace MATH{

template <typename T, int length = DEFAULT_WINDOW_SIZE>
class MovingAvg
{
public:
	typedef T VALUETYPE;

	MovingAvg();
	virtual ~MovingAvg() {}

	void addValue(T v);

	T getValue(int idx) const;
	T getAverage() const;
	T getCurrent() const;
	int getWindowSize() {return m_szWindow;}

private:
	DataArray<T, (size_t)length> m_buffer;
	T	m_current;
	int m_szWindow;
	int m_idxCurrent;
};

//Implementation
template <typename T, int length>
MovingAvg<T, length>::MovingAvg()
{
	assert(length > 0);
	m_idxCurrent = 0;
	m_szWindow = length;
	for(int i=0; i<m_szWindow; i++)
		m_buffer[i] = (T)0;
	m_current = (T)0;
}


template<typename T, int length>
T MovingAvg<T, length>::getValue(int idx) const
{
	if(idx >=0 && idx < m_szWindow)
		return m_buffer[idx];
	else
		return static_cast<T>(-1);
}

template<typename T, int length>
void MovingAvg<T, length>::addValue(T v)
{
	m_current = v;
	m_buffer[m_idxCurrent] = v;
	m_idxCurrent = (m_idxCurrent + 1) % m_szWindow;
}

template<typename T, int length>
T MovingAvg<T, length>::getAverage() const
{
	T accum = 0;
	for(int i=0; i<m_szWindow; i++)
		accum += m_buffer[i];
	return accum / static_cast<T>(m_szWindow);
}

template<typename T, int length>
T MovingAvg<T, length>::getCurrent() const
{
	return m_current;
}


}
}

#endif /* PS_MOVINGAVERAGE_H_ */
