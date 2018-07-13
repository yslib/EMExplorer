#ifndef MATRIX2_H
#define MATRIX2_H

#include "Vector.h"

/**
 * 2D matrix class - a row major order
 */
template <class T>
struct Matrix2 
{
public:
	/// default constructor
	Matrix2(T m00 = T(0), T m01 = T(0), T m10 = T(0), T m11 = T(0));

	/// copy constructor
	Matrix2(const Matrix2 &matrix);	

	/// assignment operator
	Matrix2& operator = (const Matrix2 &matrix);

	/// boolean operators
	bool operator == (const Matrix2 &matrix) const;
	bool operator != (const Matrix2 &matrix) const;

	/// row data access
	Vector2<T> row(const size_t iRow)    const;
	/// column data access
	Vector2<T> column(const size_t iCol) const;
	/// row data access
	const T* operator [] (const size_t iRow) const;
	/// row data access
	T*       operator [] (const size_t iRow);


	/// Matrix arithmetic operations
	Matrix2  operator +  (const Matrix2 &matrix) const;
	Matrix2& operator += (const Matrix2 &matrix);
	Matrix2  operator -  (const Matrix2 &matrix) const;
	Matrix2& operator -= (const Matrix2 &matrix);
	Matrix2  operator *  (const Matrix2 &matrix) const;
	Matrix2& operator *= (const Matrix2 &matrix);

	/// matrix and vector multiplication
	Vector2<T> operator * (const Vector2<T> &vec) const;

	// matrix creation operations
	/// make zero matrix
	void zero();
	/// make identity matrix I
	void identity ();
	/// create a diagonal matrix
	void diagonal(T m00, T m11);
	/// set matrix elements
	void setElements(T m00, T m01, T m10, T m11);
	/// create a tensor product U*V^T
	void makeTensorProduct(const Vector2<T> &U, const Vector2<T> &V);

	// matrix math operations
	/// inverse matrix A^-1
	Matrix2 inverse () const;
	/// make adjoint matrix A*
	Matrix2 adjoint () const;
	/// transpose matrix A^T
	Matrix2 transpose () const;
	/// determinant of matrix
	T determinant () const;
	/// Gram-Schmidt orthonormalization
	void orthonormalize();
	/// eigenvalue and eigenvector decomposition
	void eigenDecomposition(Matrix2 &rot, Matrix2 &diag) const;

	// rotation matrix
	/// create a rotation matrix (positive angle - counterclockwise)
	void fromAngle(T angle);
	/// recover the angle from the rotation matrix
	T toAngle() const;

	/// matrix element in a row major order
    T m[2][2];
};


/**
 *	default constructor
 */
template <class T>
Matrix2<T>::Matrix2(T m00, T m01, T m10, T m11)
{
	m[0][0] = m00;
	m[0][1] = m01;
	m[1][0] = m10;
	m[1][1] = m11;
}

/**
 * copy constructor
 */
template <class T>
Matrix2<T>::Matrix2(const Matrix2<T> &matrix)
{
	m[0][0] = matrix.m[0][0];
	m[0][1] = matrix.m[0][1];
	m[1][0] = matrix.m[1][0];
	m[1][1] = matrix.m[1][1];
}

/**
 * assignment operator
 */
template <class T>
Matrix2<T>& Matrix2<T>::operator = (const Matrix2<T> &matrix)
{
	m[0][0] = matrix.m[0][0];
	m[0][1] = matrix.m[0][1];
	m[1][0] = matrix.m[1][0];
	m[1][1] = matrix.m[1][1];
	return (*this);
}

/**
 * boolean operators
 */
template <class T>
inline bool Matrix2<T>::operator == (const Matrix2 &matrix) const
{
	return (m[0][0] == matrix.m[0][0] && m[0][1] == matrix.m[0][1] &&
		    m[1][0] == matrix.m[1][0] && m[1][1] == matrix.m[1][1]);
}

template <class T>
inline bool Matrix2<T>::operator != (const Matrix2 &matrix) const
{
	return !(*this == matrix);
}

