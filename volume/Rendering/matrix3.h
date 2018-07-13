#ifndef MATRIX3_H
#define MATRIX3_H

#include "Vector.h"

/**
 * 3D matrix class - a row major order
 */
template <class T>
struct Matrix3 
{
public:
	/// default constructor
	Matrix3(T m00 = T(0), T m01 = T(0), T m02 = T(0), 
		    T m10 = T(0), T m11 = T(0), T m12 = T(0), 
			T m20 = T(0), T m21 = T(0), T m22 = T(0));

	/// copy constructor
	Matrix3(const Matrix3 &matrix);	

	/// assignment operator
	Matrix3& operator = (const Matrix3 &matrix);

	/// boolean operators
	bool operator == (const Matrix3 &matrix) const;
	bool operator != (const Matrix3 &matrix) const;

	/// row data access
	Vector3<T> row(const size_t iRow)    const;
	/// column data access
	Vector3<T> column(const size_t iCol) const;
	/// row data access
	const T* operator [] (const size_t iRow) const;
	/// row data access
	T*       operator [] (const size_t iRow);

	/// arithmetic operations
	Matrix3 operator -() const;
	Matrix3 operator /(float f) const;


	/// Matrix arithmetic operations
	Matrix3  operator +  (const Matrix3 &matrix) const;
	Matrix3& operator += (const Matrix3 &matrix);
	Matrix3  operator -  (const Matrix3 &matrix) const;
	Matrix3& operator -= (const Matrix3 &matrix);
	Matrix3  operator *  (const Matrix3 &matrix) const;
	Matrix3& operator *= (const Matrix3 &matrix);

	/// matrix and vector multiplication
	Vector3<T> operator * (const Vector3<T> &vec) const;

	// matrix creation operations
	/// make zero matrix
	void zero();
	/// make identity matrix I
	void identity ();
	/// create a diagonal matrix
	void diagonal(T m00, T m11, T m22);
	/// set matrix elements
	void setElements(T m00, T m01, T m02, T m10, T m11, T m12, T m20, T m21, T m22);
	/// create a tensor product U*V^T
	void makeTensorProduct(const Vector3<T> &U, const Vector3<T> &V);

	// matrix math operations
	/// inverse matrix A^-1
	Matrix3 inverse () const;
	/// make adjoint matrix A*
	Matrix3 adjoint () const;
	/// transpose matrix A^T
	Matrix3 transpose () const;
	/// determinant of matrix
	T determinant () const;

	/// Gram-Schmidt orthonormalization
	void orthonormalize();

	/// eigenvalue and eigenvector decomposition
	void eigenDecomposition(Matrix3 &rot, Matrix3 &diag) const;

	// Singular value decomposition, M = L*S*R, where L and R are orthogonal
	// and S is a diagonal matrix whose diagonal entries are nonnegative.
	void singularValueDecomposition(Matrix3& rkL, Matrix3& rkS, Matrix3& rkR) const;
	void singularValueComposition(const Matrix3& rkL, const Matrix3& rkS, const Matrix3& rkR);

	// factor M = Q*D*U with orthogonal Q, diagonal D, upper triangular U
	void QDUDecomposition(Matrix3& rkQ, Matrix3& rkD, Matrix3& rkU) const;

	// rotation matrix
	/// create a rotation matrix (positive angle - counterclockwise, angle in radians)
	void fromAxisXAngle(T angle);
	void fromAxisYAngle(T angle);
	void fromAxisZAngle(T angle);
	void fromAxisAngle(const Vector3<T> &axis, T angle);
	/// recover the angle from the rotation matrix
	void toAxisAngle(Vector3<T> &axis, T &angle) const;

	/// rotation matrix and Euler angles transformation
	/// The matrix must be orthonormal.  The decomposition is yaw*pitch*roll
	/// where yaw is rotation about the Up vector, pitch is rotation about the
	/// Right axis, and roll is rotation about the Direction axis.
	bool toEulerAnglesXYZ(T &XAngle, T &YAngle, T &ZAngle) const;
	bool toEulerAnglesXZY(T &XAngle, T &ZAngle, T &YAngle) const;
	bool toEulerAnglesYXZ(T &YAngle, T &XAngle, T &ZAngle) const;
	bool toEulerAnglesYZX(T &YAngle, T &ZAngle, T &XAngle) const;
	bool toEulerAnglesZXY(T &ZAngle, T &XAngle, T &YAngle) const;
	bool toEulerAnglesZYX(T &ZAngle, T &YAngle, T &XAngle) const;
	void fromEulerAnglesXYZ(T XAngle, T YAngle, T ZAngle);
	void fromEulerAnglesXZY(T XAngle, T ZAngle, T YAngle);
	void fromEulerAnglesYXZ(T YAngle, T XAngle, T ZAngle);
	void fromEulerAnglesYZX(T YAngle, T ZAngle, T XAngle);
	void fromEulerAnglesZXY(T ZAngle, T XAngle, T YAngle);
	void fromEulerAnglesZYX(T ZAngle, T YAngle, T XAngle);

	// support for eigendecomposition
	void tridiagonalize(T afDiag[3], T afSubDiag[3]);
	bool QLAlgorithm(T afDiag[3], T afSubDiag[3]);

	// support for singular value decomposition
	static void bidiagonalize (Matrix3& rkA, Matrix3& rkL, Matrix3& rkR);
	static void GolubKahanStep (Matrix3& rkA, Matrix3& rkL, Matrix3& rkR);

	/// matrix element in a row major order
    T m[3][3];
};

template <class T>
Matrix3<T> Matrix3<T>::operator -() const{
	return Matrix3<T>(m[0][0], m[0][1], m[0][2], m[1][0], m[1][1], m[1][2], m[2][0], m[2][1], m[2][2]);
}

template <class T>
Matrix3<T> Matrix3<T>::operator /(float f) const{
	return Matrix3<T>(m[0][0]/f, m[0][1]/f, m[0][2]/f, m[1][0]/f, m[1][1]/f, m[1][2]/f, m[2][0]/f, m[2][1]/f, m[2][2]/f);
}
/**
 *	default constructor
 */
template <class T>
Matrix3<T>::Matrix3(T m00, T m01, T m02, T m10, T m11, T m12, T m20, T m21, T m22)
{
	m[0][0] = m00; 
	m[0][1] = m01; 
	m[0][2] = m02;
	m[1][0] = m10; 
	m[1][1] = m11; 
	m[1][2] = m12;
	m[2][0] = m20; 
	m[2][1] = m21; 
	m[2][2] = m22;
}

