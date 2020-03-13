#pragma once

#include "Matrix4.h"


namespace Math {
	class Transform {
	public:
		INLINE Transform() : m_mat(kIdentity) {}
		INLINE explicit Transform(const Vector3& x, const Vector3& y, const Vector3& z, const Vector3& w)
			: m_mat(x, y, z, w) {}
		INLINE explicit Transform(const Matrix3& mat, Vector3 v): m_mat(mat, v) {}
		INLINE explicit Transform(const Matrix4& mat) : m_mat(mat) {}

		INLINE const Matrix3& GetRotation() { return m_mat.Get3x3(); }
		INLINE Vector3 GetTranslation() { return m_mat.GetW(); }
		
		// Apply transformation to Vector3
		INLINE Vector3 operator()(const Vector3& v) const { return m_mat * v; }

		INLINE operator XMMATRIX() { return m_mat; }
		
		// Transformation multiply
		INLINE Transform operator*(const Transform& t) const { return Transform(m_mat * t.m_mat); }
	private:
		Matrix4 m_mat;
	};
}