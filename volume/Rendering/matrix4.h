#ifndef MATRIX4_H
#define MATRIX4_H

#include "Vector.h"
/**
 * 4D matrix class - a row major order
 */
template <class T>
struct Matrix4 
{
public:
	/// default constructor
	Matrix4(T m00 = T(0), T m01 = T(0), T m02 = T(0), T m03 = T(0), 
		    T m10 = T(0), T m11 = T(0), T m12 = T(0), T m13 = T(0),
		    T m20 = T(0), T m21 = T(0), T m22 = T(0), T m23 = T(0),
			T m30 = T(0), T m31 = T(0), T m32 = T(0), T m33 = T(0));

	/// copy constructor
	Matrix4(const Matrix4 &matrix);	

	/// assignment operator
	Matrix4& operator = (const Matrix4 &matrix);

	/// boolean operators
	bool operator == (const Matrix4 &matrix) const;
	bool operator != (const Matrix4 &matrix) const;

	/// row data access
	Vector4<T> row(const size_t iRow)    const;
	/// column data access
	Vector4<T> column(const size_t iCol) const;
	/// row data access
	const T* operator [] (const size_t iRow) const;
	/// row data access
	T*       operator [] (const size_t iRow);

	/// Matrix arithmetic operations
	Matrix4  operator +  (const Matrix4 &matrix) const;
	Matrix4& operator += (const Matrix4 &matrix);
	Matrix4  operator -  (const Matrix4 &matrix) const;
	Matrix4& operator -= (const Matrix4 &matrix);
	Matrix4  operator *  (const Matrix4 &matrix) const;
	Matrix4& operator *= (const Matrix4 &matrix);

	/// matrix and vector multiplication
	Vector3<T> operator * (const Vector3<T> &vec) const;
	Vector4<T> operator * (const Vector4<T> &vec) const;

	// matrix creation operations
	/// make zero matrix
	void zero();
	/// make identity matrix I
	void identity ();
	/// create a diagonal matrix
	void diagonal(T m00, T m11, T m22, T m33);
	/// set matrix elements
	void setElements(T m00, T m01, T m02, T m03, T m10, T m11, T m12, T m13, T m20, T m21, T m22, T m23, T m30, T m31, T m32, T m33);
	/// create a tensor product U*V^T
	void makeTensorProduct(const Vector4<T> &U, const Vector4<T> &V);

	// matrix math operations
	/// inverse matrix A^-1
	Matrix4 inverse () const;
	/// make adjoint matrix A*
	Matrix4 adjoint () const;
	/// transpose matrix A^T
	Matrix4 transpose () const;
	/// Return the upper left 3x3-submatrix in a 4x4 matrix
	Matrix4 rotationalPart () const;
	/// determinant of matrix
	T determinant () const;

	/// create transform matrix
	void fromTranslate(T dx, T dy, T dz);
	void fromScale(T sx, T sy, T sz);
	void fromXRotate(T angle);
	void fromYRotate(T angle);
	void fromZRotate(T angle);	
	/// create a rotation matrix (positive angle - counterclockwise, angle in radians)
	void fromAxisAngle(const Vector3<T> &axis, T angle);

	/// create a gluLookAt matrix, given the position, focus-point, and up-Vector of a camera
	void createLookAt(const Vector3<T> &eye, const Vector3<T> &focus, const Vector3<T> &up);

	/// create a glFrustum matrix
	void createFrustum(T left, T right, T top, T bottom, T near, T far);

	/// create a gluPerspective matrix
	void createPerspective(T fov, T aspect, T near, T far);

	/// create a gluOrtho matrix
	void createOrtho(T left, T right, T top, T bottom, T near, T far);

	/// transform point, vector, and normal
	Vector3<T> transformPoint(Vector3<T> &p) const;
	Vector3<T> transformVector(Vector3<T> &v) const;
	Vector3<T> transformNormal(Vector3<T> &n) const;

	/// matrix element in a row major order
    T m[4][4];
};


/**
 *	default constructor
 */
template <class T>
Matrix4<T>::Matrix4(T m00, T m01, T m02, T m03, T m10, T m11, T m12, T m13, T m20, T m21, T m22, T m23, T m30, T m31, T m32, T m33)
{
	m[0][0] = m00;
	m[0][1] = m01;
	m[0][2] = m02;
	m[0][3] = m03;
	m[1][0] = m10;
	m[1][1] = m11;
	m[1][2] = m12;
	m[1][3] = m13;
	m[2][0] = m20;
	m[2][1] = m21;
	m[2][2] = m22;
	m[2][3] = m23;
	m[3][0] = m30;
	m[3][1] = m31;
	m[3][2] = m32;
	m[3][3] = m33;
}

/**
 * copy constructor
 */
template <class T>
Matrix4<T>::Matrix4(const Matrix4<T> &matrix)
{
	m[0][0] = matrix.m[0][0];
	m[0][1] = matrix.m[0][1];
	m[0][2] = matrix.m[0][2];
	m[0][3] = matrix.m[0][3];
	m[1][0] = matrix.m[1][0];
	m[1][1] = matrix.m[1][1];
	m[1][2] = matrix.m[1][2];
	m[1][3] = matrix.m[1][3];
	m[2][0] = matrix.m[2][0];
	m[2][1] = matrix.m[2][1];
	m[2][2] = matrix.m[2][2];
	m[2][3] = matrix.m[2][3];
	m[3][0] = matrix.m[3][0];
	m[3][1] = matrix.m[3][1];
	m[3][2] = matrix.m[3][2];
	m[3][3] = matrix.m[3][3];
}

