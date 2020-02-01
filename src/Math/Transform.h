#pragma once

#include "Matrix4.h"


namespace Math {
	class Transform {
	public:
		INLINE Transform() : m_mat(kIdentity) {}
		INLINE explicit Transform(const Vector3& x, const Vector3& y, const Vector3& z, const Vector3& w)
			: m_mat(x, y, z, w) {}
		INLINE explicit Transform(const Matrix3& mat, Vector3 v): m_mat(mat, v) {}

		INLINE const Matrix3& GetRotation() { return m_mat.Get3x3(); }
		INLINE Vector3 GetTranslation() { return m_mat.GetW(); }
 
	private:
		Matrix4 m_mat;
	};
}