/**
 * row data access
 */
template <class T>
inline Vector2<T> Matrix2<T>::row(const size_t iRow) const
{
	vxAssert(iRow < 2, "out of range");
	return Vector2<T>(m[iRow][0], m[iRow][1]);
}

/** 
 * column data access
 */
template <class T>
inline Vector2<T> Matrix2<T>::column(const size_t iCol) const
{
	vxAssert(iCol < 2, "out of range");
	return Vector2<T>(m[0][iCol], m[1][iCol]);
}

/**
 * row data access
 */
template <class T> 
inline const T* Matrix2<T>::operator [](const size_t iRow) const
{
	vxAssert(iRow < 2, "out of range");
	return m[iRow];
}

/**
 * row data access
 */
template <class T> 
inline T* Matrix2<T>::operator [](const size_t iRow)
{
	vxAssert(iRow < 2, "out of range");
	return m[iRow];
}

/**
 *	Matrix arithmetic operations
 */
template <class T> 
inline Matrix2<T> Matrix2<T>::operator + (const Matrix2<T> &matrix) const
{
	return Matrix2(m[0][0] + matrix.m[0][0], m[0][1] + matrix.m[0][1], m[1][0] + matrix.m[1][0], m[1][1] + matrix.m[1][1]);
}

template <class T> 
inline Matrix2<T>& Matrix2<T>::operator += (const Matrix2<T> &matrix)
{
	m[0][0] += matrix.m[0][0];
	m[0][1] += matrix.m[0][1];
	m[1][0] += matrix.m[1][0];
	m[1][1] += matrix.m[1][1];
	return (*this);
}

template <class T> 
inline Matrix2<T> Matrix2<T>::operator - (const Matrix2<T> &matrix) const
{
	return Matrix2<T>(m[0][0] - matrix.m[0][0], m[0][1] - matrix.m[0][1], m[1][0] - matrix.m[1][0], m[1][1] - matrix.m[1][1]);
}

template <class T> 
inline Matrix2<T>& Matrix2<T>::operator -= (const Matrix2<T> &matrix)
{
	m[0][0] -= matrix.m[0][0];
	m[0][1] -= matrix.m[0][1];
	m[1][0] -= matrix.m[1][0];
	m[1][1] -= matrix.m[1][1];
	return (*this);
}

template <class T> 
inline Matrix2<T> Matrix2<T>::operator * (const Matrix2<T> &matrix) const
{
	//Matrix2<T> product;
	//for(size_t i = 0; i < 2; ++i) {
	//	for(size_t j = 0; j < 2; ++j) {
	//		T value(0);
	//		for(size_t k = 0; k < 2; ++k)
	//			value += m[i][k] * matrix.m[k][j];
	//		product.m[i][j] = value;
	//	}
	//}
	//return product;

	// unroll loop
	Matrix2<T> product;
	product.m[0][0] = m[0][0] * matrix.m[0][0] + m[0][1] * matrix.m[1][0];
	product.m[0][1] = m[0][0] * matrix.m[0][1] + m[0][1] * matrix.m[1][1];
	product.m[1][0] = m[1][0] * matrix.m[0][0] + m[1][1] * matrix.m[1][0];
	product.m[1][1] = m[1][0] * matrix.m[0][1] + m[1][1] * matrix.m[1][1];
	return product;
}

template <class T> 
inline Matrix2<T>& Matrix2<T>::operator *= (const Matrix2<T> &matrix)
{
	//Matrix2<T> product;
	//for(size_t i = 0; i < 2; ++i) {
	//	for(size_t j = 0; j < 2; ++j) {
	//		T value(0);
	//		for(size_t k = 0; k < 2; ++k)
	//			value += m[i][k] * matrix.m[k][j];
	//		product.m[i][j] = value;
	//	}
	//}
	//(*this) = product;
	//return (*this);
	
	// unroll loop
	T m00 = m[0][0] * matrix.m[0][0] + m[0][1] * matrix.m[1][0];
	T m01 = m[0][0] * matrix.m[0][1] + m[0][1] * matrix.m[1][1];
	T m10 = m[1][0] * matrix.m[0][0] + m[1][1] * matrix.m[1][0];
	T m11 = m[1][0] * matrix.m[0][1] + m[1][1] * matrix.m[1][1];

	m[0][0] = m00;
	m[0][1] = m01;
	m[1][0] = m10;
	m[1][1] = m11;

	return (*this);
}