/**
 * assignment operator
 */
template <class T>
Matrix4<T>& Matrix4<T>::operator = (const Matrix4<T> &matrix)
{
	m[0][0] = matrix.m[0][0];
	m[0][1] = matrix.m[0][1];
	m[0][2] = matrix.m[0][2];
	m[0][3] = matrix.m[0][3];
	m[1][0] = matrix.m[1][0];
	m[1][1] = matrix.m[1][1];
	m[1][2] = matrix.m[1][2];
	m[1][3] = matrix.m[1][3];
	m[2][0] = matrix.m[2][0];
	m[2][1] = matrix.m[2][1];
	m[2][2] = matrix.m[2][2];
	m[2][3] = matrix.m[2][3];
	m[3][0] = matrix.m[3][0];
	m[3][1] = matrix.m[3][1];
	m[3][2] = matrix.m[3][2];
	m[3][3] = matrix.m[3][3];

	return (*this);
}

/**
 * boolean operators
 */
template <class T>
inline bool Matrix4<T>::operator == (const Matrix4 &matrix) const
{
	return (m[0][0] == matrix.m[0][0] && m[0][1] == matrix.m[0][1] && m[0][2] == matrix.m[0][2] && m[0][3] == matrix.m[0][3] &&
		    m[1][0] == matrix.m[1][0] && m[1][1] == matrix.m[1][1] && m[1][2] == matrix.m[1][2] && m[1][3] == matrix.m[1][3] &&
			m[2][0] == matrix.m[2][0] && m[2][1] == matrix.m[2][1] && m[2][2] == matrix.m[2][2] && m[2][3] == matrix.m[2][3] &&
			m[3][0] == matrix.m[3][0] && m[3][1] == matrix.m[3][1] && m[3][2] == matrix.m[3][2] && m[3][3] == matrix.m[3][3]);
}

template <class T>
inline bool Matrix4<T>::operator != (const Matrix4 &matrix) const
{
	return !(*this == matrix);
}

/**
 * row data access
 */
template <class T>
inline Vector4<T> Matrix4<T>::row(const size_t iRow) const
{
	vxAssert(iRow < 4, "out of range");
	return Vector4<T>(m[iRow][0], m[iRow][1], m[iRow][2], m[iRow][3]);
}

/** 
 * column data access
 */
template <class T>
inline Vector4<T> Matrix4<T>::column(const size_t iCol) const
{
	vxAssert(iCol < 4, "out of range");
	return Vector4<T>(m[0][iCol], m[1][iCol], m[2][iCol], m[3][iCol]);
}

/**
 * row data access
 */
template <class T> 
inline const T* Matrix4<T>::operator [](const size_t iRow) const
{
	vxAssert(iRow < 4, "out of range");
	return m[iRow];
}

/**
 * row data access
 */
template <class T> 
inline T* Matrix4<T>::operator [](const size_t iRow)
{
	vxAssert(iRow < 4, "out of range");
	return m[iRow];
}

/**
 *	Matrix arithmetic operations
 */
template <class T> 
inline Matrix4<T> Matrix4<T>::operator + (const Matrix4<T> &matrix) const
{
	return Matrix4(m[0][0] + matrix.m[0][0], m[0][1] + matrix.m[0][1], m[0][2] + matrix.m[0][2], m[0][3] + matrix.m[0][3],
				   m[1][0] + matrix.m[1][0], m[1][1] + matrix.m[1][1], m[1][2] + matrix.m[1][2], m[1][3] + matrix.m[1][3],
				   m[2][0] + matrix.m[2][0], m[2][1] + matrix.m[2][1], m[2][2] + matrix.m[2][2], m[2][3] + matrix.m[2][3],
				   m[3][0] + matrix.m[3][0], m[3][1] + matrix.m[3][1], m[3][2] + matrix.m[3][2], m[3][3] + matrix.m[3][3]);
}

template <class T> 
inline Matrix4<T>& Matrix4<T>::operator += (const Matrix4<T> &matrix)
{
	m[0][0] += matrix.m[0][0];
	m[0][1] += matrix.m[0][1];
	m[0][2] += matrix.m[0][2];
	m[0][3] += matrix.m[0][3];
	m[1][0] += matrix.m[1][0];
	m[1][1] += matrix.m[1][1];
	m[1][2] += matrix.m[1][2];
	m[1][3] += matrix.m[1][3];
	m[2][0] += matrix.m[2][0];
	m[2][1] += matrix.m[2][1];
	m[2][2] += matrix.m[2][2];
	m[2][3] += matrix.m[2][3];
	m[3][0] += matrix.m[3][0];
	m[3][1] += matrix.m[3][1];
	m[3][2] += matrix.m[3][2];
	m[3][3] += matrix.m[3][3];

	return (*this);
}

template <class T> 
inline Matrix4<T> Matrix4<T>::operator - (const Matrix4<T> &matrix) const
{
	return Matrix4(m[0][0] - matrix.m[0][0], m[0][1] - matrix.m[0][1], m[0][2] - matrix.m[0][2], m[0][3] - matrix.m[0][3],
				   m[1][0] - matrix.m[1][0], m[1][1] - matrix.m[1][1], m[1][2] - matrix.m[1][2], m[1][3] - matrix.m[1][3],
				   m[2][0] - matrix.m[2][0], m[2][1] - matrix.m[2][1], m[2][2] - matrix.m[2][2], m[2][3] - matrix.m[2][3],
				   m[3][0] - matrix.m[3][0], m[3][1] - matrix.m[3][1], m[3][2] - matrix.m[3][2], m[3][3] - matrix.m[3][3]);
}

