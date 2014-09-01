//***************************************************************************
// Author: Pourya Shirazian 
// Quaternion math for efficient rotations. Use quaternions to avoid 
// gymbal lock problem.
//***************************************************************************
#ifndef PS_QUATERNION_H
#define PS_QUATERNION_H

#include <math.h>
#include "Vec.h"

namespace PS{
namespace MATH{

// quaternions are always represent as type float.
// Represents 3d rotations as a quaternion number.
template <typename T>
class Quaternion
{
public:
    Quaternion() { identity();}
    Quaternion(const Quaternion& rhs):x(rhs.x),y(rhs.y),z(rhs.z), w(rhs.w) {}
    Quaternion(float x_, float y_, float z_, float w_):x(x_),y(y_),z(z_), w(w_) {}
    Quaternion(const vec3f& q_, float w_):x(q_.x),y(q_.y),z(q_.z), w(w_) {}
    Quaternion(const vec4f& q_):x(q_.x),y(q_.y),z(q_.z), w(q_.w) {}

	void identity() {
        x = y = z = 0.0;
		w = 1;
	}

	//As Vec3
	Vec3<T> xyz() const {
		return Vec3<T>(x, y, z);
	}

	//As Vec4
	Vec4<T> xyzw() const {
		return Vec4<T>(x, y, z, w);
	}

    //Normalize Quaternion
    void normalize();

	//transform
    Vec3<T> transform(const Vec3<T>& p) const;
    Vec3<T> transform(const Quaternion& inv, const Vec3<T>& p) const;

    //Convert To/From Euler
    void fromEuler(T roll, T pitch, T yaw);
    void toEuler(T& roll, T& pitch, T& yaw) const;

    // Convert axis/angle
    void fromAxisAngle(const Vec3<T>& axis, T deg);
    void toAxisAngle(Vec3<T>& axis, T& deg) const;

    // Taking the reciprocal of a quaternion makes its rotation go the other way
    void reciprocal();
    void invert();
    Quaternion inverted() const;

    //Ops
    Quaternion mul(const Quaternion& b) const;
    static Quaternion mul(const Quaternion& a, const Quaternion &b);