/**
 * copy constructor
 */
template <class T>
Matrix3<T>::Matrix3(const Matrix3<T> &matrix)
{
	m[0][0] = matrix.m[0][0]; 
	m[0][1] = matrix.m[0][1]; 
	m[0][2] = matrix.m[0][2];
	m[1][0] = matrix.m[1][0]; 
	m[1][1] = matrix.m[1][1]; 
	m[1][2] = matrix.m[1][2];
	m[2][0] = matrix.m[2][0]; 
	m[2][1] = matrix.m[2][1]; 
	m[2][2] = matrix.m[2][2];
}

/**
 * assignment operator
 */
template <class T>
Matrix3<T>& Matrix3<T>::operator = (const Matrix3<T> &matrix)
{
	m[0][0] = matrix.m[0][0]; 
	m[0][1] = matrix.m[0][1]; 
	m[0][2] = matrix.m[0][2];
	m[1][0] = matrix.m[1][0]; 
	m[1][1] = matrix.m[1][1]; 
	m[1][2] = matrix.m[1][2];
	m[2][0] = matrix.m[2][0]; 
	m[2][1] = matrix.m[2][1]; 
	m[2][2] = matrix.m[2][2];
	return (*this);
}

/**
 * boolean operators
 */
template <class T>
inline bool Matrix3<T>::operator == (const Matrix3 &matrix) const
{
	return (m[0][0] == matrix.m[0][0] && m[0][1] == matrix.m[0][1] && m[0][2] == matrix.m[0][2] &&
		    m[1][0] == matrix.m[1][0] && m[1][1] == matrix.m[1][1] && m[1][2] == matrix.m[1][2] &&
			m[2][0] == matrix.m[2][0] && m[2][1] == matrix.m[2][1] && m[2][2] == matrix.m[2][2]);
}

template <class T>
inline bool Matrix3<T>::operator != (const Matrix3 &matrix) const
{
	return !(*this == matrix);
}

/**
 * row data access
 */
template <class T>
inline Vector3<T> Matrix3<T>::row(const size_t iRow) const
{
	vxAssert(iRow < 3, "out of range");
	return Vector3<T>(m[iRow][0], m[iRow][1], m[iRow][2]);
}

/** 
 * column data access
 */
template <class T>
inline Vector3<T> Matrix3<T>::column(const size_t iCol) const
{
	vxAssert(iCol < 3, "out of range");
	return Vector3<T>(m[0][iCol], m[1][iCol], m[2][iCol]);
}

/**
 * row data access
 */
template <class T> 
inline const T* Matrix3<T>::operator [](const size_t iRow) const
{
	//vxAssert(iRow < 3, "out of range");
	assert(iRow < 3);
	return m[iRow];
}

/**
 * row data access
 */
template <class T> 
inline T* Matrix3<T>::operator [](const size_t iRow)
{
	//vxAssert(iRow < 3, "out of range");
	assert(iRow < 3);
	return m[iRow];
}

/**
 *	Matrix arithmetic operations
 */
template <class T> 
inline Matrix3<T> Matrix3<T>::operator + (const Matrix3<T> &matrix) const
{
	return Matrix3(m[0][0] + matrix.m[0][0], m[0][1] + matrix.m[0][1], m[0][2] + matrix.m[0][2],
				   m[1][0] + matrix.m[1][0], m[1][1] + matrix.m[1][1], m[1][2] + matrix.m[1][2],
				   m[2][0] + matrix.m[2][0], m[2][1] + matrix.m[2][1], m[2][2] + matrix.m[2][2]);
}

template <class T> 
inline Matrix3<T>& Matrix3<T>::operator += (const Matrix3<T> &matrix)
{
	m[0][0] += matrix.m[0][0];
	m[0][1] += matrix.m[0][1];
	m[0][2] += matrix.m[0][2];
	m[1][0] += matrix.m[1][0];
	m[1][1] += matrix.m[1][1];
	m[1][2] += matrix.m[1][2];
	m[2][0] += matrix.m[2][0];
	m[2][1] += matrix.m[2][1];
	m[2][2] += matrix.m[2][2];

	return (*this);
}

template <class T> 
inline Matrix3<T> Matrix3<T>::operator - (const Matrix3<T> &matrix) const
{
	return Matrix3(m[0][0] - matrix.m[0][0], m[0][1] - matrix.m[0][1], m[0][2] - matrix.m[0][2],
				   m[1][0] - matrix.m[1][0], m[1][1] - matrix.m[1][1], m[1][2] - matrix.m[1][2],
				   m[2][0] - matrix.m[2][0], m[2][1] - matrix.m[2][1], m[2][2] - matrix.m[2][2]);
}

template <class T> 
inline Matrix3<T>& Matrix3<T>::operator -= (const Matrix3<T> &matrix)
{
	m[0][0] -= matrix.m[0][0];
	m[0][1] -= matrix.m[0][1];
	m[0][2] -= matrix.m[0][2];
	m[1][0] -= matrix.m[1][0];
	m[1][1] -= matrix.m[1][1];
	m[1][2] -= matrix.m[1][2];
	m[2][0] -= matrix.m[2][0];
	m[2][1] -= matrix.m[2][1];
	m[2][2] -= matrix.m[2][2];

	return (*this);
}

