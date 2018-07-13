/**
 * Vector class.
 * Common mathematical operations on vectors in R3.
 *
 * Written by Robert Osada, March 1999.
 **/
#ifndef __VECTOR_H__
#define __VECTOR_H__

#include <cmath>
#include <cassert>
#include <iostream>

static const double MI_PI = 3.14159265358979323846;
static const double DOUBLE_EPSILON  = 1e-10;

/**
 * Vector2
 **/
template <class T>
struct Vector2
{
  // create a vector
  Vector2 (T x_=0, T y_=0) : x(x_), y(y_) {}

  // set coordinates
  void set (T x_, T y_) { x=x_; y=y_; }

  // norm
  Vector2 normalize  () const { return (*this) / norm();    }
  double norm        () const { return sqrt(normSquared()); }
  T      normSquared () const { return x*x+y*y;			    }

  // boolean operators
  bool operator == (const Vector2& v) const { return x==v.x && y==v.y; }
  bool operator != (const Vector2& v) const { return x!=v.x || y!=v.y; }
  bool operator <  (const Vector2& v) const { return x < v.x || (x == v.x && (y < v.y)); }

  // operators
  Vector2  operator +  (const Vector2 &v) const { return Vector2(x+v.x, y+v.y); }
  Vector2& operator += (const Vector2 &v)       { x+=v.x; y+=v.y; return *this; }
  Vector2  operator -  () const                 { return Vector2(-x, -y); }
  Vector2  operator -  (const Vector2 &v) const { return Vector2(x-v.x, y-v.y); }
  Vector2& operator -= (const Vector2 &v)       { x-=v.x; y-=v.y; return *this; }
  Vector2  operator *  (T s) const              { return Vector2(x*s, y*s); }
  Vector2& operator *= (float s)                { x*=s; y*=s; return *this; }
  Vector2  operator /  (float s) const          { assert(s); return (*this)* (1/s); }
  Vector2& operator /= (float s)                { assert(s); return (*this)*=(1/s); }

  /// Index operator
  const T& operator [] (size_t index) const { return index == 0 ? x : y; }
        T& operator [] (size_t index)       { return index == 0 ? x : y; }

  // coordinates
  T x, y;
};

/**
 * types
 **/
typedef Vector2<char>   Vector2c;
typedef Vector2<int>    Vector2i;
typedef Vector2<float>  Vector2f;
typedef Vector2<double> Vector2d;

/**
 * Vector3
 **/
template <class T>
struct Vector3
{
  // create a vector
  Vector3 (T x_=0, T y_=0, T z_=0) : x(x_), y(y_), z(z_) {}

  // set coordinates
  void set (T x_, T y_, T z_) { x=x_; y=y_; z=z_; }

  // norm
  Vector3 normalize  () const { return (*this) / norm();    }
  double norm        () const { return sqrt(normSquared()); }
  T      normSquared () const { return x*x+y*y+z*z;			}

  // boolean operators
  bool operator == (const Vector3& v) const { return x==v.x && y==v.y && z==v.z; }
  bool operator != (const Vector3& v) const { return x!=v.x || y!=v.y || z!=v.z; }
  bool operator <  (const Vector3& v) const { return x < v.x || (x == v.x && (y < v.y || (y == v.y && z < v.z))); }

  // operators
  Vector3  operator +  (const Vector3 &v) const { return Vector3(x+v.x, y+v.y, z+v.z); }
  Vector3& operator += (const Vector3 &v)       { x+=v.x; y+=v.y; z+=v.z; return *this; }
  Vector3  operator -  () const                 { return Vector3(-x, -y, -z); }
  Vector3  operator -  (const Vector3 &v) const { return Vector3(x-v.x, y-v.y, z-v.z); }
  Vector3& operator -= (const Vector3 &v)       { x-=v.x; y-=v.y; z-=v.z; return *this; }
  Vector3  operator *  (T s) const              { return Vector3(x*s, y*s, z*s); }
  Vector3& operator *= (float s)                { x*=s; y*=s; z*=s; return *this; }
  Vector3  operator /  (float s) const          { assert(s); return (*this)* (1/s); }
  Vector3& operator /= (float s)                { assert(s); return (*this)*=(1/s); }

  // coordinates
  T x, y, z;
};

/**
 * Vector4
 **/
template <class T>
struct Vector4
{
  // create a vector
  Vector4 (T x_=0, T y_=0, T z_=0, T w_=0) : x(x_), y(y_), z(z_), w(w_) {}

  // set coordinates
  void set (T x_, T y_, T z_, T w_) { x=x_; y=y_; z=z_; w=w_; }

  // norm
  Vector4 normalize  () const { return (*this) / norm();    }
  double norm        () const { return sqrt(normSquared()); }
  T      normSquared () const { return x*x+y*y+z*z+w*w;		}

