#pragma once

#include "Matrix3.h"

namespace Math {

	class Matrix4 
	{
	public:
		INLINE Matrix4() {}
		INLINE explicit Matrix4(const XMMATRIX& mat) { m_mat = mat; }
		
		INLINE Matrix4(const Matrix3& mat)
		{
			m_mat.r[0] = XMVectorSetW(mat.GetX(), 0.f);
			m_mat.r[1] = XMVectorSetW(mat.GetY(), 0.f);
			m_mat.r[2] = XMVectorSetW(mat.GetZ(), 0.f);
			m_mat.r[3] = XMVectorSet(0.f, 0.f, 0.f, 1.f);
		}

		INLINE Matrix4(const Matrix3& mat, Vector3 v)
		{
			m_mat.r[0] = XMVectorSetW(mat.GetX(), 0.f);
			m_mat.r[1] = XMVectorSetW(mat.GetY(), 0.f);
			m_mat.r[2] = XMVectorSetW(mat.GetZ(), 0.f);
			m_mat.r[3] = XMVectorSetW(v, 1.f);
		}

		INLINE Matrix4(const Vector3& x, const Vector3& y, const Vector3& z, const Vector3& w) 
		{
			m_mat.r[0] = XMVectorSetW(x, 0.f);
			m_mat.r[1] = XMVectorSetW(y, 0.f);
			m_mat.r[2] = XMVectorSetW(z, 0.f);
			m_mat.r[3] = XMVectorSetW(w, 1.f);
		}

		INLINE Matrix4(const Matrix4& mat) { m_mat = mat.m_mat; }
		INLINE explicit Matrix4(IdentityTag) { m_mat = XMMatrixIdentity(); }
		INLINE explicit Matrix4(ZeroTag) { m_mat.r[0] = m_mat.r[1] = m_mat.r[2] = m_mat.r[3] = XMVectorZero(); }

		INLINE const Matrix3& Get3x3() const { return (const Matrix3&)*this; }

		INLINE const Vector3 GetX() { return Vector3(m_mat.r[0]); }
		INLINE const Vector3 GetY() { return Vector3(m_mat.r[1]); }
		INLINE const Vector3 GetZ() { return Vector3(m_mat.r[2]); }
		INLINE const Vector3 GetW() { return Vector3(m_mat.r[3]); }

		INLINE Matrix4 operator* (const Matrix4& mat) const { return Matrix4(XMMatrixMultiply(m_mat, mat.m_mat)); }
		// NOTICE! XMVector3Transform ignore w in the result! w maybe not equal to 1.0
		INLINE Vector3 operator* (const Vector3& vec) const { return Vector3(XMVector3Transform(vec, m_mat)); } 
		
		INLINE operator XMMATRIX() { return m_mat; }
	protected:
		XMMATRIX m_mat;
	};

}