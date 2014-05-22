/*!
  * Namespace: PS::MATH
  * Synopsis: 2,3,4 Dimensional generic vector math. Clean implementation with template classes.
  * Author: Pourya Shirazian
  */
#ifndef PS_VECTOR_H
#define PS_VECTOR_H

#include "MathBase.h"

namespace PS{
namespace MATH{

enum ScalarType {stFloat, stDouble};
typedef float psreal;

/*!
  * 2D Vector arithmetic
  */
template<typename T>
class Vec2{
public:
    //Constructors
    Vec2() {}
    Vec2(T x_, T y_):x(x_), y(y_) {}
    Vec2(const Vec2& rhs):x(rhs.x), y(rhs.y) {}
    Vec2(const T* pValues) {
        load(pValues);
    }

    //Load and Store
    void load(const T* pValues) {
        x = pValues[0];
        y = pValues[1];
    }

    void store(const T* pValues) const {
        pValues[0] = x;
        pValues[1] = y;
    }


    //Public this functions
    inline void normalize();
    inline Vec2 normalized() const;
    inline T length() const;
    inline T length2() const;

    //Axis
    inline T element(int i) const;
    inline void setElement(int i, T v);
    inline int longestAxis() const;

    T* ptr() {return &e[0];}
    const T* cptr() const {return &e[0];}


    //Static Functions
    static T dot(const Vec2& a, const Vec2& b);
    static T angleDeg(const Vec2& a, const Vec2& b);
    static Vec2 reflect(const Vec2& a, const Vec2& n);
    static T distance(const Vec2& a, const Vec2& b);
    static Vec2 minP(const Vec2& a, const Vec2& b);
    static Vec2 maxP(const Vec2& a, const Vec2& b);

    //Primary operators
    static Vec2 mul(const Vec2& a, const Vec2& b);
    static Vec2 mul(T a, const Vec2& b);
    static Vec2 div(const Vec2& a, const Vec2& b);
    static Vec2 add(const Vec2& a, const Vec2& b);
    static Vec2 sub(const Vec2& a, const Vec2& b);