  // boolean operators
  bool operator == (const Vector4& v) const { return x==v.x && y==v.y && z==v.z && w==v.w; }
  bool operator != (const Vector4& v) const { return x!=v.x || y!=v.y || z!=v.z || w!=v.w; }
  bool operator <  (const Vector4& v) const { return x < v.x || (x == v.x && (y < v.y || (y == v.y && (z < v.z || (z == v.z && w < v.w))))); }

  // operators
  Vector4  operator +  (const Vector4 &v) const { return Vector4(x+v.x, y+v.y, z+v.z, w+v.w); }
  Vector4& operator += (const Vector4 &v)       { x+=v.x; y+=v.y; z+=v.z; w+=v.w; return *this; }
  Vector4  operator -  () const                 { return Vector4(-x, -y, -z, -w); }
  Vector4  operator -  (const Vector4 &v) const { return Vector4(x-v.x, y-v.y, z-v.z, w-v.w); }
  Vector4& operator -= (const Vector4 &v)       { x-=v.x; y-=v.y; z-=v.z, w-=v.w; return *this; }
  Vector4  operator *  (T s) const              { return Vector4(x*s, y*s, z*s, w*s); }
  Vector4& operator *= (float s)                { x*=s; y*=s; z*=s; w*=s; return *this; }
  Vector4  operator /  (float s) const          { assert(s); return (*this)* (1/s); }
  Vector4& operator /= (float s)                { assert(s); return (*this)*=(1/s); }

  // coordinates
  T x, y, z, w;
};

// dot product
template <class T> inline
T Dot (const Vector3<T>& l, const Vector3<T>& r)
{
  return l.x*r.x + l.y*r.y + l.z*r.z;
}

// cross product
template <class T> inline
Vector3<T> Cross (const Vector3<T>& l, const Vector3<T>& r)
{
  return Vector3<T>(
    l.y*r.z - l.z*r.y,
    l.z*r.x - l.x*r.z,
    l.x*r.y - l.y*r.x );
}

// rotate vector
template <class T> inline
Vector3<T> Rotate(const Vector3<T>& v, const Vector3<T>& axis, double theta)
{
	// Rotate vector counterclockwise around axis (looking at axis end-on) (rz(xaxis) = yaxis)
	// From Goldstein: v' = v cos t + a (v . a) [1 - cos t] - (v x a) sin t 
	const double cos_theta = cos(theta);
	const double dot = Dot(v, axis);
	Vector3<T> cross = Cross(v, axis);
	Vector3d result = v * cos_theta;
	result += axis * dot * (1.0 - cos_theta);
	result -= cross * sin(theta);
	return result;
}

template <class T> inline
Vector3<T> Rotate(const Vector3<T>& v, const Vector3<T>& origin, const Vector3<T>& axis, double theta)
{
	// Translate axis to origin
	Vector3<T> tv = v - origin;

	// Rotate point counterclockwise around axis through origin by radians ???
	Vector3<T> result = Rotate(tv, axis, theta);

	// Translate axis back from origin
	result += origin;
	return result;
}

// min/max functions
template <class T>
inline T Max  (T x, T y) { return x>=y ? x : y; }
template <class T>
inline T Min  (T x, T y) { return x<=y ? x : y; }

template <class T>
inline T Max  (T a, T b, T c)      { return Max(Max(a,b),c); }
template <class T>
inline T Max  (T a, T b, T c, T d) { return Max(Max(a,b),Max(c,d)); }
template <class T>
inline T Min  (T a, T b, T c)      { return Min(Min(a,b),c); }
template <class T>
inline T Min  (T a, T b, T c, T d) { return Min(Min(a,b),Min(c,d)); }

template <class T> inline
Vector3<T> Min (const Vector3<T> &l, const Vector3<T> &r)
{
	return Vector3<T>(Min(l.x,r.x), Min(l.y,r.y), Min(l.z,r.z));
}

template <class T> inline
Vector3<T> Max (const Vector3<T> &l, const Vector3<T> &r)
{
	return Vector3<T>(Max(l.x,r.x), Max(l.y,r.y), Max(l.z,r.z));
}

// round
inline int Round(double x) { return (int)(x + 0.5); }

// clamp
template <class T> inline
T Clamp(T v, T minv, T maxv)
{
	return (v < minv ? minv : (v > maxv ? maxv : v));
}

// Distance
template <class T> inline
double Distance(Vector2<T> &l, Vector2<T> &r)
{
	return (r - l).norm();
}

// Operator * 
template <class T> inline
Vector2<T> operator * (T s, Vector2<T> &l)
{
	return Vector2<T>(s * l.x, s * l.y);
}

// debug
template <class T> inline
std::ostream& operator << (std::ostream &out, const Vector3<T> &r)
{
  return out << '[' << r.x << ',' << r.y << ',' << r.z << ']';
}


/**
 * types
 **/
typedef Vector3<char>   Vector3c;
typedef Vector3<int>    Vector3i;
typedef Vector3<float>  Vector3f;
typedef Vector3<double> Vector3d;

typedef Vector4<char>   Vector4c;
typedef Vector4<int>    Vector4i;
typedef Vector4<float>  Vector4f;
typedef Vector4<double> Vector4d;

#endif