template <class T> 
inline Matrix4<T>& Matrix4<T>::operator -= (const Matrix4<T> &matrix)
{
	m[0][0] -= matrix.m[0][0];
	m[0][1] -= matrix.m[0][1];
	m[0][2] -= matrix.m[0][2];
	m[0][3] -= matrix.m[0][3];
	m[1][0] -= matrix.m[1][0];
	m[1][1] -= matrix.m[1][1];
	m[1][2] -= matrix.m[1][2];
	m[1][3] -= matrix.m[1][3];
	m[2][0] -= matrix.m[2][0];
	m[2][1] -= matrix.m[2][1];
	m[2][2] -= matrix.m[2][2];
	m[2][3] -= matrix.m[2][3];
	m[3][0] -= matrix.m[3][0];
	m[3][1] -= matrix.m[3][1];
	m[3][2] -= matrix.m[3][2];
	m[3][3] -= matrix.m[3][3];

	return (*this);
}

template <class T> 
inline Matrix4<T> Matrix4<T>::operator * (const Matrix4<T> &matrix) const
{
	//Matrix4<T> product;
	//for(size_t i = 0; i < 4; ++i) {
	//	for(size_t j = 0; j < 4; ++j) {
	//		T value(0);
	//		for(size_t k = 0; k < 4; ++k)
	//			value += m[i][k] * matrix.m[k][j];
	//		product.m[i][j] = value;
	//	}
	//}
	//return product;

	// unroll loop
	Matrix4<T> product;
	product.m[0][0] = m[0][0] * matrix.m[0][0] + m[0][1] * matrix.m[1][0] + m[0][2] * matrix.m[2][0] + m[0][3] * matrix.m[3][0];
	product.m[0][1] = m[0][0] * matrix.m[0][1] + m[0][1] * matrix.m[1][1] + m[0][2] * matrix.m[2][1] + m[0][3] * matrix.m[3][1];
	product.m[0][2] = m[0][0] * matrix.m[0][2] + m[0][1] * matrix.m[1][2] + m[0][2] * matrix.m[2][2] + m[0][3] * matrix.m[3][2];
	product.m[0][3] = m[0][0] * matrix.m[0][3] + m[0][1] * matrix.m[1][3] + m[0][2] * matrix.m[2][3] + m[0][3] * matrix.m[3][3];
	product.m[1][0] = m[1][0] * matrix.m[0][0] + m[1][1] * matrix.m[1][0] + m[1][2] * matrix.m[2][0] + m[1][3] * matrix.m[3][0];
	product.m[1][1] = m[1][0] * matrix.m[0][1] + m[1][1] * matrix.m[1][1] + m[1][2] * matrix.m[2][1] + m[1][3] * matrix.m[3][1];
	product.m[1][2] = m[1][0] * matrix.m[0][2] + m[1][1] * matrix.m[1][2] + m[1][2] * matrix.m[2][2] + m[1][3] * matrix.m[3][2];
	product.m[1][3] = m[1][0] * matrix.m[0][3] + m[1][1] * matrix.m[1][3] + m[1][2] * matrix.m[2][3] + m[1][3] * matrix.m[3][3];
	product.m[2][0] = m[2][0] * matrix.m[0][0] + m[2][1] * matrix.m[1][0] + m[2][2] * matrix.m[2][0] + m[2][3] * matrix.m[3][0];
	product.m[2][1] = m[2][0] * matrix.m[0][1] + m[2][1] * matrix.m[1][1] + m[2][2] * matrix.m[2][1] + m[2][3] * matrix.m[3][1];
	product.m[2][2] = m[2][0] * matrix.m[0][2] + m[2][1] * matrix.m[1][2] + m[2][2] * matrix.m[2][2] + m[2][3] * matrix.m[3][2];
	product.m[2][3] = m[2][0] * matrix.m[0][3] + m[2][1] * matrix.m[1][3] + m[2][2] * matrix.m[2][3] + m[2][3] * matrix.m[3][3];
	product.m[3][0] = m[3][0] * matrix.m[0][0] + m[3][1] * matrix.m[1][0] + m[3][2] * matrix.m[2][0] + m[3][3] * matrix.m[3][0];
	product.m[3][1] = m[3][0] * matrix.m[0][1] + m[3][1] * matrix.m[1][1] + m[3][2] * matrix.m[2][1] + m[3][3] * matrix.m[3][1];
	product.m[3][2] = m[3][0] * matrix.m[0][2] + m[3][1] * matrix.m[1][2] + m[3][2] * matrix.m[2][2] + m[3][3] * matrix.m[3][2];
	product.m[3][3] = m[3][0] * matrix.m[0][3] + m[3][1] * matrix.m[1][3] + m[3][2] * matrix.m[2][3] + m[3][3] * matrix.m[3][3];
	return product;
}

