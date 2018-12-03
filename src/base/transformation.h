#ifndef TRANSFORMATION_H_
#define TRANSFORMATION_H_
#include <iostream>
#include <cmath>

#include "globals.h"
#include "geometry.h"
//#include "arithmetic.h"

#include <valarray>

#define HSL_MATRIX_COLUMN_MAJOR

#ifdef YSL_TO_QT
#include <QMatrix4x4>
#endif


namespace  ysl {

	struct Matrix4x4 {
		// Matrix4x4 Public Methods
		Matrix4x4()
		{
			m[0][0] = m[1][1] = m[2][2] = m[3][3] = 1.f;
			m[0][1] = m[0][2] = m[0][3] = m[1][0] = m[1][2] = m[1][3] = m[2][0] =
				m[2][1] = m[2][3] = m[3][0] = m[3][1] = m[3][2] = 0.f;
		}

		Matrix4x4(Float mat[4][4])
		{
			::memcpy(m, mat, sizeof(Float) * 16);
		}

		Matrix4x4(Float t00, Float t01, Float t02, Float t03, Float t10, Float t11,
			Float t12, Float t13, Float t20, Float t21, Float t22, Float t23,
			Float t30, Float t31, Float t32, Float t33)
		{
			m[0][0] = t00;
			m[0][1] = t01;
			m[0][2] = t02;
			m[0][3] = t03;
			m[1][0] = t10;
			m[1][1] = t11;
			m[1][2] = t12;
			m[1][3] = t13;
			m[2][0] = t20;
			m[2][1] = t21;
			m[2][2] = t22;
			m[2][3] = t23;
			m[3][0] = t30;
			m[3][1] = t31;
			m[3][2] = t32;
			m[3][3] = t33;
		}

		bool operator==(const Matrix4x4 &m2) const {
			for (int i = 0; i < 4; ++i)
				for (int j = 0; j < 4; ++j)
					if (m[i][j] != m2.m[i][j]) return false;
			return true;
		}
		bool operator!=(const Matrix4x4 &m2) const {
			for (int i = 0; i < 4; ++i)
				for (int j = 0; j < 4; ++j)
					if (m[i][j] != m2.m[i][j]) return true;
			return false;
		}

		void Transpose()
		{
			for (auto i = 0; i < 3; i++) {
				for (auto j = 0; j < 3; j++) {
					const auto t = m[i][j];
					m[i][j] = m[j][i];
					m[j][i] = t;
				}
			}
		}

		Matrix4x4 Transposed()const
		{
			Matrix4x4 mat(m[0][0], m[1][0], m[2][0], m[3][0],
				m[0][1], m[1][1], m[2][1], m[3][1],
				m[0][2], m[1][2], m[2][2], m[3][2],
				m[0][3], m[1][3], m[2][3], m[3][3]);
			return mat;
		}

		void Inverse()
		{
			//assert(false);
			*this = Inversed();
		}

