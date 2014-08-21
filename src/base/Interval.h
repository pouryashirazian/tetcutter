#ifndef CINTERVAL_H
#define CINTERVAL_H

#include <float.h>

#define INTERVAL_EPSILON 0.0001f

namespace PS{
	namespace MATH{

template <typename T>
class Interval
{

public:
	Interval():m_lower(INTERVAL_EPSILON), m_upper(FLT_MAX) {};

	Interval(T lower, T upper) {
		if (lower > upper) {
			m_lower = upper;
			m_upper = lower;
		} else {
			m_lower = lower;
			m_upper = upper;
		}
	}

	void setLower(const T lower)
	{
		m_lower = lower;
	}

	void setUpper(const T max)
	{
		m_upper = max;
	}

	void set(const T lower, const T upper)
	{
		m_lower = lower;
		m_upper = upper;
	}

	T length() { return (m_upper - m_lower);}

	bool isInside(const T f) const { return ((f >= m_lower)&&(f <= m_upper));}


	bool hasOverlap(const Interval &A)
	{
		return (isInside(A.m_lower))||(isInside(A.m_upper));
	}

	T lower() const { return m_lower;}
	T upper() const {return m_upper;}

	Interval& operator=(const Interval& y)
	{
		this->m_lower = y.m_lower;
		this->m_upper = y.m_upper;
		return(*this);
	}

	bool operator==(const Interval& y) const
	{
		return ((this->m_lower == y.m_lower)&&(this->m_upper == y.m_upper));
	}

	bool operator!=(const Interval& y) const
	{
		return ((this->m_lower != y.m_lower)||(this->m_upper != y.m_upper));
	}

private:
	T m_lower;
	T m_upper;
};

typedef Interval<double> RangeD;
typedef Interval<float> RangeF;
typedef Interval<psreal> Range;
typedef Interval<int> RangeI;
}
}

#endif