    //Operators
    inline Vec2& operator=(const Vec2& rhs);
    inline Vec2 operator*(T s) const;
    inline Vec2 operator+(const Vec2& rhs) const;
    inline Vec2 operator-(const Vec2& rhs) const;
    inline bool operator==(const Vec2& rhs) const;
    inline T& operator[](int index);
    inline const T& operator[](int index) const;
public:
    union{
        struct{
            T x;
            T y;
        };
        T e[2];
    };
};

template<typename T>
inline void Vec2<T>::normalize(){
    T dInv = 1.0 / sqrt(x*x + y*y);
    x *= dInv;
    y *= dInv;
}

template<typename T>
inline Vec2<T> Vec2<T>::normalized() const{
    Vec2<T> result = (*this);
    result.normalize();
    return result;
}

template<typename T>
inline T Vec2<T>::length() const{
    return sqrt(x*x + y*y);
}

template<typename T>
inline T Vec2<T>::length2() const{
    return (x*x + y*y);
}

template<typename T>
inline T Vec2<T>::element(int i) const
{
    assert(i >=0 && i < 3);
    return this->e[i];
}

template<typename T>
inline void Vec2<T>::setElement(int i, T v)
{
    assert(i >=0 && i < 3);
    this->e[i] = v;
}

template<typename T>
inline int Vec2<T>::longestAxis() const {
	return (x > y)? 0:1;
}

template<typename T>
Vec2<T>& Vec2<T>::operator=(const Vec2<T>& rhs)
{
    this->x = rhs.x;
    this->y = rhs.y;
    return (*this);
}

template<typename T>
Vec2<T> Vec2<T>::operator*(T s) const
{
    Vec2<T> result;
    result.x = x * s;
    result.y = y * s;
    return result;
}

template<typename T>
Vec2<T> Vec2<T>::operator+(const Vec2<T>& rhs) const
{
    Vec2<T> result;
    result.x = x + rhs.x;
    result.y = y + rhs.y;
    return result;
}

template<typename T>
Vec2<T> Vec2<T>::operator-(const Vec2<T>& rhs) const
{
    Vec2<T> result;
    result.x = x - rhs.x;
    result.y = y - rhs.y;
    return result;
}

template<typename T>
bool Vec2<T>::operator==(const Vec2<T>& rhs) const
{
    return (x == rhs.x && y == rhs.y);
}

template<typename T>
T& Vec2<T>::operator[](int index) {
	return e[index];
}

template<typename T>
const T& Vec2<T>::operator[](int index) const {
	return e[index];
}

template<typename T>
T Vec2<T>::dot(const Vec2<T>& a, const Vec2<T>& b)
{
    return a.x * b.x + a.y * b.y;
}

template<typename T>
T Vec2<T>::angleDeg(const Vec2<T>& a, const Vec2<T>& b)
{
    Vec2<T> aN = a.normalized();
    Vec2<T> bN = b.normalized();
    return RADTODEG(acos(dot(aN, bN)));
}

template<typename T>
Vec2<T> Vec2<T>::reflect(const Vec2<T>& a, const Vec2<T>& n)
{
    Vec2<T> result = - a + 2.0 * dot(a, n);
    return result;
}

template<typename T>
T Vec2<T>::distance(const Vec2<T>& a, const Vec2<T>& b)
{
    return (b - a).length();
}

template<typename T>
Vec2<T> Vec2<T>::minP(const Vec2& a, const Vec2& b)
{
    Vec2<T> result;
    result.x = MATHMIN(a.x, b.x);
    result.y = MATHMIN(a.y, b.y);
    return result;
}

template<typename T>
Vec2<T> Vec2<T>::maxP(const Vec2& a, const Vec2& b)
{
    Vec2<T> result;
    result.x = MATHMAX(a.x, b.x);
    result.y = MATHMAX(a.y, b.y);
    return result;
}

template<typename T>
Vec2<T> Vec2<T>::mul(const Vec2& a, const Vec2& b) {
    Vec2<T> result;
    result.x = a.x * b.x;
    result.y = a.y * b.y;
    return result;
}

template<typename T>
Vec2<T> Vec2<T>::mul(T a, const Vec2& b) {
    Vec2<T> result;
    result.x = a * b.x;
    result.y = a * b.y;
    return result;
}

template<typename T>
Vec2<T> Vec2<T>::div(const Vec2& a, const Vec2& b) {
    Vec2<T> result;
    result.x = a.x / b.x;
    result.y = a.y / b.y;
    return result;
}

template<typename T>
Vec2<T> Vec2<T>::add(const Vec2& a, const Vec2& b) {
    Vec2<T> result;
    result.x = a.x + b.x;
    result.y = a.y + b.y;
    return result;
}

template<typename T>
Vec2<T> Vec2<T>::sub(const Vec2& a, const Vec2& b) {
    Vec2<T> result;
    result.x = a.x - b.x;
    result.y = a.y - b.y;
    return result;
}

typedef Vec2<double> vec2d;
typedef Vec2<float> vec2f;
typedef Vec2<psreal> vec2;
typedef Vec2<bool> vec2b;

typedef Vec2<I32>   vec2i;
typedef Vec2<U32>	 vec2u;

typedef Vec2<I8>	vec2i8;
typedef Vec2<U8>	vec2u8;
typedef Vec2<I16>   vec2i16;
typedef Vec2<U16>	 vec2u16;
typedef Vec2<I32>   vec2i32;
typedef Vec2<U32>	 vec2u32;
typedef Vec2<I64>   vec2i64;
typedef Vec2<U64>	 vec2u64;

////////////////////////////////////////////////////////////////
/*!
  * 3D Vector arithmetic
  */
template<typename T>
class Vec3{
public:
    //Constructors
    Vec3() {}
    Vec3(T a_):x(a_), y(a_), z(a_) {}
    Vec3(T x_, T y_, T z_):x(x_), y(y_), z(z_) {}
    Vec3(const Vec3& rhs):x(rhs.x), y(rhs.y), z(rhs.z) {}
    Vec3(const T* pValues) {
        load(pValues);
    }