		Matrix4x4 Inversed()const {

			int indxc[4], indxr[4];
			int ipiv[4] = { 0, 0, 0, 0 };
			Float minv[4][4];
			::memcpy(minv, this->m, 4 * 4 * sizeof(Float));
			for (int i = 0; i < 4; i++) {
				int irow = 0, icol = 0;
				Float big = 0.f;
				// Choose pivot
				for (int j = 0; j < 4; j++) {
					if (ipiv[j] != 1) {
						for (int k = 0; k < 4; k++) {
							if (ipiv[k] == 0) {
								if (std::abs(minv[j][k]) >= big) {
									big = Float(std::abs(minv[j][k]));
									irow = j;
									icol = k;
								}
							}
							else if (ipiv[k] > 1)
								YSL_ASSERT_X(false, "Matrix4x4::Inverse", "Singular matrix in MatrixInvert");
						}
					}
				}
				++ipiv[icol];
				// Swap rows _irow_ and _icol_ for pivot
				if (irow != icol) {
					for (int k = 0; k < 4; ++k) std::swap(minv[irow][k], minv[icol][k]);
				}
				indxr[i] = irow;
				indxc[i] = icol;
				if (minv[icol][icol] == 0.f)
					YSL_ASSERT_X(false, "Matrix4x4::Inverse", "Singular matrix in MatrixInvert");

				// Set $m[icol][icol]$ to one by scaling row _icol_ appropriately
				Float pivinv = 1. / minv[icol][icol];
				minv[icol][icol] = 1.;
				for (int j = 0; j < 4; j++) minv[icol][j] *= pivinv;

				// Subtract this row from others to zero out their columns
				for (int j = 0; j < 4; j++) {
					if (j != icol) {
						Float save = minv[j][icol];
						minv[j][icol] = 0;
						for (int k = 0; k < 4; k++) minv[j][k] -= minv[icol][k] * save;
					}
				}
			}
			// Swap columns to reflect permutation
			for (int j = 3; j >= 0; j--) {
				if (indxr[j] != indxc[j]) {
					for (int k = 0; k < 4; k++)
						std::swap(minv[k][indxr[j]], minv[k][indxc[j]]);
				}
			}
			return Matrix4x4{ minv };
		}

		static Matrix4x4 Mul(const Matrix4x4 & m1, const Matrix4x4 & m2)
		{
			Matrix4x4 r;
			for (auto i = 0; i < 4; ++i)
				for (auto j = 0; j < 4; ++j)
					r.m[i][j] = m1.m[i][0] * m2.m[0][j] + m1.m[i][1] * m2.m[1][j] +
					m1.m[i][2] * m2.m[2][j] + m1.m[i][3] * m2.m[3][j];
			return r;
		}

		friend std::ostream &operator<<(std::ostream &os, const Matrix4x4 &m)
		{
			os << "[" <<
				m.m[0][0] << "," << m.m[0][1] << "," << m.m[0][2] << "," << m.m[0][3] << "]\n" <<
				m.m[1][0] << "," << m.m[1][1] << "," << m.m[1][2] << "," << m.m[1][3] << "]\n" <<
				m.m[2][0] << "," << m.m[2][1] << "," << m.m[2][2] << "," << m.m[2][3] << "]\n" <<
				m.m[3][0] << "," << m.m[3][1] << "," << m.m[3][2] << "," << m.m[3][3] << "]\n";
			return os;
		}

		Float m[4][4];
	};

	inline
		Matrix4x4
		operator*(const Matrix4x4 & m1, const Matrix4x4 & m2)
	{
		return Matrix4x4::Mul(m1, m2);
	}


	class Transform {
	public:
		// Transform Public Methods
		Transform() = default;

		explicit Transform(const Float mat[4][4])
		{
			m_m = Matrix4x4(mat[0][0], mat[0][1], mat[0][2], mat[0][3], mat[1][0],
				mat[1][1], mat[1][2], mat[1][3], mat[2][0], mat[2][1],
				mat[2][2], mat[2][3], mat[3][0], mat[3][1], mat[3][2],
				mat[3][3]);
			m_inv = m_m.Inversed();
		}

		Transform(const Matrix4x4 & m, const Matrix4x4 & inv) :m_m(m), m_inv(inv)
		{

		}


		explicit Transform(const Matrix4x4 & m) :m_m(m), m_inv(m.Inversed())
		{

		}


		Transform Transposed() const;

		void Transpose();

		friend Transform Transpose(const Transform & trans)
		{
			return { trans.m_inv, trans.m_m };
		}

		bool operator==(const Transform& t) const;

		bool operator!=(const Transform& t) const;

		bool IsIdentity() const;

		Transform Inversed()const { return Transform{m_inv,m_m};}

		const Matrix4x4& Matrix() const;

		const Matrix4x4& InverseMatrix() const;