template <class T> 
inline Matrix3<T> Matrix3<T>::operator * (const Matrix3<T> &matrix) const
{
	//Matrix3<T> product;
	//for(size_t i = 0; i < 3; ++i) {
	//	for(size_t j = 0; j < 3; ++j) {
	//		T value(0);
	//		for(size_t k = 0; k < 3; ++k)
	//			value += m[i][k] * matrix.m[k][j];
	//		product.m[i][j] = value;
	//	}
	//}
	//return product;

	// unroll loop
	Matrix3<T> product;
	product.m[0][0] = m[0][0] * matrix.m[0][0] + m[0][1] * matrix.m[1][0] + m[0][2] * matrix.m[2][0];
	product.m[0][1] = m[0][0] * matrix.m[0][1] + m[0][1] * matrix.m[1][1] + m[0][2] * matrix.m[2][1];
	product.m[0][2] = m[0][0] * matrix.m[0][2] + m[0][1] * matrix.m[1][2] + m[0][2] * matrix.m[2][2];
	product.m[1][0] = m[1][0] * matrix.m[0][0] + m[1][1] * matrix.m[1][0] + m[1][2] * matrix.m[2][0];
	product.m[1][1] = m[1][0] * matrix.m[0][1] + m[1][1] * matrix.m[1][1] + m[1][2] * matrix.m[2][1];
	product.m[1][2] = m[1][0] * matrix.m[0][2] + m[1][1] * matrix.m[1][2] + m[1][2] * matrix.m[2][2];
	product.m[2][0] = m[2][0] * matrix.m[0][0] + m[2][1] * matrix.m[1][0] + m[2][2] * matrix.m[2][0];
	product.m[2][1] = m[2][0] * matrix.m[0][1] + m[2][1] * matrix.m[1][1] + m[2][2] * matrix.m[2][1];
	product.m[2][2] = m[2][0] * matrix.m[0][2] + m[2][1] * matrix.m[1][2] + m[2][2] * matrix.m[2][2];
	return product;
}

template <class T> 
inline Matrix3<T>& Matrix3<T>::operator *= (const Matrix3<T> &matrix)
{
	//Matrix3<T> product;
	//for(size_t i = 0; i < 3; ++i) {
	//	for(size_t j = 0; j < 3; ++j) {
	//		T value(0);
	//		for(size_t k = 0; k < 3; ++k)
	//			value += m[i][k] * matrix.m[k][j];
	//		product.m[i][j] = value;
	//	}
	//}
	//(*this) = product;
	//return (*this);
	
	// unroll loop
	T m00 = m[0][0] * matrix.m[0][0] + m[0][1] * matrix.m[1][0] + m[0][2] * matrix.m[2][0];
	T m01 = m[0][0] * matrix.m[0][1] + m[0][1] * matrix.m[1][1] + m[0][2] * matrix.m[2][1];
	T m02 = m[0][0] * matrix.m[0][2] + m[0][1] * matrix.m[1][2] + m[0][2] * matrix.m[2][2];
	T m10 = m[1][0] * matrix.m[0][0] + m[1][1] * matrix.m[1][0] + m[1][2] * matrix.m[2][0];
	T m11 = m[1][0] * matrix.m[0][1] + m[1][1] * matrix.m[1][1] + m[1][2] * matrix.m[2][1];
	T m12 = m[1][0] * matrix.m[0][2] + m[1][1] * matrix.m[1][2] + m[1][2] * matrix.m[2][2];
	T m20 = m[2][0] * matrix.m[0][0] + m[2][1] * matrix.m[1][0] + m[2][2] * matrix.m[2][0];
	T m21 = m[2][0] * matrix.m[0][1] + m[2][1] * matrix.m[1][1] + m[2][2] * matrix.m[2][1];
	T m22 = m[2][0] * matrix.m[0][2] + m[2][1] * matrix.m[1][2] + m[2][2] * matrix.m[2][2];

	m[0][0] = m00; 
	m[0][1] = m01; 
	m[0][2] = m02;
	m[1][0] = m10; 
	m[1][1] = m11; 
	m[1][2] = m12;
	m[2][0] = m20; 
	m[2][1] = m21; 
	m[2][2] = m22;

	return (*this);
}

/**
 * matrix and vector multiplication
 */
template <class T> 
inline Vector3<T> Matrix3<T>::operator * (const Vector3<T> &vec) const
{
	return Vector3<T>(m[0][0] * vec[0] + m[0][1] * vec[1] + m[0][2] * vec[2], 
					  m[1][0] * vec[0] + m[1][1] * vec[1] + m[1][2] * vec[2],
					  m[2][0] * vec[0] + m[2][1] * vec[1] + m[2][2] * vec[2]);
}

/**
 * make zero matrix
 */
template <class T> 
inline void Matrix3<T>::zero()
{
	m[0][0] = m[0][1] = m[0][2] = T(0);
	m[1][0] = m[1][1] = m[1][2] = T(0);
	m[2][0] = m[2][1] = m[2][2] = T(0);
}

/**
 * make identity matrix I
 */
template <class T> 
inline void Matrix3<T>::identity()
{
	m[0][0] = m[1][1] = m[2][2] = T(1);
	m[0][1] = m[0][2] = m[1][0] = T(0);
	m[1][2] = m[2][0] = m[2][1] = T(0);
}


/**
 * create a diagonal matrix
 */
template <class T>
inline void Matrix3<T>::diagonal(T m00, T m11, T m22)
{
	m[0][0] = m00;
	m[1][1] = m11;
	m[2][2] = m22;
	m[0][1] = m[0][2] = m[1][0] = T(0);
	m[1][2] = m[2][0] = m[2][1] = T(0);
}

/**
 * set matrix elements
 */
template <class T>
inline void Matrix3<T>::setElements(T m00, T m01, T m02, T m10, T m11, T m12, T m20, T m21, T m22)
{
	m[0][0] = m00;
	m[0][1] = m01;
	m[0][2] = m02;
	m[1][0] = m10;
	m[1][1] = m11;
	m[1][2] = m12;
	m[2][0] = m20;
	m[2][1] = m21;
	m[2][2] = m22;
}


/**
 * create a tensor product U*V^T
 */
template <class T> 
inline void Matrix3<T>::makeTensorProduct(const Vector3<T> &U, const Vector3<T> &V)
{
	m[0][0] = U[0] * V[0];
	m[0][1] = U[0] * V[1];
	m[0][2] = U[0] * V[2];
	m[1][0] = U[1] * V[0];
	m[1][1] = U[1] * V[1];
	m[1][2] = U[1] * V[2];
	m[2][0] = U[2] * V[0];
	m[2][1] = U[2] * V[1];
	m[2][2] = U[2] * V[2];
}

/**
 * inverse matrix A^-1
 */