/**
 * matrix and vector multiplication
 */
template <class T> 
inline Vector2<T> Matrix2<T>::operator * (const Vector2<T> &vec) const
{
	return Vector2<T>(m[0][0] * vec[0] + m[0][1] * vec[1], m[1][0] * vec[0] + m[1][1] * vec[1]);
}

/**
 * make zero matrix
 */
template <class T> 
inline void Matrix2<T>::zero()
{
	m[0][0] = m[0][1] = T(0);
	m[1][0] = m[1][1] = T(0);
}

/**
 * make identity matrix I
 */
template <class T> 
inline void Matrix2<T>::identity()
{
	m[0][0] = T(1);
	m[0][1] = T(0);
	m[1][0] = T(0);
	m[1][1] = T(1);
}


/**
 * create a diagonal matrix
 */
template <class T>
inline void Matrix2<T>::diagonal(T m00, T m11)
{
	m[0][0] = m00;
	m[0][1] = T(0);
	m[1][0] = T(0);
	m[1][1] = m11;
}

/**
 * set matrix elements
 */
template <class T>
inline void Matrix2<T>::setElements(T m00, T m01, T m10, T m11)
{
	m[0][0] = m00;
	m[0][1] = m01;
	m[1][0] = m10;
	m[1][1] = m11;
}

/**
 * create a tensor product U*V^T
 */
template <class T> 
inline void Matrix2<T>::makeTensorProduct(const Vector2<T> &U, const Vector2<T> &V)
{
	m[0][0] = U[0] * V[0];
	m[0][1] = U[0] * V[1];
	m[1][0] = U[1] * V[0];
	m[1][1] = U[1] * V[1];
}

/**
 * inverse matrix A^-1
 */
template <class T>
inline Matrix2<T> Matrix2<T>::inverse() const
{
	Matrix2<T> invMatrix;

	T det = determinant();
	if(abs(det) > Math<T>::EPSILON) {
		T invDet = T(1) / det;
		invMatrix.m[0][0] =  m[1][1] * invDet;
		invMatrix.m[0][1] = -m[0][1] * invDet;
		invMatrix.m[1][0] = -m[1][0] * invDet;
		invMatrix.m[1][1] =  m[0][0] * invDet;
	}

	return invMatrix;
}

/**
 * make adjoint matrix A*
 */
template <class T>
inline Matrix2<T> Matrix2<T>::adjoint() const
{
	return Matrix2<T>(m[1][1], -m[0][1], -m[1][0], m[0][0]);
}

/**
 * transpose matrix A^T
 */
template <class T>
inline Matrix2<T> Matrix2<T>::transpose() const
{
	return Matrix2<T>(m[0][0], m[1][0], m[0][1], m[1][1]);
}

/**
 * determinant of matrix
 */
template <class T>
inline T Matrix2<T>::determinant() const
{
	return (m[0][0] * m[1][1] - m[0][1] * m[1][0]);
}

/**
 * Gram-Schmidt orthonormalization
 */