		void LookAt(const Point3f & eye, const Point3f & center, const Vector3f & up);

		void SetTranslate(const Vector3f& t);

		void SetTranslate(Float x, Float y, Float z);

		void SetTranslate(Float * t);

		void SetScale(const Vector3f & s);

		void SetScale(Float x, Float y, Float z);

		void SetScale(Float * s);

		void SetRotate(const Vector3f & axis, Float degrees);

		void SetRotate(Float x, Float y, Float z, Float degrees);

		void SetRotate(Float *a, Float degrees);

		void SetRotateX(Float degrees);

		void SetRotateY(Float degrees);

		void SetRotateZ(Float degrees);

		Transform operator*(const Transform & trans)const;

		template<typename T> Point3<T> operator*(const Point3<T> & p)const;

		template<typename T> Vector3<T> operator*(const Vector3<T> & v)const;

		template<typename T> Ray operator*(const Ray & ray)const;

		template<typename T> AABB operator*(const AABB & aabb)const;



		friend std::ostream &operator<<(std::ostream &os, const Transform & t)
		{
			os << "t = " << t.m_m << " t' = " << t.m_inv;
			return os;
		}

	private:
		Matrix4x4 m_m;
		Matrix4x4 m_inv;
	};

	inline
		Transform
		Transform::Transposed() const
	{
		return { m_inv, m_m };
	}

	inline
		void
		Transform::Transpose()
	{
		m_m.Inverse();
		m_inv.Inverse();
	}

	inline
		bool
		Transform::operator==(const Transform& t) const
	{
		return t.m_m == m_m;
	}

	inline
		bool
		Transform::operator!=(const Transform& t) const
	{
		return !(*this == t);
	}

	inline
		bool
		Transform::IsIdentity() const
	{
		return (m_m.m[0][0] == 1.f && m_m.m[0][1] == 0.f && m_m.m[0][2] == 0.f &&
			m_m.m[0][3] == 0.f && m_m.m[1][0] == 0.f && m_m.m[1][1] == 1.f &&
			m_m.m[1][2] == 0.f && m_m.m[1][3] == 0.f && m_m.m[2][0] == 0.f &&
			m_m.m[2][1] == 0.f && m_m.m[2][2] == 1.f && m_m.m[2][3] == 0.f &&
			m_m.m[3][0] == 0.f && m_m.m[3][1] == 0.f && m_m.m[3][2] == 0.f &&
			m_m.m[3][3] == 1.f);
	}

	inline const
		Matrix4x4&
		Transform::Matrix() const
	{
		return m_m;
	}

	inline const
		Matrix4x4&
		Transform::InverseMatrix() const
	{
		return m_inv;
	}

	inline 
	void 
	Transform::LookAt(const Point3f& eye, const Point3f& center, const Vector3f& up)
	{
		const auto direction = (center - eye).Normalized();
		const auto right = Vector3f::Cross(direction, up.Normalized());
		const auto newUp = Vector3f::Cross(right, direction).Normalized();

		m_m = Matrix4x4
		{
			right.x,newUp.x,direction.x,eye.x,
			right.y,newUp.y,direction.y,eye.y,
			right.z,newUp.z,direction.z,eye.z,
			0.f,0.f,0.f,1.0f
		};
		m_inv = m_m.Inversed();
	}


	inline void Transform::SetTranslate(const Vector3f& t)
	{
		SetTranslate(t[0], t[1], t[2]);
	}

	inline
		void
		Transform::SetTranslate(Float x, Float y, Float z)
	{
		m_m = Matrix4x4{ 1.f,0.f,0.f,x,
				   0.f,1.f,0.f,y,
				   0.f,0.f,1.f,z,
				   0.f,0.f,0.f,1.f };
		m_inv = Matrix4x4{ 1.f, 0.f, 0.f, -x,
		   0.f, 1.f, 0.f, -y,
		   0.f, 0.f, 1.f, -z,
		   0.f, 0.f, 0.f, 1.f };
	}