template <class T>
inline Matrix3<T> Matrix3<T>::inverse() const
{
	Matrix3 invMatrix;
	T det = determinant();
	if(abs(det) > Math<T>::EPSILON) {
		T invDet = T(1) / det;
		invMatrix.m[0][0] = (m[1][1] * m[2][2] - m[1][2] * m[2][1]) * invDet;
		invMatrix.m[0][1] = (m[2][1] * m[0][2] - m[0][1] * m[2][2]) * invDet;
		invMatrix.m[0][2] = (m[0][1] * m[1][2] - m[1][1] * m[0][2]) * invDet;
		invMatrix.m[1][0] = (m[1][2] * m[2][0] - m[1][0] * m[2][2]) * invDet;
		invMatrix.m[1][1] = (m[0][0] * m[2][2] - m[2][0] * m[0][2]) * invDet;
		invMatrix.m[1][2] = (m[1][0] * m[0][2] - m[0][0] * m[1][2]) * invDet;
		invMatrix.m[2][0] = (m[1][0] * m[2][1] - m[2][0] * m[1][1]) * invDet;
		invMatrix.m[2][1] = (m[2][0] * m[0][1] - m[0][0] * m[2][1]) * invDet;
		invMatrix.m[2][2] = (m[0][0] * m[1][1] - m[0][1] * m[1][0]) * invDet;
	}
	return invMatrix;
}

/**
 * make adjoint matrix A*
 */
template <class T>
inline Matrix3<T> Matrix3<T>::adjoint() const
{
	Matrix3 kAdjoint;
	kAdjoint.m[0][0] = m[1][1] * m[2][2] - m[1][2] * m[2][1];
	kAdjoint.m[0][1] = m[2][1] * m[0][2] - m[0][1] * m[2][2];
	kAdjoint.m[0][2] = m[0][1] * m[1][2] - m[1][1] * m[0][2];
	kAdjoint.m[1][0] = m[1][2] * m[2][0] - m[1][0] * m[2][2];
	kAdjoint.m[1][1] = m[0][0] * m[2][2] - m[2][0] * m[0][2];
	kAdjoint.m[1][2] = m[1][0] * m[0][2] - m[0][0] * m[1][2];
	kAdjoint.m[2][0] = m[1][0] * m[2][1] - m[2][0] * m[1][1];
	kAdjoint.m[2][1] = m[2][0] * m[0][1] - m[0][0] * m[2][1];
	kAdjoint.m[2][2] = m[0][0] * m[1][1] - m[0][1] * m[1][0];
	return kAdjoint;
}

/**
 * transpose matrix A^T
 */
template <class T>
inline Matrix3<T> Matrix3<T>::transpose() const
{
	Matrix3 kTranspose;
	kTranspose.m[0][0] = m[0][0];
	kTranspose.m[0][1] = m[1][0];
	kTranspose.m[0][2] = m[2][0];
	kTranspose.m[1][0] = m[0][1];
	kTranspose.m[1][1] = m[1][1];
	kTranspose.m[1][2] = m[2][1];
	kTranspose.m[2][0] = m[0][2];
	kTranspose.m[2][1] = m[1][2];
	kTranspose.m[2][2] = m[2][2];
	return kTranspose;
}

/**
 * determinant of matrix
 */
template <class T>
inline T Matrix3<T>::determinant() const
{
	return (m[0][0] * (m[1][1] * m[2][2] - m[2][1] * m[1][2]) - 
		    m[0][1] * (m[1][0] * m[2][2] - m[2][0] * m[1][2]) + 
		    m[0][2] * (m[1][0] * m[2][1] - m[2][0] * m[1][1]));
}

/**
 * Gram-Schmidt orthonormalization
 */
template <class T>
inline void Matrix3<T>::orthonormalize()
{
	// Algorithm uses Gram-Schmidt orthogonalization.  If 'this' matrix is
	// M = [m0|m1|m2], then orthonormal output matrix is Q = [q0|q1|q2],
	//
	//   q0 = m0/|m0|
	//   q1 = (m1-(q0*m1)q0)/|m1-(q0*m1)q0|
	//   q2 = (m2-(q0*m2)q0-(q1*m2)q1)/|m2-(q0*m2)q0-(q1*m2)q1|
	//
	// where |V| indicates length of vector V and A*B indicates dot
	// product of vectors A and B.

	// compute q0
	T fInvLength = T(1) / sqrt(m[0][0] * m[0][0] + m[1][0] * m[1][0] + m[2][0] * m[2][0]);

	m[0][0] *= fInvLength;
	m[1][0] *= fInvLength;
	m[2][0] *= fInvLength;

	// compute q1
	T dot0 = m[0][0] * m[0][1] + m[1][0] * m[1][1] + m[2][0] * m[2][1];

	m[0][1] -= dot0 * m[0][0];
	m[1][1] -= dot0 * m[1][0];
	m[2][1] -= dot0 * m[2][0];

	fInvLength = T(1) / sqrt(m[0][1] * m[0][1] + m[1][1] * m[1][1] + m[2][1] * m[2][1]);

	m[0][1] *= fInvLength;
	m[1][1] *= fInvLength;
	m[2][1] *= fInvLength;

	// compute q2
	T dot1 = m[0][1] * m[0][2] + m[1][1] * m[1][2] + m[2][1] * m[2][2];

	dot0 = m[0][0] * m[0][2] + m[1][0] * m[1][2] + m[2][0] * m[2][2];

	m[0][2] -= dot0 * m[0][0] + dot1 * m[0][1];
	m[1][2] -= dot0 * m[1][0] + dot1 * m[1][1];
	m[2][2] -= dot0 * m[2][0] + dot1 * m[2][1];

	fInvLength = T(1) / sqrt(m[0][2] * m[0][2] + m[1][2] * m[1][2] + m[2][2] * m[2][2]);

	m[0][2] *= fInvLength;
	m[1][2] *= fInvLength;
	m[2][2] *= fInvLength;
}

/**
 * eigenvalue and eigenvector decomposition
 * The matrix must be symmetric.  Factor M = R * D * R^T where
 * R = [u0|u1|u2] is a rotation matrix with columns u0, u1, and u2 and
 * D = diag(d0,d1,d2) is a diagonal matrix whose diagonal entries are d0,
 * d1, and d2.  The eigenvector u[i] corresponds to eigenvector d[i].
 * The eigenvalues are ordered as d0 <= d1 <= d2.
 */