    //overloads
    Quaternion& operator =(const Quaternion& rhs);
    bool operator==(const Quaternion &rhs) const;
    Quaternion operator*(const Quaternion& rhs) const;
    inline T& operator[](int index);
    inline const T& operator[](int index) const;
public:
    //x/y/z/w components of quaternion.
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

template <typename T>
void Quaternion<T>::normalize()
{
    T	dist, square;

    square = x * x + y * y + z * z + w * w;
    if (square > 0.0f)
        dist = (T)(1.0f / sqrt(square));
    else
        dist = 1;

    x *= dist;
    y *= dist;
    z *= dist;
    w *= dist;
}

template <typename T>
Vec3<T> Quaternion<T>::transform(const Vec3<T>& p) const
{
    Quaternion input(p, 0.0f);
    Quaternion inv = inverted();

    Quaternion res = mul(input);
    return res.mul(inv).xyz();
}

template <typename T>
Vec3<T> Quaternion<T>::transform(const Quaternion& inv, const Vec3<T>& p) const
{
    Quaternion input(p, 0.0f);
    Quaternion res = mul(input);
    return res.mul(inv).xyz();
}

// build quaternion based on euler angles
template <typename T>
void Quaternion<T>::fromEuler(T roll, T pitch, T yaw)
{
    roll  *= 0.5f;
    pitch *= 0.5f;
    yaw   *= 0.5f;

    T cr = (T)cos(roll);
    T cp = (T)cos(pitch);
    T cy = (T)cos(yaw);

    T sr = (T)sin(roll);
    T sp = (T)sin(pitch);
    T sy = (T)sin(yaw);

    T cpcy = cp * cy;
    T spsy = sp * sy;
    T spcy = sp * cy;
    T cpsy = cp * sy;

    w = cr * cpcy + sr * spsy;
    x = sr * cpcy - cr * spsy;
    y = cr * spcy + sr * cpsy;
    z = cr * cpsy - sr * spcy;
}

template <typename T>
void Quaternion<T>::toEuler(T& roll, T& pitch, T& yaw ) const
{
    T sint		= (2.0f * w * y) - (2.0f * x * z);
    T cost_temp = 1.0f - (sint * sint);
    T cost		= 0;

    if ( (T)fabs(cost_temp) > 0.001f )
    {
        cost = sqrt( cost_temp );
    }

    T sinv, cosv, sinf, cosf;
    if ( (T)fabs(cost) > 0.001f )
    {
        sinv = ((2.0f * y * z) + (2.0f * w * x)) / cost;
        cosv = (1.0f - (2.0f * x * x) - (2.0f * y * y)) / cost;
        sinf = ((2.0f * x * y) + (2.0f * w * z)) / cost;
        cosf = (1.0f - (2.0f * y * y) - (2.0f * z * z)) / cost;
    }
    else
    {
        sinv = (2.0f * w * x) - (2.0f * y * z);
        cosv = 1.0f - (2.0f * x * x) - (2.0f * z * z);
        sinf = 0;
        cosf = 1.0f;
    }

    // compute output rotations
    roll	= atan2( sinv, cosv );
    pitch	= atan2( sint, cost );
    yaw		= atan2( sinf, cosf );
}

template <typename T>
void Quaternion<T>::fromAxisAngle(const Vec3<T>& axis, T deg)
{
    T halftheta    = DEGTORAD(deg) * 0.5;
    T sinHalfTheta = (T)sin( halftheta );
    T cosHalfTheta = (T)cos( halftheta );

    x = axis.x*sinHalfTheta;
    y = axis.y*sinHalfTheta;
    z = axis.z*sinHalfTheta;
    w = cosHalfTheta;
}

template <typename T>
void Quaternion<T>::toAxisAngle(Vec3<T>& axis, T& deg) const
{
    deg = RADTODEG(acos(w) * 2.0);
    T sa = sqrt(1.0f - w*w);
    if (sa)
        axis = Vec3<T>(x/sa, y/sa, z/sa);
    else
        axis = Vec3<T>(1,0,0);
}


template <typename T>
void Quaternion<T>::reciprocal()
{
    x = -x;
    y = -y;
    z = -z;
}

template <typename T>
void Quaternion<T>::invert()
{
    T square = x * x + y * y + z * z + w * w;
    if(square == 0.0)
        square = 1.0;
    T coeff = 1.0/square;
    x = x * (- coeff);
    y = y * (- coeff);
    z = z * (- coeff);
    w = w * coeff;
}

template <typename T>
Quaternion<T> Quaternion<T>::inverted() const
{
    Quaternion res = *this;
    res.invert();
    return res;
}

template <typename T>
Quaternion<T> Quaternion<T>::mul(const Quaternion& b) const {
	return mul(*this, b);
}

template <typename T>
Quaternion<T> Quaternion<T>::mul(const Quaternion& a, const Quaternion &b)
{
    Quaternion res;
    res.x = b.w * a.x + b.x * a.w + b.y * a.z - b.z * a.y;
    res.y = b.w * a.y + b.y * a.w + b.z * a.x - b.x * a.z;
    res.z = b.w * a.z + b.z * a.w + b.x * a.y - b.y * a.x;
    res.w = b.w * a.w - b.x * a.x - b.y * a.y - b.z * a.z;
    res.normalize();
    return res;
}

template <typename T>
Quaternion<T>& Quaternion<T>::operator=(const Quaternion& rhs)
{
    x = rhs.x;
    y = rhs.y;
    z = rhs.z;
    w = rhs.w;
    return(*this);
}

template <typename T>
bool Quaternion<T>::operator==(const Quaternion &rhs) const
{
    return ( rhs.x == x && rhs.y == y && rhs.z == z && rhs.w == w );
}

template <typename T>
Quaternion<T> Quaternion<T>::operator*(const Quaternion& rhs) const
{
	return mul(*this, rhs);
}

template<typename T>
T& Quaternion<T>::operator[](int index) {
	return e[index];
}

template<typename T>
const T& Quaternion<T>::operator[](int index) const {
	return e[index];
}

typedef Quaternion<psreal> quat;
typedef Quaternion<float> quatf;
typedef Quaternion<double> quatd;

}
}
#endif