template <class T> 
inline Matrix4<T>& Matrix4<T>::operator *= (const Matrix4<T> &matrix)
{
	//Matrix4<T> product;
	//for(size_t i = 0; i < 4; ++i) {
	//	for(size_t j = 0; j < 4; ++j) {
	//		T value(0);
	//		for(size_t k = 0; k < 4; ++k)
	//			value += m[i][k] * matrix.m[k][j];
	//		product.m[i][j] = value;
	//	}
	//}
	//(*this) = product;
	//return (*this);
	
	// unroll loop
	T m00 = m[0][0] * matrix.m[0][0] + m[0][1] * matrix.m[1][0] + m[0][2] * matrix.m[2][0] + m[0][3] * matrix.m[3][0];
	T m01 = m[0][0] * matrix.m[0][1] + m[0][1] * matrix.m[1][1] + m[0][2] * matrix.m[2][1] + m[0][3] * matrix.m[3][1];
	T m02 = m[0][0] * matrix.m[0][2] + m[0][1] * matrix.m[1][2] + m[0][2] * matrix.m[2][2] + m[0][3] * matrix.m[3][2];
	T m03 = m[0][0] * matrix.m[0][3] + m[0][1] * matrix.m[1][3] + m[0][2] * matrix.m[2][3] + m[0][3] * matrix.m[3][3];
	T m10 = m[1][0] * matrix.m[0][0] + m[1][1] * matrix.m[1][0] + m[1][2] * matrix.m[2][0] + m[1][3] * matrix.m[3][0];
	T m11 = m[1][0] * matrix.m[0][1] + m[1][1] * matrix.m[1][1] + m[1][2] * matrix.m[2][1] + m[1][3] * matrix.m[3][1];
	T m12 = m[1][0] * matrix.m[0][2] + m[1][1] * matrix.m[1][2] + m[1][2] * matrix.m[2][2] + m[1][3] * matrix.m[3][2];
	T m13 = m[1][0] * matrix.m[0][3] + m[1][1] * matrix.m[1][3] + m[1][2] * matrix.m[2][3] + m[1][3] * matrix.m[3][3];
	T m20 = m[2][0] * matrix.m[0][0] + m[2][1] * matrix.m[1][0] + m[2][2] * matrix.m[2][0] + m[2][3] * matrix.m[3][0];
	T m21 = m[2][0] * matrix.m[0][1] + m[2][1] * matrix.m[1][1] + m[2][2] * matrix.m[2][1] + m[2][3] * matrix.m[3][1];
	T m22 = m[2][0] * matrix.m[0][2] + m[2][1] * matrix.m[1][2] + m[2][2] * matrix.m[2][2] + m[2][3] * matrix.m[3][2];
	T m23 = m[2][0] * matrix.m[0][3] + m[2][1] * matrix.m[1][3] + m[2][2] * matrix.m[2][3] + m[2][3] * matrix.m[3][3];
	T m30 = m[3][0] * matrix.m[0][0] + m[3][1] * matrix.m[1][0] + m[3][2] * matrix.m[2][0] + m[3][3] * matrix.m[3][0];
	T m31 = m[3][0] * matrix.m[0][1] + m[3][1] * matrix.m[1][1] + m[3][2] * matrix.m[2][1] + m[3][3] * matrix.m[3][1];
	T m32 = m[3][0] * matrix.m[0][2] + m[3][1] * matrix.m[1][2] + m[3][2] * matrix.m[2][2] + m[3][3] * matrix.m[3][2];
	T m33 = m[3][0] * matrix.m[0][3] + m[3][1] * matrix.m[1][3] + m[3][2] * matrix.m[2][3] + m[3][3] * matrix.m[3][3];

	m[0][0] = m00;
	m[0][1] = m01;
	m[0][2] = m02;
	m[0][3] = m03;
	m[1][0] = m10;
	m[1][1] = m11;
	m[1][2] = m12;
	m[1][3] = m13;
	m[2][0] = m20;
	m[2][1] = m21;
	m[2][2] = m22;
	m[2][3] = m23;
	m[3][0] = m30;
	m[3][1] = m31;
	m[3][2] = m32;
	m[3][3] = m33;

	return (*this);
}

/**
 * matrix and vector multiplication
 */
template <class T> 
inline Vector3<T> Matrix4<T>::operator * (const Vector3<T> &vec) const
{
	// [x, y, z, 1]
	double xp = m[0][0] * vec.x + m[0][1] * vec.y + m[0][2] * vec.z + m[0][3];
	double yp = m[1][0] * vec.x + m[1][1] * vec.y + m[1][2] * vec.z + m[1][3];
	double zp = m[2][0] * vec.x + m[2][1] * vec.y + m[2][2] * vec.z + m[2][3];
	double wp = m[3][0] * vec.x + m[3][1] * vec.y + m[3][2] * vec.z + m[3][3];

	assert(wp != 0);
	return (wp == T(1)) ? Vector3<T>(xp, yp, zp) : Vector3<T>(xp, yp, zp) / wp;
}

template <class T> 
inline Vector4<T> Matrix4<T>::operator * (const Vector4<T> &vec) const
{
	return Vector4<T>(m[0][0] * vec[0] + m[0][1] * vec[1] + m[0][2] * vec[2] + m[0][3] * vec[3], 
		m[1][0] * vec[0] + m[1][1] * vec[1] + m[1][2] * vec[2] + m[1][3] * vec[3],
		m[2][0] * vec[0] + m[2][1] * vec[1] + m[2][2] * vec[2] + m[2][3] * vec[3],
		m[3][0] * vec[0] + m[3][1] * vec[1] + m[3][2] * vec[2] + m[3][3] * vec[3]);
}

/**
 * make zero matrix
 */