template <class T>
inline void Matrix3<T>::eigenDecomposition(Matrix3 &rot, Matrix3 &diag) const
{
	// Factor M = R*D*R^T.  The columns of R are the eigenvectors.  The
	// diagonal entries of D are the corresponding eigenvalues.
	T afDiag[3], afSubDiag[3];
	rot = *this;
	rot.tridiagonalize(afDiag,afSubDiag);
	rot.QLAlgorithm(afDiag,afSubDiag);

	// The Householder transformation is a reflection.  Make the eigenvectors
	// a right--handed system by changing sign on the last column.
	rot[2][0] = -rot[2][0];
	rot[2][1] = -rot[2][1];
	rot[2][2] = -rot[2][2];

	// (insertion) sort eigenvalues in increasing order, d0 <= d1 <= d2
	T fSave;
	if(afDiag[1] < afDiag[0]) {
		// swap d0 and d1
		fSave = afDiag[0];
		afDiag[0] = afDiag[1];
		afDiag[1] = fSave;

		// swap V0 and V1
		for(size_t i = 0; i < 3; ++i) {
			fSave = rot[i][0];
			rot[i][0] = rot[i][1];
			rot[i][1] = fSave;
		}
	}

	if(afDiag[2] < afDiag[1]) {
		// swap d1 and d2
		fSave = afDiag[1];
		afDiag[1] = afDiag[2];
		afDiag[2] = fSave;

		// swap V1 and V2
		for(size_t i = 0; i < 3; ++i) {
			fSave = rot[i][1];
			rot[i][1] = rot[i][2];
			rot[i][2] = fSave;
		}
	}

	if(afDiag[1] < afDiag[0]) {
		// swap d0 and d1
		fSave = afDiag[0];
		afDiag[0] = afDiag[1];
		afDiag[1] = fSave;

		// swap V0 and V1
		for(size_t i = 0; i < 3; ++i) {
			fSave = rot[i][0];
			rot[i][0] = rot[i][1];
			rot[i][1] = fSave;
		}
	}

	diag.diagonal(afDiag[0], afDiag[1], afDiag[2]);
}

/**
 *	Create a rotation matrix around the x-axis
 */
template <class T>
inline void Matrix3<T>::fromAxisXAngle(T angle)
{
	T cosValue = cos(angle);
	T sinValue = sin(angle);
	m[0][0] = T(1);
	m[0][1] = T(0);
	m[0][2] = T(0);
	m[1][0] = T(0);
	m[1][1] = cosValue;
	m[1][2] = -sinValue;
	m[2][0] = T(0);
	m[2][1] = sinValue;
	m[2][2] = cosValue;
}

/**
 *	Create a rotation matrix around the y-axis
 */
template <class T>
inline void Matrix3<T>::fromAxisYAngle(T angle)
{
	T cosValue = cos(angle);
	T sinValue = sin(angle);
	m[0][0] = cosValue;
	m[0][1] = T(0);
	m[0][2] = sinValue;
	m[1][0] = T(0);
	m[1][1] = T(1);
	m[1][2] = T(0);
	m[2][0] = -sinValue;
	m[2][1] = T(0);
	m[2][2] = cosValue;
}

/**
 *	Create a rotation matrix around the z-axis
 */
template <class T>
inline void Matrix3<T>::fromAxisZAngle(T angle)
{
	T cosValue = cos(angle);
	T sinValue = sin(angle);
	m[0][0] = cosValue;
	m[0][1] = sinValue;
	m[0][2] = T(0);
	m[1][0] = sinValue;
	m[1][1] = cosValue;
	m[1][2] = T(0);
	m[2][0] = T(0);
	m[2][1] = T(0);
	m[2][2] = T(1);
}

/**
 *	Create a rotation matrix around the axis
 */
template <class T>
inline void Matrix3<T>::fromAxisAngle(const Vector3<T> &axis, T angle)
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
	m[1][0] = XYM + ZSin;
	m[1][1] = Y2 * oneMinusCos + cosValue;
	m[1][2] = YZM - XSin;
	m[2][0] = XZM - YSin;
	m[2][1] = YZM + XSin;
	m[2][2] = Z2 * oneMinusCos + cosValue;
}

/**
 * recover the angle from the rotation matrix
 */
template <class T>
inline void Matrix3<T>::toAxisAngle(Vector3<T> &axis, T &angle) const
{
	// Let (x,y,z) be the unit-length axis and let A be an angle of rotation.
	// The rotation matrix is R = I + sin(A)*P + (1-cos(A))*P^2 where
	// I is the identity and
	//
	//       +-        -+
	//   P = |  0 -z +y |
	//       | +z  0 -x |
	//       | -y +x  0 |
	//       +-        -+
	//
	// If A > 0, R represents a counterclockwise rotation about the axis in
	// the sense of looking from the tip of the axis vector towards the
	// origin.  Some algebra will show that
	//
	//   cos(A) = (trace(R)-1)/2  and  R - R^t = 2*sin(A)*P
	//
	// In the event that A = pi, R-R^t = 0 which prevents us from extracting
	// the axis through P.  Instead note that R = I+2*P^2 when A = pi, so
	// P^2 = (R-I)/2.  The diagonal entries of P^2 are x^2-1, y^2-1, and
	// z^2-1.  We can solve these for axis (x,y,z).  Because the angle is pi,
	// it does not matter which sign you choose on the square roots.

	T trace    = m[0][0] + m[1][1] + m[2][2];
	T cosValue = T(0.5) * (trace - T(1.0));
	angle = acos(cosValue);  // in [0,PI]

	if(angle > 0.0) {
		if (angle < Math<T>::PI) {
			axis[0] = m[2][1] - m[1][2];
			axis[1] = m[0][2] - m[2][0];
			axis[2] = m[1][0] - m[0][1];
			axis = axis.normalize();
		}
		else {
			// angle is PI
			T halfInverse;
			if(m[0][0] >= m[1][1]) {
				// r00 >= r11
				if (m[0][0] >= m[2][2]) {
					// r00 is maximum diagonal term
					axis[0] = T(0.5) * sqrt(m[0][0] - m[1][1] - m[2][2] + T(1));
					halfInverse = T(0.5) / axis[0];
					axis[1] = halfInverse * m[0][1];
					axis[2] = halfInverse * m[0][2];
				}
				else {
					// r22 is maximum diagonal term
					axis[2] = T(0.5) * sqrt(m[2][2] -	m[0][0] - m[1][1] + T(1));
					halfInverse = T(0.5) / axis[2];
					axis[0] = halfInverse * m[0][2];
					axis[1] = halfInverse * m[1][2];
				}
			}
			else {
				// r11 > r00
				if(m[1][1] >= m[2][2]) {
					// r11 is maximum diagonal term
					axis[1] = T(0.5) * sqrt(m[1][1] - m[0][0] - m[2][2] + T(1));
					halfInverse = T(0.5) / axis[1];
					axis[0] = halfInverse * m[0][1];
					axis[2] = halfInverse * m[1][2];
				}
				else {
					// r22 is maximum diagonal term
					axis[2] = T(0.5) * sqrt(m[2][2] - m[0][0] - m[1][1] + T(1));
					halfInverse = T(0.5) / axis[2];
					axis[0] = halfInverse * m[0][2];
					axis[1] = halfInverse * m[1][2];
				}
			}
		}
	}
	else {
		// The angle is 0 and the matrix is the identity.  Any axis will
		// work, so just use the x-axis.
		axis[0] = T(1);
		axis[1] = T(0);
		axis[2] = T(0);
	}
}