    //Load and Store
    void load(const T* pValues) {
        x = pValues[0];
        y = pValues[1];
        z = pValues[2];
    }

    void store(T* pValues) const {
        pValues[0] = x;
        pValues[1] = y;
        pValues[2] = z;
    }

    //Public this functions
    inline void normalize();
    inline Vec3 normalized() const;
    inline T length() const;
    inline T length2() const;


    //Axis Ops
    inline T element(int i) const;
    inline void setElement(int i, T v);
    inline int longestAxis() const;

    //Pointers
    T* ptr() {return &e[0];}
    const T* cptr() const {return &e[0];}

    //Static Functions
    static T dot(const Vec3& a, const Vec3& b);
    static Vec3 cross(const Vec3& a, const Vec3& b);
    static T angleDeg(const Vec3& a, const Vec3& b);
    static Vec3 reflect(const Vec3& a, const Vec3& n);
    static T distance(const Vec3& a, const Vec3& b);
    static Vec3 minP(const Vec3& a, const Vec3& b);
    static Vec3 maxP(const Vec3& a, const Vec3& b);
    static T cubeSurface(const Vec3& lo, const Vec3& hi);

    //Primary Operator
    static Vec3 mul(const Vec3& a, const Vec3& b);
    static Vec3 mul(T a, const Vec3& b);
    static Vec3 div(const Vec3& a, const Vec3& b);
    static Vec3 add(const Vec3& a, const Vec3& b);
    static Vec3 sub(const Vec3& a, const Vec3& b);


    //Operators
    inline Vec3& operator=(const Vec3& rhs);
    inline Vec3 operator*(T s) const;

