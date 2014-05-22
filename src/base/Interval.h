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
	Interval():left(INTERVAL_EPSILON), right(FLT_MAX) {};

	Interval(T min, T max)
	{
		if(min > max)
		{
			left = max;
			right = min;
		}
		else
		{
			left = min;
			right = max;
		}
	}

	void setMin(const T min)
	{
		left = min;
	}

	void setMax(const T max)
	{
		right = max;
	}

	void set(const T l, const T r)
	{
		left = l;
		right = r;
	}

	T length() { return (right - left);}

	bool isInside(const T f) const { return ((f >= left)&&(f <= right));}


	bool hasOverlap(const Interval &A)
	{
		return (isInside(A.left))||(isInside(A.right));
	}

	Interval& operator=(const Interval& y)
	{
		this->left = y.left;
		this->right = y.right;
		return(*this);
	}

	bool operator==(const Interval& y) const
	{
		return ((this->left == y.left)&&(this->right == y.right));
	}

	bool operator!=(const Interval& y) const
	{
		return ((this->left != y.left)||(this->right != y.right));
	}
public:
	T left;
	T right;
};

typedef Interval<double> RangeD;
typedef Interval<float> RangeF;
typedef Interval<psreal> Range;
typedef Interval<int> RangeI;
}
}

#endif