/**
 * rotation matrix and Euler angles transformation
 * The matrix must be orthonormal.  The decomposition is yaw*pitch*roll
 * where yaw is rotation about the Up vector, pitch is rotation about the
 * Right axis, and roll is rotation about the Direction axis.
 */
template <class T>
inline bool Matrix3<T>::toEulerAnglesXYZ(T &XAngle, T &YAngle, T &ZAngle) const
{
	// rot =  cy*cz          -cy*sz           sy
	//        cz*sx*sy+cx*sz  cx*cz-sx*sy*sz -cy*sx
	//       -cx*cz*sy+sx*sz  cz*sx+cx*sy*sz  cx*cy

	if(m[0][2] < T(1)) {
		if(m[0][2] > T(-1)) {
			XAngle = atan2(-m[1][2], m[2][2]);
			YAngle = asin(m[0][2]);
			ZAngle = atan2(-m[0][1], m[0][0]);
			return true;
		}
		else {
			// WARNING.  Not unique.  XA - ZA = -atan2(r10,r11)
			XAngle = -atan2(m[1][0], m[1][1]);
			YAngle = -Math<T>::EPSILON * T(0.5);
			ZAngle = T(0);
			return false;
		}
	}
	else {
		// WARNING.  Not unique.  XAngle + ZAngle = atan2(r10,r11)
		XAngle = atan2(m[1][0], m[1][1]);
		YAngle = -Math<T>::HALF_PI;
		ZAngle = T(0);
		return false;
	}
}

template <class T>
inline bool Matrix3<T>::toEulerAnglesXZY(T &XAngle, T &YAngle, T &ZAngle) const
{
	// rot =  cy*cz          -sz              cz*sy
	//        sx*sy+cx*cy*sz  cx*cz          -cy*sx+cx*sy*sz
	//       -cx*sy+cy*sx*sz  cz*sx           cx*cy+sx*sy*sz

	if(m[0][1] < T(1)) {
		if(m[0][1] > T(-1)) {
			XAngle = atan2(m[2][1], m[1][1]);
			ZAngle = asin(-m[0][1]);
			YAngle = atan2(m[0][2], m[0][0]);
			return true;
		}
		else {
			// WARNING.  Not unique.  XA - YA = atan2(r20,r22)
			XAngle = atan2(m[2][0], m[2][2]);
			ZAngle = Math<T>::HALF_PI;
			YAngle = T(0);
			return false;
		}
	}
	else {
		// WARNING.  Not unique.  XA + YA = atan2(-r20,r22)
		XAngle = atan2(-m[2][0], m[2][2]);
		ZAngle = -Math<T>::HALF_PI;
		YAngle = T(0);
		return false;
	}
}

template <class T>
inline bool Matrix3<T>::toEulerAnglesYXZ(T &XAngle, T &YAngle, T &ZAngle) const
{
	// rot =  cy*cz+sx*sy*sz  cz*sx*sy-cy*sz  cx*sy
	//        cx*sz           cx*cz          -sx
	//       -cz*sy+cy*sx*sz  cy*cz*sx+sy*sz  cx*cy

	if(m[1][2] < T(1)) {
		if(m[1][2] > T(-1)) {
			YAngle = atan2(m[0][2], m[2][2]);
			XAngle = asin(-m[1][2]);
			ZAngle = atan2(m[1][0], m[1][1]);
			return true;
		}
		else {
			// WARNING.  Not unique.  YA - ZA = atan2(r01,r00)
			YAngle = atan2(m[0][1], m[0][0]);
			XAngle = Math<T>::HALF_PI;
			ZAngle = T(0);
			return false;
		}
	}
	else {
		// WARNING.  Not unique.  YA + ZA = atan2(-r01,r00)
		YAngle = atan2(-m[0][1], m[0][0]);
		XAngle = -Math<T>::HALF_PI;
		ZAngle = T(0);
		return false;
	}
}

template <class T>
inline bool Matrix3<T>::toEulerAnglesYZX(T &XAngle, T &YAngle, T &ZAngle) const
{
	// rot =  cy*cz           sx*sy-cx*cy*sz  cx*sy+cy*sx*sz
	//        sz              cx*cz          -cz*sx
	//       -cz*sy           cy*sx+cx*sy*sz  cx*cy-sx*sy*sz

	if(m[1][0] < T(1)) {
		if (m[1][0] > T(-1)) {
			YAngle = atan2(-m[2][0], m[0][0]);
			ZAngle = asin(m[1][0]);
			XAngle = atan2(-m[1][2], m[1][1]);
			return true;
		}
		else {
			// WARNING.  Not unique.  YA - XA = -atan2(r21,r22);
			YAngle = -atan2(m[2][1], m[2][2]);
			ZAngle = -Math<T>::HALF_PI;
			XAngle = T(0);
			return false;
		}
	}
	else {
		// WARNING.  Not unique.  YA + XA = atan2(r21,r22)
		YAngle = atan2(m[2][1], m[2][2]);
		ZAngle = Math<T>::HALF_PI;
		XAngle = T(0);
		return false;
	}
}

template <class T>
inline bool Matrix3<T>::toEulerAnglesZXY(T &XAngle, T &YAngle, T &ZAngle) const
{
	// rot =  cy*cz-sx*sy*sz -cx*sz           cz*sy+cy*sx*sz
	//        cz*sx*sy+cy*sz  cx*cz          -cy*cz*sx+sy*sz
	//       -cx*sy           sx              cx*cy

	if(m[2][1] < T(1)) {
		if(m[2][1] > T(-1)) {
			ZAngle = atan2(-m[0][1], m[1][1]);
			XAngle = asin(m[2][1]);
			YAngle = atan2(-m[2][0], m[2][2]);
			return true;
		}
		else {
			// WARNING.  Not unique.  ZA - YA = -atan(r02,r00)
			ZAngle = -atan2(m[0][2], m[0][0]);
			XAngle = -Math<T>::HALF_PI;
			YAngle = T(0);
			return false;
		}
	}
	else {
		// WARNING.  Not unique.  ZA + YA = atan2(r02,r00)
		ZAngle = atan2(m[0][2], m[0][0]);
		XAngle = Math<T>::HALF_PI;
		YAngle = T(0);
		return false;
	}
}