template <class T>
inline void Matrix2<T>::orthonormalize()
{
	// Algorithm uses Gram-Schmidt orthogonalization.  If 'this' matrix is
	// M = [m0|m1], then orthonormal output matrix is Q = [q0|q1],
	//
	//   q0 = m0/|m0|
	//   q1 = (m1-(q0*m1)q0)/|m1-(q0*m1)q0|
	//
	// where |V| indicates length of vector V and A*B indicates dot
	// product of vectors A and B.

	// compute q0
	T invLength = (T(1) / sqrt(m[0][0] * m[0][0] + m[1][0] * m[1][0]));

	m[0][0] *= invLength;
	m[1][0] *= invLength;

	// compute q1
	T dot0 = m[0][0] * m[0][1] + m[1][0] * m[1][1];
	m[0][1] -= dot0 * m[0][0];
	m[1][1] -= dot0 * m[1][0];

	invLength = (T(1) / sqrt(m[0][1] * m[0][1] + m[1][1] * m[1][1]));

	m[0][1] *= invLength;
	m[1][1] *= invLength;
}

/**
 * eigenvalue and eigenvector decomposition
 * The matrix must be symmetric.  Factor M = R * D * R^T where
 * R = [u0|u1] is a rotation matrix with columns u0 and u1 and
 * D = diag(d0,d1) is a diagonal matrix whose diagonal entries are d0 and
 * d1.  The eigenvector u[i] corresponds to eigenvector d[i].  The
 * eigenvalues are ordered as d0 <= d1.
 */
template <class T>
inline void Matrix2<T>::eigenDecomposition(Matrix2 &rot, Matrix2 &diag) const
{
	T trace = m[0][0] + m[1][1];
	T diff  = m[0][0] - m[1][1];
	T discr = sqrt(diff * diff + 4 * m[0][1] * m[0][1]);
	T eVal0 = T(0.5) * (trace - discr);
	T eVal1 = T(0.5) * (trace + discr);
	diag.makeDiagonal(eVal0, eVal1);

	Vector2<T> kRow0(m[0][0] - eVal0, m[0][1]);
	Vector2<T> kRow1(m[0][1], m[1][1] - eVal0);
	T fLength0 = kRow0.norm();
	T fLength1 = kRow1.norm();

	if (fLength0 > fLength1) {
		if (fLength0 > Math<T>::EPSILON) {
			rot.m[0][0] = kRow0[1];
			rot.m[0][1] = kRow0[0];
			rot.m[1][1] = -kRow0[0];
			rot.m[1][1] = kRow0[1];
		}
		else {
			rot.identity();
		}
	}
	else {
		if (fLength1 > Math<T>::EPSILON) {
			rot.m[0][0] = kRow1[1];
			rot.m[0][1] = kRow1[0];
			rot.m[1][0] = -kRow1[0];
			rot.m[1][1] = kRow1[1];
		}
		else {
			rot.identity();
		}
	}
}

/**
 *	Create a rotation matrix (positive angle - counterclockwise)
 *  Rotation matrices are of the form
 *    R = cos(t) -sin(t)
 *        sin(t)  cos(t)
 *  where t > 0 indicates a counterclockwise rotation in the xy-plane.
 */
template <class T>
inline void Matrix2<T>::fromAngle(T angle)
{
	m[0][0] = cos(angle);
	m[1][0] = sin(angle);
	m[0][1] = -m[1][0];
	m[1][1] =  m[0][0];
}

/**
 * recover the angle from the rotation matrix
 */
template <class T>
inline T Matrix2<T>::toAngle() const
{
	return atan2(m[1][0], m[0][0]);
}

/**
 * Vector2<T>^T * Matrix2<T>
 */
template <class T>
inline const Vector2<T> operator * (const Vector2<T> &v, const Matrix2<T> &m)
{
	return Vector2<T>(v[0] * m.m[0][0] + v[1] * m.m[1][0], v[0] * m.m[0][1] + v[1] * m.m[1][1]);
}

/**
 * stream operator
 */
template <class T>
inline std::ostream& operator << (std::ostream &out, const Matrix2<T> &m)
{
	out << '|' << m.m[0][0] << ", " << m.m[0][1] << "|\r\n";
	out << '|' << m.m[1][0] << ", " << m.m[1][1] << "|";
	return out;
}

/**
 * types
 */
typedef Matrix2<float>  Matrix2f;
typedef Matrix2<double> Matrix2d;

#endif // MATRIX2_H