template <class T> 
inline void Matrix4<T>::zero()
{
	m[0][0] = m[0][1] = m[0][2] = m[0][3] = T(0);
	m[1][0] = m[1][1] = m[1][2] = m[1][3] = T(0);
	m[2][0] = m[2][1] = m[2][2] = m[2][3] = T(0);
	m[3][0] = m[3][1] = m[3][2] = m[3][3] = T(0);
}

/**
 * make identity matrix I
 */
template <class T> 
inline void Matrix4<T>::identity()
{
	m[0][0] = m[1][1] = m[2][2] = m[3][3] = T(1);
	m[0][1] = m[0][2] = m[0][3] = m[1][0] = T(0);
	m[1][2] = m[1][3] = m[2][0] = m[2][1] = T(0);
	m[2][3] = m[3][0] = m[3][1] = m[3][2] = T(0);
}


/**
 * create a diagonal matrix
 */
template <class T>
inline void Matrix4<T>::diagonal(T m00, T m11, T m22, T m33)
{
	m[0][0] = m00;
	m[1][1] = m11;
	m[2][2] = m22;
	m[3][3] = m33;
	m[0][1] = m[0][2] = m[0][3] = m[1][0] = T(0);
	m[1][2] = m[1][3] = m[2][0] = m[2][1] = T(0);
	m[2][3] = m[3][0] = m[3][1] = m[3][2] = T(0);
}

/**
 * set matrix elements
 */
template <class T>
inline void Matrix4<T>::setElements(T m00, T m01, T m02, T m03, T m10, T m11, T m12, T m13, T m20, T m21, T m22, T m23, T m30, T m31, T m32, T m33)
{
	m[0][0] = m00;
	m[0][1] = m01;
	m[0][2] = m02;
	m[0][3] = m03;
	m[1][0] = m10;
	m[1][1] = m11;
	m[1][2] = m12;
	m[1][3] = m13;
	m[2][0] = m20;
	m[2][1] = m21;
	m[2][2] = m22;
	m[2][3] = m23;
	m[3][0] = m30;
	m[3][1] = m31;
	m[3][2] = m32;
	m[3][3] = m33;
}

/**
 * create a tensor product U*V^T
 */
template <class T> 
inline void Matrix4<T>::makeTensorProduct(const Vector4<T> &U, const Vector4<T> &V)
{
	m[0][0] = U[0] * V[0];
	m[0][1] = U[0] * V[1];
	m[0][2] = U[0] * V[2];
	m[0][3] = U[0] * V[3];
	m[1][0] = U[1] * V[0];
	m[1][1] = U[1] * V[1];
	m[1][2] = U[1] * V[2];
	m[1][3] = U[1] * V[3];
	m[2][0] = U[2] * V[0];
	m[2][1] = U[2] * V[1];
	m[2][2] = U[2] * V[2];
	m[2][3] = U[2] * V[3];
	m[3][0] = U[3] * V[0];
	m[3][1] = U[3] * V[1];
	m[3][2] = U[3] * V[2];
	m[3][3] = U[3] * V[3];
}

/**
 * inverse matrix A^-1
 */
template <class T>
inline Matrix4<T> Matrix4<T>::inverse() const
{
	T fA0 = m[0][0]*m[1][1] - m[0][1]*m[1][0];
	T fA1 = m[0][0]*m[1][2] - m[0][2]*m[1][0];
	T fA2 = m[0][0]*m[1][3] - m[0][3]*m[1][0];
	T fA3 = m[0][1]*m[1][2] - m[0][2]*m[1][1];
	T fA4 = m[0][1]*m[1][3] - m[0][3]*m[1][1];
	T fA5 = m[0][2]*m[1][3] - m[0][3]*m[1][2];
	T fB0 = m[2][0]*m[3][1] - m[2][1]*m[3][0];
	T fB1 = m[2][0]*m[3][2] - m[2][2]*m[3][0];
	T fB2 = m[2][0]*m[3][3] - m[2][3]*m[3][0];
	T fB3 = m[2][1]*m[3][2] - m[2][2]*m[3][1];
	T fB4 = m[2][1]*m[3][3] - m[2][3]*m[3][1];
	T fB5 = m[2][2]*m[3][3] - m[2][3]*m[3][2];

	T fDet = fA0 * fB5 - fA1 * fB4 + fA2 * fB3 + fA3 * fB2 - fA4 * fB1 + fA5 * fB0;
	if(abs(fDet) <= Math<T>::EPSILON)
		return Matrix4<T>();

	Matrix4<T> invMatrix;
	invMatrix.m[0][0] = + m[1][1]*fB5 - m[1][2]*fB4 + m[1][3]*fB3;
	invMatrix.m[1][0] = - m[1][0]*fB5 + m[1][2]*fB2 - m[1][3]*fB1;
	invMatrix.m[2][0] = + m[1][0]*fB4 - m[1][1]*fB2 + m[1][3]*fB0;
	invMatrix.m[3][0] = - m[1][0]*fB3 + m[1][1]*fB1 - m[1][2]*fB0;
	invMatrix.m[0][1] = - m[0][1]*fB5 + m[0][2]*fB4 - m[0][3]*fB3;
	invMatrix.m[1][1] = + m[0][0]*fB5 - m[0][2]*fB2 + m[0][3]*fB1;
	invMatrix.m[2][1] = - m[0][0]*fB4 + m[0][1]*fB2 - m[0][3]*fB0;
	invMatrix.m[3][1] = + m[0][0]*fB3 - m[0][1]*fB1 + m[0][2]*fB0;
	invMatrix.m[0][2] = + m[3][1]*fA5 - m[3][2]*fA4 + m[3][3]*fA3;
	invMatrix.m[1][2] = - m[3][0]*fA5 + m[3][2]*fA2 - m[3][3]*fA1;
	invMatrix.m[2][2] = + m[3][0]*fA4 - m[3][1]*fA2 + m[3][3]*fA0;
	invMatrix.m[3][2] = - m[3][0]*fA3 + m[3][1]*fA1 - m[3][2]*fA0;
	invMatrix.m[0][3] = - m[2][1]*fA5 + m[2][2]*fA4 - m[2][3]*fA3;
	invMatrix.m[1][3] = + m[2][0]*fA5 - m[2][2]*fA2 + m[2][3]*fA1;
	invMatrix.m[2][3] = - m[2][0]*fA4 + m[2][1]*fA2 - m[2][3]*fA0;
	invMatrix.m[3][3] = + m[2][0]*fA3 - m[2][1]*fA1 + m[2][2]*fA0;

	T fInvDet = T(1) / fDet;
	for (size_t iRow = 0; iRow < 4; ++iRow) {
		for (size_t iCol = 0; iCol < 4; ++iCol)
			invMatrix.m[iRow][iCol] *= fInvDet;
	}

	return invMatrix;
}