template <class T>
inline bool Matrix3<T>::toEulerAnglesZYX(T &XAngle, T &YAngle, T &ZAngle) const
{
	// rot =  cy*cz           cz*sx*sy-cx*sz  cx*cz*sy+sx*sz
	//        cy*sz           cx*cz+sx*sy*sz -cz*sx+cx*sy*sz
	//       -sy              cy*sx           cx*cy

	if(m[2][0] < T(1)) {
		if (m[2][0] > T(-1)) {
			ZAngle = atan2(m[1][0], m[0][0]);
			YAngle = asin(-m[2][0]);
			XAngle = atan2(m[2][1], m[2][2]);
			return true;
		}
		else {
			// WARNING.  Not unique.  ZA - XA = -atan2(r01,r02)
			ZAngle = -atan2(m[0][1], m[0][2]);
			YAngle = Math<T>::HALF_PI;
			XAngle = T(0);
			return false;
		}
	}
	else {
		// WARNING.  Not unique.  ZA + XA = atan2(-r01,-r02)
		ZAngle = atan2(-m[0][1], -m[0][2]);
		YAngle = -Math<T>::HALF_PI;
		XAngle = T(0);
		return false;
	}
}

template <class T>
inline void Matrix3<T>::fromEulerAnglesXYZ(T XAngle, T YAngle, T ZAngle)
{
	T fCos = cos(XAngle);
	T fSin = sin(XAngle);
	Matrix3<T> kXMat(
		1, 0,    0,
		0, fCos, -fSin,
		0, fSin, fCos);

	fCos = cos(YAngle);
	fSin = sin(YAngle);
	Matrix3<T> kYMat(
		fCos,  0, fSin,
		0,     1, 0.0,
		-fSin, 0, fCos);

	fCos = cos(ZAngle);
	fSin = sin(ZAngle);
	Matrix3<T> kZMat(
		fCos, -fSin, 0,
		fSin, fCos,  0,
		0,    0,     1);

	*this = kXMat * (kYMat * kZMat);
}

template <class T>
inline void Matrix3<T>::fromEulerAnglesXZY(T XAngle, T ZAngle, T YAngle)
{
	T fCos = cos(YAngle);
	T fSin = sin(YAngle);
	Matrix3<T> kXMat(
		1, 0,    0,
		0, fCos, -fSin,
		0, fSin, fCos);

	fCos = cos(YAngle);
	fSin = sin(YAngle);
	Matrix3<T> kYMat(
		fCos,  0, fSin,
		0,     1, 0,
		-fSin, 0, fCos);

	fCos = cos(ZAngle);
	fSin = sin(ZAngle);
	Matrix3<T> kZMat(
		fCos, -fSin, 0,
		fSin, fCos,  0,
		0,    0,     1);

	*this = kXMat * (kZMat * kYMat);
}

template <class T>
inline void Matrix3<T>::fromEulerAnglesYXZ(T YAngle, T XAngle, T ZAngle)
{
	T fCos = cos(YAngle);
	T fSin = sin(YAngle);
	Matrix3<T> kXMat(
		1, 0,    0,
		0, fCos, -fSin,
		0, fSin, fCos);

	fCos = cos(YAngle);
	fSin = sin(YAngle);
	Matrix3<T> kYMat(
		fCos,  0, fSin,
		0,     1, 0,
		-fSin, 0, fCos);

	fCos = cos(ZAngle);
	fSin = sin(ZAngle);
	Matrix3<T> kZMat(
		fCos, -fSin, 0,
		fSin, fCos,  0,
		0,    0,     1);

	*this = kYMat * (kXMat * kZMat);
}

template <class T>
inline void Matrix3<T>::fromEulerAnglesYZX(T YAngle, T ZAngle, T XAngle)
{
	T fCos = cos(YAngle);
	T fSin = sin(YAngle);
	Matrix3<T> kXMat(
		1, 0,    0,
		0, fCos, -fSin,
		0, fSin, fCos);

	fCos = cos(YAngle);
	fSin = sin(YAngle);
	Matrix3<T> kYMat(
		fCos,  0, fSin,
		0,     1, 0,
		-fSin, 0, fCos);

	fCos = cos(ZAngle);
	fSin = sin(ZAngle);
	Matrix3<T> kZMat(
		fCos, -fSin, 0,
		fSin, fCos,  0,
		0,    0,     1);

	*this = kYMat * (kZMat * kXMat);
}

template <class T>
inline void Matrix3<T>::fromEulerAnglesZXY(T ZAngle, T XAngle, T YAngle)
{
	T fCos = cos(YAngle);
	T fSin = sin(YAngle);
	Matrix3<T> kXMat(
		1, 0,    0,
		0, fCos, -fSin,
		0, fSin, fCos);

	fCos = cos(YAngle);
	fSin = sin(YAngle);
	Matrix3<T> kYMat(
		fCos,  0, fSin,
		0,     1, 0,
		-fSin, 0, fCos);

	fCos = cos(ZAngle);
	fSin = sin(ZAngle);
	Matrix3<T> kZMat(
		fCos, -fSin, 0,
		fSin, fCos,  0,
		0,    0,     1);

	*this = kZMat * (kXMat * kYMat);
}

template <class T>
inline void Matrix3<T>::fromEulerAnglesZYX(T ZAngle, T YAngle, T XAngle)
{
	T fCos = cos(YAngle);
	T fSin = sin(YAngle);
	Matrix3<T> kXMat(
		1, 0,    0,
		0, fCos, -fSin,
		0, fSin, fCos);

	fCos = cos(YAngle);
	fSin = sin(YAngle);
	Matrix3 kYMat(
		fCos,  0, fSin,
		0,     1, 0,
		-fSin, 0, fCos);

	fCos = cos(ZAngle);
	fSin = sin(ZAngle);
	Matrix3<T> kZMat(
		fCos, -fSin, 0,
		fSin, fCos,  0,
		0,    0,     1);

	*this = kZMat * (kYMat * kXMat);
}

