#pragma once
#ifndef MATHHELPER_H
#define MATHHELPER_H

#include <limits>
#include <math.h>
#include <stdlib.h>
#include <assert.h>

#ifdef WIN32
    #define PS_OS_WINDOWS 1
#elif defined(__linux__) || defined(linux)
    #define	PS_OS_LINUX	2
#elif defined(__APPLE__)
    #define PS_OS_MAC 3
#elif defined(__QNX__)
    #define PS_OS_QNX 4
#endif

#ifdef _MSC_VER
    #define PS_COMPILER_MS
#elif defined(__GNUC__)
    #define PS_COMPILER_GNU
#endif

#if defined(_M_X64) || defined(__amd64__) || defined(_LP64)
    #define PS_POINTER_SIZE 8
#else
    #define PS_POINTER_SIZE 4
#endif


#ifdef PS_OS_LINUX
    #include <cmath>
    #include <cfloat>
#elif defined(PS_OS_MAC)
    #include <cfloat>
#endif

#define PS_PLUS_INFINITY FLT_MAX
#define PS_MINUS_INFINITY -1.0f*FLT_MAX


#ifndef SAFE_DELETE
    #define SAFE_DELETE(p) { if(p) {delete (p); (p) = NULL;}}
#endif

#ifndef SAFE_DELETE_ARRAY
    #define SAFE_DELETE_ARRAY(p) { if( (p) ) {delete [] (p); (p) = NULL;}}
#endif

#define PS_UNUSED(arg) (void)arg;

#ifndef COMFORT_TYPES
#define COMFORT_TYPES

typedef unsigned char		U8;
typedef unsigned short		U16;
typedef unsigned int		U32;
typedef			 char		I8;
typedef			 short		I16;
typedef			 int		I32;

#if defined(_MSC_VER)
typedef unsigned __int64	U64;
typedef	__int64				I64;
#else
typedef unsigned long long	U64;
typedef	long long			I64;
#endif

typedef unsigned char	u_char;
typedef unsigned short	u_short;
typedef unsigned int	u_int;

#endif

//Constants for conversion
//const float DEG_TO_RAD = ((2.0f * 3.14152654f) / 360.0f);
//const float RAD_TO_DEG = (360.0f / (2.0f * 3.141592654f));

//PI
const float Pi       =  3.14159265358979323846f;

//PI over 2
const float PiOver2  = Pi / 2.0f;

//2PI
const float TwoPi    = Pi * 2.0f;

// error tolerance for check
const float EPSILON  = 0.0001f;

//////////////////////////////////////////////////////////////////////////
// Angle Conversions
#define	DEGTORAD(x)	( ((x) * Pi) / 180.0f )
#define	RADTODEG(x)	( ((x) * 180.0f) / Pi )

#define	SQR(x)		( (x) * (x) )

//Min and Max
#define MATHMIN(_A_,_B_) ((_A_) < (_B_))?(_A_):(_B_)
#define MATHMAX(_A_,_B_) ((_A_) > (_B_))?(_A_):(_B_)

//Angle between Vectors
#define VAngleDegree(p,q) ((acos((p).x * (q).x + (p).y * (q).y + (p).z * (q).z) * 180.0f) / Pi)

// limits a value to low and high
#define LIMIT_RANGE(low, value, high)	{	if (value < low)	value = low;	else if(value > high)	value = high;	}

// set float to 0 if within tolerance
#define ZERO_CLAMP(x)	((((x) > 0 && ((x) < EPSILON)) || ((x) < 0 && ((x) > -EPSILON)))?0.0f:(x) )

//////////////////////////////////////////////////////////////////////////
inline int Log2f(float x)
{
    unsigned int ix = static_cast<unsigned int>(x);
    unsigned int exp = (ix >> 23) & 0xFF;
    int log2 = int(exp) - 127;
    return log2;
}

inline int Log2i(unsigned int x)
{
    return Log2f((float)x);
}

inline float Lerp(float t, float s1, float s2)
{
    return (1 - t)*s1 + t*s2;
}

template <class T>
inline void Clamp(T &v, T minVal, T maxVal)
{
    if(v < minVal)
        v = minVal;
    else if(v > maxVal)
        v = maxVal;
}

inline bool FLOAT_EQ(float x, float v)
{
    return ( ((v - EPSILON) < x) && (x < (v + EPSILON)) );
}