/**
 * make adjoint matrix A*
 */
template <class T>
inline Matrix4<T> Matrix4<T>::adjoint() const
{
	T fA0 = m[0][0]*m[1][1] - m[0][1]*m[1][0];
	T fA1 = m[0][0]*m[1][2] - m[0][2]*m[1][0];
	T fA2 = m[0][0]*m[1][3] - m[0][3]*m[1][0];
	T fA3 = m[0][1]*m[1][2] - m[0][2]*m[1][1];
	T fA4 = m[0][1]*m[1][3] - m[0][3]*m[1][1];
	T fA5 = m[0][2]*m[1][3] - m[0][3]*m[1][2];
	T fB0 = m[2][0]*m[3][1] - m[2][1]*m[3][0];
	T fB1 = m[2][0]*m[3][2] - m[2][2]*m[3][0];
	T fB2 = m[2][0]*m[3][3] - m[2][3]*m[3][0];
	T fB3 = m[2][1]*m[3][2] - m[2][2]*m[3][1];
	T fB4 = m[2][1]*m[3][3] - m[2][3]*m[3][1];
	T fB5 = m[2][2]*m[3][3] - m[2][3]*m[3][2];

	Matrix4 kAdj;
	kAdj.m[0][0] = + m[1][1]*fB5 - m[1][2]*fB4 + m[1][3]*fB3;
	kAdj.m[1][0] = - m[1][0]*fB5 + m[1][2]*fB2 - m[1][3]*fB1;
	kAdj.m[2][0] = + m[1][0]*fB4 - m[1][1]*fB2 + m[1][3]*fB0;
	kAdj.m[3][0] = - m[1][0]*fB3 + m[1][1]*fB1 - m[1][2]*fB0;
	kAdj.m[0][1] = - m[0][1]*fB5 + m[0][2]*fB4 - m[0][3]*fB3;
	kAdj.m[1][1] = + m[0][0]*fB5 - m[0][2]*fB2 + m[0][3]*fB1;
	kAdj.m[2][1] = - m[0][0]*fB4 + m[0][1]*fB2 - m[0][3]*fB0;
	kAdj.m[3][1] = + m[0][0]*fB3 - m[0][1]*fB1 + m[0][2]*fB0;
	kAdj.m[0][2] = + m[3][1]*fA5 - m[3][2]*fA4 + m[3][3]*fA3;
	kAdj.m[1][2] = - m[3][0]*fA5 + m[3][2]*fA2 - m[3][3]*fA1;
	kAdj.m[2][2] = + m[3][0]*fA4 - m[3][1]*fA2 + m[3][3]*fA0;
	kAdj.m[3][2] = - m[3][0]*fA3 + m[3][1]*fA1 - m[3][2]*fA0;
	kAdj.m[0][3] = - m[2][1]*fA5 + m[2][2]*fA4 - m[2][3]*fA3;
	kAdj.m[1][3] = + m[2][0]*fA5 - m[2][2]*fA2 + m[2][3]*fA1;
	kAdj.m[2][3] = - m[2][0]*fA4 + m[2][1]*fA2 - m[2][3]*fA0;
	kAdj.m[3][3] = + m[2][0]*fA3 - m[2][1]*fA1 + m[2][2]*fA0;

	return kAdj;
}

/**
 * transpose matrix A^T
 */
template <class T>
inline Matrix4<T> Matrix4<T>::transpose() const
{
	return Matrix4(m[0][0], m[1][0], m[2][0], m[3][0],
				   m[0][1], m[1][1], m[2][1], m[3][1],
				   m[0][2], m[1][2], m[2][2], m[3][2],
				   m[0][3], m[1][3], m[2][3], m[3][3]);
}

/**
 * Return the upper left 3x3-submatrix in a 4x4 matrix
 */
template <class T>
inline Matrix4<T> Matrix4<T>::rotationalPart () const
{
	return Matrix4(m[0][0], m[0][1], m[0][2], 0,
				   m[1][0], m[1][1], m[1][2], 0,
				   m[2][0], m[2][1], m[2][2], 0,
				   0,       0,       0,       1);
}