/**
 * support for eigendecomposition
 */
template <class T>
inline void Matrix3<T>::tridiagonalize(T afDiag[3], T afSubDiag[3])
{
	// Householder reduction T = Q^t M Q
	//   Input:   
	//     mat, symmetric 3x3 matrix M
	//   Output:  
	//     mat, orthogonal matrix Q (a reflection)
	//     diag, diagonal entries of T
	//     subd, subdiagonal entries of T (T is symmetric)

	T fA = m[0][0];
	T fB = m[0][1];
	T fC = m[0][2];
	T fD = m[1][1];
	T fE = m[1][2];
	T fF = m[2][2];

	afDiag[0] = fA;
	afSubDiag[2] = T(0);
	if(abs(fC) >= Math<T>::EPSILON) {
		T fLength = sqrt(fB * fB + fC * fC);
		T fInvLength = T(1) / fLength;
		fB *= fInvLength;
		fC *= fInvLength;
		T fQ = T(2) * fB * fE + fC * (fF - fD);
		afDiag[1] = fD + fC * fQ;
		afDiag[2] = fF - fC * fQ;
		afSubDiag[0] = fLength;
		afSubDiag[1] = fE - fB * fQ;

		m[0][0] = T(1);
		m[0][1] = T(0);
		m[0][2] = T(0);
		m[1][0] = T(0);
		m[1][1] = fB;
		m[1][2] = fC;
		m[2][0] = T(0);
		m[2][1] = fC;
		m[2][2] = -fB;
	}
	else {
		afDiag[1] = fD;
		afDiag[2] = fF;
		afSubDiag[0] = fB;
		afSubDiag[1] = fE;

		m[0][0] = T(1);
		m[0][1] = T(0);
		m[0][2] = T(0);
		m[1][0] = T(0);
		m[1][1] = T(1);
		m[1][2] = T(0);
		m[2][0] = T(0);
		m[2][1] = T(0);
		m[2][2] = T(-1);
	}
}

template <class T>
inline bool Matrix3<T>::QLAlgorithm(T afDiag[3], T afSubDiag[3])
{
	// QL iteration with implicit shifting to reduce matrix from tridiagonal to diagonal

	const int iMaxIter = 32;
	for (int i0 = 0; i0 < 3; ++i0) {
		int iIter;
		for (iIter = 0; iIter < iMaxIter; ++iIter) {
			int i1 = i0;
			for (; i1 < 2; ++i1) {
				T fSum = abs(afDiag[i1]) + abs(afDiag[i1 + 1]);
				if (abs(afSubDiag[i1]) + fSum == fSum)
					break;
			}
			if (i1 == i0)
				break;

			T fTmp0 = (afDiag[i0+1] - afDiag[i0]) / (T(2) * afSubDiag[i0]);
			T fTmp1 = sqrt(fTmp0 * fTmp0 + T(1));
			if (fTmp0 < T(1))
				fTmp0 = afDiag[i1] - afDiag[i0] + afSubDiag[i0] / (fTmp0 - fTmp1);
			else
				fTmp0 = afDiag[i1] - afDiag[i0] + afSubDiag[i0] / (fTmp0+fTmp1);
			T fSin = T(1);
			T fCos = T(1);
			T fTmp2 = T(0);
			for (int i2 = i1 - 1; i2 >= i0; --i2) {
				T fTmp3 = fSin * afSubDiag[i2];
				T fTmp4 = fCos * afSubDiag[i2];
				if (abs(fTmp3) >= abs(fTmp0)) {
					fCos = fTmp0 / fTmp3;
					fTmp1 = sqrt(fCos * fCos + T(1));
					afSubDiag[i2 + 1] = fTmp3 * fTmp1;
					fSin = T(1) / fTmp1;
					fCos *= fSin;
				}
				else {
					fSin = fTmp3 / fTmp0;
					fTmp1 = sqrt(fSin * fSin + T(1));
					afSubDiag[i2 + 1] = fTmp0 * fTmp1;
					fCos = T(1) / fTmp1;
					fSin *= fCos;
				}
				fTmp0 = afDiag[i2 + 1] - fTmp2;
				fTmp1 = (afDiag[i2] - fTmp0) * fSin+ T(2) * fTmp4 * fCos;
				fTmp2 = fSin * fTmp1;
				afDiag[i2+1] = fTmp0 + fTmp2;
				fTmp0 = fCos * fTmp1 - fTmp4;

				for (int iRow = 0; iRow < 3; iRow++) {
					fTmp3 = m[iRow][i2 + 1];
					m[iRow][i2 + 1] = fSin * m[iRow][i2] + fCos * fTmp3;
					m[iRow][i2] = fCos * m[iRow][i2] - fSin * fTmp3;
				}
			}
			afDiag[i0]   -= fTmp2;
			afSubDiag[i0] = fTmp0;
			afSubDiag[i1] = T(0);
		}

		if (iIter == iMaxIter) {
			// should not get here under normal circumstances
			vxAssert(false, "iIter == iMaxIter");
			return false;
		}
	}

	return true;
}

/**
 * Vector3<T>^T * Matrix3<T>
 */
template <class T>
inline const Vector3<T> operator * (const Vector3<T> &v, const Matrix3<T> &m)
{
	return Vector3<T>(v[0] * m.m[0][0] + v[1] * m.m[1][0] + v[2] * m.m[2][0], 
					  v[0] * m.m[0][1] + v[1] * m.m[1][1] + v[2] * m.m[2][1],
					  v[0] * m.m[0][2] + v[1] * m.m[1][2] + v[2] * m.m[2][2]);
}

/**
 * stream operator
 */
template <class T>
inline std::ostream& operator << (std::ostream &out, const Matrix3<T> &m)
{
	out << '|' << m.m[0][0] << ", " << m.m[0][1] << ", " << m.m[0][2] << "|\r\n";
	out << '|' << m.m[1][0] << ", " << m.m[1][1] << ", " << m.m[1][2] << "|\r\n";
	out << '|' << m.m[2][0] << ", " << m.m[2][1] << ", " << m.m[2][2] << "|";
	return out;
}

/**
 * types
 */
typedef Matrix3<float>  Matrix3f;
typedef Matrix3<double> Matrix3d;

#endif // MATRIX3_H