inline bool FLOAT_EQ(float x, float v, float epsi)
{
    return ( ((v - epsi) < x) && (x < (v + epsi)) );
}

//Swap 2 floating point numbers
inline void SWAP(float &x, float &y)
{
    float temp;
    temp = x;
    x = y;
    y = temp;
}

inline void SWAP(int &x, int &y)
{
    int temp;
    temp = x;
    x = y;
    y = temp;
}

//Round for Accuracy. The same method we used by AMin
// round a float to a specified degree of accuracy
inline float ROUND(const float value, const int accuracy)
{
    double integer, fraction;

    fraction = modf(value, &integer);		// get fraction and int components

    return (float(integer + (float(int(fraction*pow(float(10), float(accuracy))))) / pow(float(10), float(accuracy)) ) );
}

// If num is less than zero, we want to return the absolute value of num.
// This is simple, either we times num by -1 or subtract it from 0.
inline double Absoluted(double num)
{
    if(num < 0)
        return (0 - num);

    // Return the original number because it was already positive
    return num;
}

inline float Absolutef(float num)
{
    if(num < 0)
        return (0 - num);

    // Return the original number because it was already positive
    return num;
}

inline float maxf(float a, float b)
{
    return (a>b)?a:b;
}

inline float minf(float a, float b)
{
    return (a<b)?a:b;
}

template <class T>
inline T gmax(T a, T b)
{
    return (a > b)?a:b;
}

template <class T>
inline T gmin(T a, T b)
{
    return (a < b)?a:b;
}

#ifdef max
#undef max
#endif // #ifdef min
template <typename T>
T GetMaxLimit(void)
{
    return std::numeric_limits<T>::max();
}

#ifdef min
#undef min
#endif // #ifdef min
template <typename T>
T GetMinLimit(void)
{
    return std::numeric_limits<T>::min();
}

//Fast SquareRoot Found on internet http://www.codemaestro.com/reviews/9
/*
inline float FastSqrt(float number)
{
    long i;
    float x, y;
    const float f = 1.5F;

    x = number * 0.5F;
    y  = number;
    i  = * (reinterpret_cast<long *>(&y));
    i  = 0x5f3759df - ( i >> 1 );
    y  = * ( float * ) &i;
    y  = y * ( f - ( x * y * y ) );
    y  = y * ( f - ( x * y * y ) );
    return number * y;
}

inline float FastInvSqrt(float x)
{
    float xhalf = 0.5f*x;
    int i = *(int*)&x; // get bits for floating value
    i = 0x5f375a86- (i>>1); // gives initial guess y0
    x = *(float*)&i; // convert bits back to float
    x = x*(1.5f-xhalf*x*x); // Newton step, repeating increases accuracy
    return x;
}
*/

template <typename T>
inline T RandRangeT(T nMin, T nMax)
{
    // random double in range 0.0 to 1.0 (non inclusive)
    T r = static_cast<T>(rand())/(static_cast<T>(RAND_MAX) + 1);
    // transform to wanted range
    return nMin + r*(nMax - nMin);
}

//Floating Point Comparison
inline bool ApproximatelyEqual(float a, float b, float epsilon)
{
    return fabs(a - b) <= ( (fabs(a) < fabs(b) ? fabs(b) : fabs(a)) * epsilon);
}

inline bool EssentiallyEqual(float a, float b, float epsilon)
{
    return fabs(a - b) <= ( (fabs(a) > fabs(b) ? fabs(b) : fabs(a)) * epsilon);
}

inline bool EssentiallyEquald(double a, double b, double epsilon)
{
    return fabs(a - b) <= ( (fabs(a) > fabs(b) ? fabs(b) : fabs(a)) * epsilon);
}

inline bool DefinitelyGreaterThan(float a, float b, float epsilon)
{
    return (a - b) > ( (fabs(a) < fabs(b) ? fabs(b) : fabs(a)) * epsilon);
}

inline bool DefinitelyLessThan(float a, float b, float epsilon)
{
    return (b - a) > ( (fabs(a) < fabs(b) ? fabs(b) : fabs(a)) * epsilon);
}

inline float MAXF(float a, float b)
{
    if(DefinitelyGreaterThan(a, b, EPSILON))
        return a;
    else
        return b;
}

inline float MINF(float a, float b)
{
    if(DefinitelyLessThan(a, b, EPSILON))
        return a;
    else
        return b;
}

#endif