    inline Vec3 operator+(const Vec3& rhs) const;
    inline Vec3 operator-(const Vec3& rhs) const;
    inline T& operator[](int index);
    inline const T& operator[](int index) const;
public:
    union{
        struct{
            T x;
            T y;
            T z;
        };
        T e[3];
    };
};

template<typename T>
inline void Vec3<T>::normalize(){
	T len = sqrt(x*x + y*y + z*z);
	if(len == 0.0)
		return;
    T dInv = 1.0 / len;
    x *= dInv;
    y *= dInv;
    z *= dInv;
}

template<typename T>
inline Vec3<T> Vec3<T>::normalized() const {
    Vec3<T> result = (*this);
    result.normalize();
    return result;
}

template<typename T>
inline T Vec3<T>::length() const {
    return sqrt(x*x + y*y + z*z);
}

template<typename T>
inline T Vec3<T>::length2() const {
	return (x*x + y*y + z*z);
}

template<typename T>
inline T Vec3<T>::element(int i) const
{
    assert(i >=0 && i < 3);
    return this->e[i];
}

template<typename T>
inline void Vec3<T>::setElement(int i, T v)
{
    assert(i >=0 && i < 3);
    this->e[i] = v;
}

template<typename T>
inline int Vec3<T>::longestAxis() const {
	if(x > y && x > z)
		return 0;
	else if(y > z)
		return 1;
	else
		return 2;
}

template<typename T>
Vec3<T>& Vec3<T>::operator=(const Vec3<T>& rhs)
{
    this->x = rhs.x;
    this->y = rhs.y;
    this->z = rhs.z;
    return (*this);
}

template<typename T>
Vec3<T> Vec3<T>::operator*(T s) const
{
    Vec3<T> result;
    result.x = x * s;
    result.y = y * s;
    result.z = z * s;
    return result;
}

template<typename T>
Vec3<T> Vec3<T>::operator+(const Vec3<T>& rhs) const
{
    Vec3<T> result;
    result.x = x + rhs.x;
    result.y = y + rhs.y;
    result.z = z + rhs.z;
    return result;
}

template<typename T>
Vec3<T> Vec3<T>::operator-(const Vec3<T>& rhs) const
{
    Vec3<T> result;
    result.x = x - rhs.x;
    result.y = y - rhs.y;
    result.z = z - rhs.z;
    return result;
}

template<typename T>
T& Vec3<T>::operator[](int index) {
	return e[index];
}

template<typename T>
const T& Vec3<T>::operator[](int index) const {
	return e[index];
}

template<typename T>
T Vec3<T>::dot(const Vec3<T>& a, const Vec3<T>& b)
{
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

template<typename T>
Vec3<T> Vec3<T>::cross(const Vec3<T>& a, const Vec3<T>& b)
{
    Vec3<T> result;
    result.x = a.y * b.z - a.z * b.y;
    result.y = a.z * b.x - a.x * b.z;
    result.z = a.x * b.y - a.y * b.x;
    return result;
}

template<typename T>
T Vec3<T>::angleDeg(const Vec3<T>& a, const Vec3<T>& b)
{
    Vec3<T> aN = a.normalized();
    Vec3<T> bN = b.normalized();
    return RADTODEG(acos(dot(aN, bN)));
}

template<typename T>
Vec3<T> Vec3<T>::reflect(const Vec3<T>& a, const Vec3<T>& n)
{
    //Vec3<T> result = a * static_cast<T>(-1.0) + dot(a, n) * static_cast<T>(2.0);
    Vec3<T> result = a * (-1.0) + n * 2.0 * dot(a, n);
    return result;
}

template<typename T>
T Vec3<T>::distance(const Vec3<T>& a, const Vec3<T>& b)
{
    return (b - a).length();
}

template<typename T>
Vec3<T> Vec3<T>::minP(const Vec3& a, const Vec3& b)
{
    Vec3<T> result;
    result.x = MATHMIN(a.x, b.x);
    result.y = MATHMIN(a.y, b.y);
    result.z = MATHMIN(a.z, b.z);

    return result;
}

template<typename T>
Vec3<T> Vec3<T>::maxP(const Vec3& a, const Vec3& b)
{
    Vec3<T> result;
    result.x = MATHMAX(a.x, b.x);
    result.y = MATHMAX(a.y, b.y);
    result.z = MATHMAX(a.z, b.z);

    return result;
}

template<typename T>
T Vec3<T>::cubeSurface(const Vec3& lo, const Vec3& hi)
{
	Vec3 d = Vec3::sub(hi, lo);
	return 2.0 * (d.x * d.y + d.y * d.z + d.x * d.z);
}

template<typename T>
Vec3<T> Vec3<T>::mul(const Vec3& a, const Vec3& b) {
    Vec3<T> result;
    result.x = a.x * b.x;
    result.y = a.y * b.y;
    result.z = a.z * b.z;
    return result;
}

template<typename T>
Vec3<T> Vec3<T>::mul(T a, const Vec3& b) {
    Vec3<T> result;
    result.x = a * b.x;
    result.y = a * b.y;
    result.z = a * b.z;
    return result;
}

template<typename T>
Vec3<T> Vec3<T>::div(const Vec3& a, const Vec3& b) {
    Vec3<T> result;
    result.x = a.x / b.x;
    result.y = a.y / b.y;
    result.z = a.z / b.z;
    return result;
}

template<typename T>
Vec3<T> Vec3<T>::add(const Vec3& a, const Vec3& b) {
    Vec3<T> result;
    result.x = a.x + b.x;
    result.y = a.y + b.y;
    result.z = a.z + b.z;
    return result;
}

template<typename T>
Vec3<T> Vec3<T>::sub(const Vec3& a, const Vec3& b) {
    Vec3<T> result;
    result.x = a.x - b.x;
    result.y = a.y - b.y;
    result.z = a.z - b.z;
    return result;
}


typedef Vec3<double> vec3d;
typedef Vec3<float> vec3f;
typedef Vec3<psreal> vec3;
typedef Vec3<bool>  vec3b;

typedef Vec3<I32>   vec3i;
typedef Vec3<U32>	 vec3u;

typedef Vec3<I8>	vec3i8;
typedef Vec3<U8>	vec3u8;
typedef Vec3<I16>   vec3i16;
typedef Vec3<U16>	 vec3u16;
typedef Vec3<I32>   vec3i32;
typedef Vec3<U32>	 vec3u32;
typedef Vec3<I64>   vec3i64;
typedef Vec3<U64>	 vec3u64;


////////////////////////////////////////////////////////////////
/*!
  * 4D Vector arithmetic
  */
template<typename T>
class Vec4{
public:
    //Constructors
    Vec4() {}
    explicit Vec4(T a_):x(a_), y(a_), z(a_), w(a_) {}
    explicit Vec4(T x_, T y_, T z_, T w_ = 1.0f) :x(x_), y(y_), z(z_), w(w_) {}
    explicit Vec4(const Vec2<T>& vl2, const Vec2<T>& vr2):x(vl2.x), y(vl2.y), z(vr2.x), w(vr2.y) {}
    explicit Vec4(const Vec3<T>& v3, T w_):x(v3.x), y(v3.y), z(v3.z), w(w_) {}
    Vec4(const Vec4& rhs):x(rhs.x), y(rhs.y), z(rhs.z), w(rhs.w) {}
    Vec4(const T* pValues) {
        load(pValues);
    }

    //Load and Store
    void load(const T* pValues) {
        x = pValues[0];
        y = pValues[1];
        z = pValues[2];
        w = pValues[3];
    }

    void store(T* pValues) const {
        pValues[0] = x;
        pValues[1] = y;
        pValues[2] = z;
        pValues[3] = w;
    }


    /*!
      * Get the xyz part of the 4D vector
      */
    inline Vec3<T> xyz() const;
    inline T element(int i) const;
    inline void setElement(int i, T v);
    T* ptr() {return &e[0];}
    const T* cptr() const {return &e[0];}

    /*!
      * The dot product of two 4D vectors
      */
    static T dot(const Vec4<T>& a, const Vec4<T>& b);
    static Vec4 minP(const Vec4& a, const Vec4& b);
    static Vec4 maxP(const Vec4& a, const Vec4& b);
    static Vec4 clamped(const Vec4& a, float aMin, float aMax);

    //Primary Operators
    static Vec4 mul(const Vec4& a, const Vec4& b);
    static Vec4 mul(T a, const Vec4& b);
    static Vec4 div(const Vec4& a, const Vec4& b);
    static Vec4 add(const Vec4& a, const Vec4& b);
    static Vec4 sub(const Vec4& a, const Vec4& b);


    //Operators
    inline Vec4& operator=(const Vec4& rhs);
    inline Vec4 operator*(T s) const;
    inline Vec4 operator+(const Vec4& rhs) const;
    inline Vec4 operator-(const Vec4& rhs) const;
    inline T& operator[](int index);
    inline const T& operator[](int index) const;
public:
    union{
        struct{
            T x;
            T y;
            T z;
            T w;
        };
        T e[4];
    };
};

//Implementation

//template <typename T>
//Vec4<T>::Vec4<T>(const T *lpValues)


template<typename T>
Vec3<T> Vec4<T>::xyz() const
{
    return Vec3<T>(this->x, this->y, this->z);
}

template<typename T>
T Vec4<T>::element(int i) const
{
    assert(i >=0 && i < 4);
    return this->e[i];
}

template<typename T>
void Vec4<T>::setElement(int i, T v)
{
    assert(i >=0 && i < 4);
    this->e[i] = v;
}


template<typename T>
T Vec4<T>::dot(const Vec4<T>& a, const Vec4<T>& b)
{
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

template<typename T>
Vec4<T> Vec4<T>::minP(const Vec4& a, const Vec4& b) {
    Vec4<T> result;
    result.x = MATHMIN(a.x, b.x);
    result.y = MATHMIN(a.y, b.y);
    result.z = MATHMIN(a.z, b.z);
    result.w = MATHMIN(a.w, b.w);
    return result;
}

template<typename T>
Vec4<T> Vec4<T>::maxP(const Vec4& a, const Vec4& b) {
    Vec4<T> result;
    result.x = MATHMAX(a.x, b.x);
    result.y = MATHMAX(a.y, b.y);
    result.z = MATHMAX(a.z, b.z);
    result.w = MATHMAX(a.w, b.w);
    return result;
}

template<typename T>
Vec4<T> Vec4<T>::mul(const Vec4& a, const Vec4& b) {
    Vec4<T> result;
    result.x = a.x * b.x;
    result.y = a.y * b.y;
    result.z = a.z * b.z;
    result.w = a.w * b.w;
    return result;
}

template<typename T>
Vec4<T> Vec4<T>::mul(T a, const Vec4& b) {
    Vec4<T> result;
    result.x = a * b.x;
    result.y = a * b.y;
    result.z = a * b.z;
    result.w = a * b.w;
    return result;
}


template<typename T>
Vec4<T> Vec4<T>::div(const Vec4& a, const Vec4& b) {
    Vec4<T> result;
    result.x = a.x / b.x;
    result.y = a.y / b.y;
    result.z = a.z / b.z;
    result.w = a.w / b.w;
    return result;
}

template<typename T>
Vec4<T> Vec4<T>::add(const Vec4& a, const Vec4& b) {
    Vec4<T> result;
    result.x = a.x + b.x;
    result.y = a.y + b.y;
    result.z = a.z + b.z;
    result.w = a.w + b.w;
    return result;
}

template<typename T>
Vec4<T> Vec4<T>::sub(const Vec4& a, const Vec4& b) {
    Vec4<T> result;
    result.x = a.x - b.x;
    result.y = a.y - b.y;
    result.z = a.z - b.z;
    result.w = a.w - b.w;
    return result;
}

template<typename T>
Vec4<T> Vec4<T>::clamped(const Vec4& a, float aMin, float aMax) {
	Vec4<T> result = a;
	Clamp<T>(result.x, aMin, aMax);
	Clamp<T>(result.y, aMin, aMax);
	Clamp<T>(result.z, aMin, aMax);
	Clamp<T>(result.w, aMin, aMax);
	return result;
}

template<typename T>
Vec4<T>& Vec4<T>::operator=(const Vec4<T>& rhs)
{
    this->x = rhs.x;
    this->y = rhs.y;
    this->z = rhs.z;
    this->w = rhs.w;
    return (*this);
}

template<typename T>
Vec4<T> Vec4<T>::operator*(T s) const
{
    Vec4<T> result;
    result.x = x * s;
    result.y = y * s;
    result.z = z * s;
    result.w = w * s;
    return result;
}

template<typename T>
Vec4<T> Vec4<T>::operator+(const Vec4<T>& rhs) const
{
    Vec4<T> result;
    result.x = x + rhs.x;
    result.y = y + rhs.y;
    result.z = z + rhs.z;
    result.w = w + rhs.w;
    return result;
}

template<typename T>
Vec4<T> Vec4<T>::operator-(const Vec4<T>& rhs) const
{
    Vec4<T> result;
    result.x = x - rhs.x;
    result.y = y - rhs.y;
    result.z = z - rhs.z;
    result.w = w - rhs.w;
    return result;
}

template<typename T>
T& Vec4<T>::operator[](int index) {
	return e[index];
}

template<typename T>
const T& Vec4<T>::operator[](int index) const {
	return e[index];
}

typedef Vec4<double> vec4d;
typedef Vec4<float> vec4f;
typedef Vec4<psreal> vec4;
typedef Vec4<bool> vec4b;

typedef Vec4<I32>   vec4i;
typedef Vec4<U32>	 vec4u;

typedef Vec4<I8>	vec4i8;
typedef Vec4<U8>	vec4u8;
typedef Vec4<I16>   vec4i16;
typedef Vec4<U16>	 vec4u16;
typedef Vec4<I32>   vec4i32;
typedef Vec4<U32>	 vec4u32;
typedef Vec4<I64>   vec4i64;
typedef Vec4<U64>	 vec4u64;

}
}
#endif // PS_VECTOR_H