/**
 * determinant of matrix
 */
template <class T>
inline T Matrix4<T>::determinant() const
{
	T fA0 = m[0][0]*m[1][1] - m[0][1]*m[1][0];
	T fA1 = m[0][0]*m[1][2] - m[0][2]*m[1][0];
	T fA2 = m[0][0]*m[1][3] - m[0][3]*m[1][0];
	T fA3 = m[0][1]*m[1][2] - m[0][2]*m[1][1];
	T fA4 = m[0][1]*m[1][3] - m[0][3]*m[1][1];
	T fA5 = m[0][2]*m[1][3] - m[0][3]*m[1][2];
	T fB0 = m[2][0]*m[3][1] - m[2][1]*m[3][0];
	T fB1 = m[2][0]*m[3][2] - m[2][2]*m[3][0];
	T fB2 = m[2][0]*m[3][3] - m[2][3]*m[3][0];
	T fB3 = m[2][1]*m[3][2] - m[2][2]*m[3][1];
	T fB4 = m[2][1]*m[3][3] - m[2][3]*m[3][1];
	T fB5 = m[2][2]*m[3][3] - m[2][3]*m[3][2];
	T fDet = fA0*fB5-fA1*fB4+fA2*fB3+fA3*fB2-fA4*fB1+fA5*fB0;
	return fDet;
}

/**
 * create transform matrix
 */
template <class T>
inline void Matrix4<T>::fromTranslate(T dx, T dy, T dz)
{
	(*this) = Matrix4(1, 0, 0, dx,
				      0, 1, 0, dy,
				      0, 0, 1, dz,
				      0, 0, 0, 1);

}

template <class T>
inline void Matrix4<T>::fromScale(T sx, T sy, T sz)
{
	(*this) = Matrix4(sx, 0,  0,  0,
				      0,  sy, 0,  0,
				      0,  0,  sz, 0,
				      0,  0,  0,  1);
}

template <class T>
inline void Matrix4<T>::fromXRotate(T angle)
{	
	//   RX =    1       0       0
	//           0     cos(t) -sin(t)
	//           0     sin(t)  cos(t)
	// where t > 0 indicates a counterclockwise rotation in the yz-plane
	T cost = cos(angle);
	T sint = sin(angle);
	(*this) = Matrix4(1, 0,    0,     0,
		              0, cost, -sint, 0,
		              0, sint, cost,  0,
				      0, 0,    0,     1);
}

template <class T>
inline void Matrix4<T>::fromYRotate(T angle)
{
	//   RY =  cos(t)    0     sin(t)
	//           0       1       0
	//        -sin(t)    0     cos(t)
	// where t > 0 indicates a counterclockwise rotation in the zx-plane
	T cost = cos(angle);
	T sint = sin(angle);
	(*this) = Matrix4(cost,  0, sint, 0,
		              0,     1, 0,    0,
		              -sint, 0, cost, 0,
				      0,     0, 0,    1);
}

template <class T>
inline void Matrix4<T>::fromZRotate(T angle)
{
	//   RZ =  cos(t) -sin(t)    0
	//         sin(t)  cos(t)    0
	//           0       0       1
	// where t > 0 indicates a counterclockwise rotation in the xy-plane.
	T cost = cos(angle);
	T sint = sin(angle);
	(*this) = Matrix4(cost, -sint, 0, 0,
		              sint, cost,  0, 0,
	                  0,    0,     1, 0,
				      0,    0,     0, 1);

}

/**
 *	Transform point, vector, and normal
 */
template <class T>
inline Vector3<T> Matrix4<T>::transformPoint(Vector3<T> &p) const
{
	// [x, y, z, 1]
	T xp = m[0][0] * p.x + m[0][1] * p.y + m[0][2] * p.z + m[0][3];
	T yp = m[1][0] * p.x + m[1][1] * p.y + m[1][2] * p.z + m[1][3];
	T zp = m[2][0] * p.x + m[2][1] * p.y + m[2][2] * p.z + m[2][3];
	T wp = m[3][0] * p.x + m[3][1] * p.y + m[3][2] * p.z + m[3][3];

	assert(wp != 0);
	return (wp == 1.) ? Vector3<T>(xp, yp, zp) : Vector3<T>(xp, yp, zp) / wp;
}

template <class T>
inline Vector3<T> Matrix4<T>::transformVector(Vector3<T> &v) const
{
	return Vector3<T>(m[0][0] * v.x + m[0][1] * v.y + m[0][2] * v.z,
					  m[1][0] * v.x + m[1][1] * v.y + m[1][2] * v.z,
					  m[2][0] * v.x + m[2][1] * v.y + m[2][2] * v.z);
}

template <class T>
inline Vector3<T> Matrix4<T>::transformNormal(Vector3<T> &n) const
{
	Matrix4<T> invMatrix = inverse();
	return Vector3<T>(invMatrix.m[0][0] * n.x + invMatrix.m[0][1] * n.y + invMatrix.m[0][2] * n.z,
					  invMatrix.m[1][0] * n.x + invMatrix.m[1][1] * n.y + invMatrix.m[1][2] * n.z,
					  invMatrix.m[2][0] * n.x + invMatrix.m[2][1] * n.y + invMatrix.m[2][2] * n.z);
}

/**
 *	Create rotation matrices (positive angle - counterclockwise).
 *  The angle must be in radians, not degrees.
 */