	inline 
	void 
	Transform::SetTranslate(Float* t)
	{
		SetTranslate(t[0], t[1], t[2]);
	}

	inline 
	void 
	Transform::SetScale(const Vector3f& s)
	{
		SetScale(s[0], s[1], s[2]);
	}

	inline void Transform::SetScale(Float x, Float y, Float z)
	{
		m_m = Matrix4x4
		{
		   x,0.f,0.f,0.f,
		   0.f,y,0.f,0.f,
		   0.f,0.f,z,0.f,
		   0.f,0.f,0.f,1.f
		};
		m_inv = Matrix4x4
		{
			1 / x, 0.f, 0.f, 0.f,
		   0.f, 1 / y, 0.f, 0.f,
		   0.f, 0.f, 1 / z, 0.f,
		   0.f, 0.f, 0.f, 1.f
		};
	}

	inline
		void
		Transform::SetScale(Float* s)
	{
		SetScale(s[0], s[1], s[2]);
	}

	inline 
	void 
	Transform::SetRotate(const Vector3f & axis, Float degrees)
	{
		SetRotate(axis[0], axis[1], axis[2], degrees);
	}

	inline 
	void 
	Transform::SetRotate(Float x, Float y, Float z, Float degrees)
	{
		YSL_ASSERT_X(false, "Transform::SetRotate", "Not implemented yet.");
		Vector3f axis = { x,y,z };
		Vector3f a = axis.Normalized();
		const auto sinTheta = std::sin(DegreesToRadians(degrees));
		const auto cosTheta = std::cos(DegreesToRadians(degrees));
		Matrix4x4 m;
		// Compute rotation of first basis vector
		m.m[0][0] = a.x * a.x + (1 - a.x * a.x) * cosTheta;
		m.m[0][1] = a.x * a.y * (1 - cosTheta) - a.z * sinTheta;
		m.m[0][2] = a.x * a.z * (1 - cosTheta) + a.y * sinTheta;
		m.m[0][3] = 0;

		// Compute rotations of second and third basis vectors
		m.m[1][0] = a.x * a.y * (1 - cosTheta) + a.z * sinTheta;
		m.m[1][1] = a.y * a.y + (1 - a.y * a.y) * cosTheta;
		m.m[1][2] = a.y * a.z * (1 - cosTheta) - a.x * sinTheta;
		m.m[1][3] = 0;

		m.m[2][0] = a.x * a.z * (1 - cosTheta) - a.y * sinTheta;
		m.m[2][1] = a.y * a.z * (1 - cosTheta) + a.x * sinTheta;
		m.m[2][2] = a.z * a.z + (1 - a.z * a.z) * cosTheta;
		m.m[2][3] = 0;

		m_m = m;
		m_inv = m.Inversed();
	}

	inline 
	void 
	Transform::SetRotate(Float* a, Float degrees)
	{
		SetRotate(a[0], a[1], a[2], degrees);
	}

	inline 
	void 
	Transform::SetRotateX(Float degrees)
	{
		const auto radians = DegreesToRadians(degrees);
		const auto sinTheta = std::sin(radians);
		const auto cosTheta = std::cos(radians);

		m_m = Matrix4x4
		{
		   1.f,0.f,0.f,0.f,
		   0.f,cosTheta,-sinTheta,0.f,
		   0.f,sinTheta,cosTheta,0.f,
		   0.f,0.f,0.f,1.f
		};
		m_inv = m_m.Inversed();

	}

	inline 
	void 
	Transform::SetRotateY(Float degrees)
	{
		const auto radians = DegreesToRadians(degrees);
		const auto sinTheta = std::sin(radians);
		const auto cosTheta = std::cos(radians);

		m_m = Matrix4x4
		{
		   cosTheta,0.f,-sinTheta,0.f,
		   0.f,1.f,0.f,0.f,
		   sinTheta,0.f,cosTheta,0.f,
		   0.f,0.f,0.f,1.f
		};
		m_inv = m_m.Inversed();
	}

