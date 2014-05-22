/*
 * Lerping.h
 *
 *  Created on: Apr 30, 2013
 *      Author: pourya
 */

#ifndef LERPING_H_
#define LERPING_H_

#include <loki/Functor.h>

using namespace Loki;

typedef Functor<void> FOnAdvance;

template <typename T>
class LerpedValue {
public:
	LerpedValue() {}
	explicit LerpedValue(const T& value, const FOnAdvance& f):m_value(value), m_fOnAdvance(f) {}

	//Value
	T value() const {return m_value;}
	void setValue(T val) { m_value = val;}

	//Target
	void bumpTarget(T delta) { m_target = m_target + delta;}
	void setTarget(T target) { m_target = target;}
	T target() const {return m_target;}

	//Set Callback
	void setCallBack(const FOnAdvance& f) { m_fOnAdvance = f;}

	void advance() {
		m_value = m_value + (m_target - m_value) * 0.5;

		if(m_fOnAdvance)
			m_fOnAdvance();
	}
private:
	T m_value;
	T m_target;
	FOnAdvance m_fOnAdvance;
};


//Lerped Vector
template <typename T>
class LerpedVector {
public:
	LerpedVector() {}
	explicit LerpedVector(const T& value, const FOnAdvance& f):m_value(value), m_fOnAdvance(f) {}

	//Value
	T value() const {return m_value;}
	void setValue(const T& val) {
		m_value = val;
	}

	//Target
	void bumpTarget(const T& delta) { m_target = m_target + delta;}
	void setTarget(const T& target) { m_target = target;}
	T target() const {return m_target;}

	//Set Callback
	void setCallBack(const FOnAdvance& f) { m_fOnAdvance = f;}

	void advance() {
		if(m_target == m_value)
			return;

		m_value = m_value + (m_target - m_value) * 0.5;
		if(m_fOnAdvance)
			m_fOnAdvance();
	}
private:
	T m_value;
	T m_target;
	FOnAdvance m_fOnAdvance;
};


#endif /* LERPING_H_ */