template <class T>
inline void Matrix4<T>::fromAxisAngle(const Vector3<T> &axis, T angle)
{
	T cosValue = cos(angle);
	T sinValue = sin(angle);
	T oneMinusCos = T(1) - cosValue;
	T X2 = axis[0] * axis[0];
	T Y2 = axis[1] * axis[1];
	T Z2 = axis[2] * axis[2];
	T XYM = axis[0] * axis[1] * oneMinusCos;
	T XZM = axis[0] * axis[2] * oneMinusCos;
	T YZM = axis[1] * axis[2] * oneMinusCos;
	T XSin = axis[0] * sinValue;
	T YSin = axis[1] * sinValue;
	T ZSin = axis[2] * sinValue;

	m[0][0] = X2 * oneMinusCos + cosValue;
	m[0][1] = XYM - ZSin;
	m[0][2] = XZM + YSin;
	m[0][3] = T(0);
	m[1][0] = XYM + ZSin;
	m[1][1] = Y2 * oneMinusCos + cosValue;
	m[1][2] = YZM - XSin;
	m[1][3] = T(0);
	m[2][0] = XZM - YSin;
	m[2][1] = YZM + XSin;
	m[2][2] = Z2 * oneMinusCos + cosValue;
	m[2][3] = T(0);
	m[3][0] = T(0);
	m[3][1] = T(0);
	m[3][2] = T(0);
	m[3][3] = T(1);
}

/**
 * create a gluLookAt matrix, given the position, focus-point, and up-Vector of a camera
 */
template <class T>
inline void Matrix4<T>::createLookAt(const Vector3<T> &eye, const Vector3<T> &focus, const Vector3<T> &up)
{
	Vector3<T> look   = (focus - eye).normalize();
	Vector3<T> strafe = cross(look, up.normalize()).normalize();
	Vector3<T> up2    = cross(strafe, look);

	setElements(strafe.x, strafe.y, strafe.z, -eye.x,
				up.x,     up.y,     up.z,     -eye.y,
				-look.x,  -look.y,  -look.z,  -eye.z,
				T(0),     T(0),     T(0),     T(1));
}

/**
 * create a glFrustum matrix
 */
template <class T>
inline void Matrix4<T>::createFrustum(T left, T right, T top, T bottom, T near, T far)
{
	setElements(T(2)*near/(right-left), T(0),					(right+left)/(right-left),  T(0),
				T(0),					T(2)*near/(top-bottom), (top+bottom)/(top-bottom),  T(0),
				T(0),                   T(0),					(near+far)/(near-far),		(T(2)*far*near)/(near-far),
				T(0),                   T(0),                   -T(1),                       T(0));
}

/**
 * create a gluPerspective matrix
 */
template <class T>
inline void Matrix4<T>::createPerspective(T fov, T aspect, T near, T far)
{
	T f = T(1) / tanf(fov / T(2));
	setElements(f / aspect,     T(0),            T(0),                      T(0),
				T(0),           f,               T(0),                      T(0),
				T(0),           T(0),			(near+far)/(near-far),		(T(2)*far*near)/(near-far),
				T(0),           T(0),            -T(1),                     T(0));
}

/**
 * create a gluOrtho matrix
 */
template <class T>
inline void Matrix4<T>::createOrtho(T left, T right, T top, T bottom, T near, T far)
{
	setElements(T(2)/(right-left),  T(0),				T(0),				-(right+left)/(right-left),
				T(0),				T(2)/(top-bottom),  T(0),				-(top+bottom)/(top-bottom),
				T(0),				T(0),				T(2)/(far-near),	-(far+near)/(far-near),
				T(0),				T(0),				T(0),               T(1));
}

/**
 * Vector4<T>^T * Matrix4<T>
 */
template <class T>
inline const Vector4<T> operator * (const Vector4<T> &v, const Matrix4<T> &m)
{
	return Vector3<T>(v[0] * m.m[0][0] + v[1] * m.m[1][0] + v[2] * m.m[2][0] + v[3] * m.m[3][0], 
					  v[0] * m.m[0][1] + v[1] * m.m[1][1] + v[2] * m.m[2][1] + v[3] * m.m[3][1],
					  v[0] * m.m[0][2] + v[1] * m.m[1][2] + v[2] * m.m[2][2] + v[3] * m.m[3][2],
					  v[0] * m.m[0][3] + v[1] * m.m[1][3] + v[2] * m.m[2][3] + v[3] * m.m[3][3]);
}

/**
 * stream operator
 */
template <class T>
inline std::ostream& operator << (std::ostream &out, const Matrix4<T> &m)
{
	out << '|' << m.m[0][0] << ", " << m.m[0][1] << ", " << m.m[0][2] << ", " << m.m[0][3] << "|\r\n";
	out << '|' << m.m[1][0] << ", " << m.m[1][1] << ", " << m.m[1][2] << ", " << m.m[1][3] << "|\r\n";
	out << '|' << m.m[2][0] << ", " << m.m[2][1] << ", " << m.m[2][2] << ", " << m.m[2][3] << "|\r\n";
	out << '|' << m.m[3][0] << ", " << m.m[3][1] << ", " << m.m[3][2] << ", " << m.m[3][3] << "|";
	return out;
}

/**
 * types
 */
typedef Matrix4<float>  Matrix4f;
typedef Matrix4<double> Matrix4d;

#endif // MATRIX4_H