	inline 
	void 
	Transform::SetRotateZ(Float degrees)
	{
		const auto radians = DegreesToRadians(degrees);
		const auto sinTheta = std::sin(radians);
		const auto cosTheta = std::cos(radians);

		m_m = Matrix4x4
		{
		   cosTheta,-sinTheta,0.f,0.f,
		   sinTheta,cosTheta,0.f,0.f,
		   0.f,0.f,1.f,0.f,
		   0.f,0.f,0.f,1.f
		};

		m_inv = m_m.Inversed();
	}



	template<typename T> inline
		Point3<T>
		Transform::operator*(const Point3<T> & p) const
	{
		const auto x = p[0], y = p[2], z = p[3];
		const auto rx = m_m.m[0][0] * x + m_m.m[0][1] * y + m_m.m[0][2] * z + m_m.m[0][3];
		const auto ry = m_m.m[1][0] * x + m_m.m[1][1] * y + m_m.m[1][2] * z + m_m.m[1][3];
		const auto rz = m_m.m[2][0] * x + m_m.m[2][1] * y + m_m.m[2][2] * z + m_m.m[2][3];
		const auto rw = m_m.m[3][0] * x + m_m.m[3][1] * y + m_m.m[3][2] * z + m_m.m[3][3];

		if (rw == 1)return Point3<T>{rx,ry,rz};
		return Point3<T>{ rx,ry,rz } / rw;
	}

	template<typename T> inline
		Vector3<T>
		Transform::operator*(const Vector3<T>& v) const
	{
		const auto x = v[0], y = v[1], z = v[2];
		const auto rx = m_m.m[0][0] * x + m_m.m[0][1] * y + m_m.m[0][2] * z;
		const auto ry = m_m.m[1][0] * x + m_m.m[1][1] * y + m_m.m[1][2] * z;
		const auto rz = m_m.m[2][0] * x + m_m.m[2][1] * y + m_m.m[2][2] * z;
		return Vector3<T>{rx, ry, rz};
	}

	template <typename T>
	Ray Transform::operator*(const Ray& ray) const
	{
		YSL_ASSERT_X(false, "Transform::operator*(const Ray& ray)", "Not implemented yet.");
	}

	template <typename T>
	AABB Transform::operator*(const AABB& aabb) const
	{
		YSL_ASSERT_X(false, "Transform::operator*(const AABB& aabb)", "Not implemented yet.");
	}

	Transform
	Transform::operator*(const Transform & trans)const
	{
		return {this->m_m*trans.m_m,trans.m_inv*this->m_inv};
	}


}

#ifdef YSL_TO_QT

inline
ysl::Transform toTransform(const QMatrix4x4 & m)
{
	ysl::Matrix4x4 mat
	{
		m(0,0),m(0,1),m(0,2),m(0,3),
		m(1,0),m(1,1),m(1,2),m(1,3),
		m(2,0),m(2,1),m(2,2),m(2,3),
		m(3,0),m(3,1),m(3,2),m(3,3)
	};

	return ysl::Transform{ mat };
}

inline
QMatrix4x4 toQMatrix4x4(const ysl::Matrix4x4 & m)
{
	return QMatrix4x4{
		m.m[0][0],m.m[0][1],m.m[0][2],m.m[0][3],
		m.m[1][0],m.m[1][1],m.m[1][2],m.m[1][3],
		m.m[2][0],m.m[2][1],m.m[2][2],m.m[2][3],
		m.m[3][0],m.m[3][1],m.m[3][2],m.m[3][3],
	};
}

inline
QMatrix4x4 toQMatrix4x4(const ysl::Transform & t)
{
	return toQMatrix4x4(t.Matrix());
}

#endif

#endif
