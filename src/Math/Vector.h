#pragma once

#include "MathUtility.h"
#include "Scalar.h"

namespace Math 
{
	class Vector3 {
	protected:
		XMVECTOR m_vec;
	public:
		Vector3() {}

		INLINE Vector3(float x) { m_vec = XMVectorReplicate(x); }
		INLINE Vector3(float x, float y, float z) { m_vec = XMVectorSet(x, y, z, 1.f); }
		INLINE Vector3(const Vector3& v) { m_vec = v; }
		INLINE Vector3(const Scalar& s) { m_vec = s; }
		INLINE explicit Vector3(const XMVECTOR v) { m_vec = v; }
		INLINE explicit Vector3(ZeroTag) { m_vec = XMVectorSet(0.f, 0.f, 0.f, 1.f);}
		INLINE explicit Vector3(XUnitVec) { m_vec = XMVectorSet(1.f, 0.f, 0.f, 1.f);}
		INLINE explicit Vector3(YUnitVec) { m_vec = XMVectorSet(0.f, 1.f, 0.f, 1.f); }
		INLINE explicit Vector3(ZUnitVec) { m_vec = XMVectorSet(0.f, 0.f, 1.f, 1.f); }


		INLINE operator XMVECTOR() const { return m_vec; }

		INLINE Scalar GetX() const { return Scalar(XMVectorSplatX(m_vec)); }
		INLINE Scalar GetY() const { return Scalar(XMVectorSplatY(m_vec)); }
		INLINE Scalar GetZ() const { return Scalar(XMVectorSplatZ(m_vec)); }

		INLINE void SetX(Scalar x) { m_vec = XMVectorPermute<4, 1, 2, 3>(m_vec, x); }
		INLINE void SetY(Scalar y) { m_vec = XMVectorPermute<0, 5, 2, 3>(m_vec, y); }
		INLINE void SetZ(Scalar z) { m_vec = XMVectorPermute<0, 1, 6, 3>(m_vec, z); }


		INLINE Vector3 operator- () const { return Vector3(XMVectorNegate(m_vec)); }
		INLINE Vector3 operator+ (Vector3 v2) const { return Vector3(XMVectorAdd(m_vec, v2)); }
		INLINE Vector3 operator- (Vector3 v2) const { return Vector3(XMVectorSubtract(m_vec, v2)); }
		INLINE Vector3 operator* (Vector3 v2) const { return Vector3(XMVectorMultiply(m_vec, v2)); }
		INLINE Vector3 operator/ (Vector3 v2) const { return Vector3(XMVectorDivide(m_vec, v2)); }
		INLINE Vector3 operator* (Scalar  v2) const { return *this * Vector3(v2); }
		INLINE Vector3 operator/ (Scalar  v2) const { return *this / Vector3(v2); }
		INLINE Vector3 operator* (float  v2) const { return *this * Scalar(v2); }
		INLINE Vector3 operator/ (float  v2) const { return *this / Scalar(v2); }

		INLINE Vector3& operator += (Vector3 v) { *this = *this + v; return *this; }
		INLINE Vector3& operator -= (Vector3 v) { *this = *this - v; return *this; }
		INLINE Vector3& operator *= (Vector3 v) { *this = *this * v; return *this; }
		INLINE Vector3& operator /= (Vector3 v) { *this = *this / v; return *this; }

		INLINE friend Vector3 operator* (Scalar  v1, Vector3 v2) { return Vector3(v1) * v2; }
		INLINE friend Vector3 operator/ (Scalar  v1, Vector3 v2) { return Vector3(v1) / v2; }
		INLINE friend Vector3 operator* (float   v1, Vector3 v2) { return Scalar(v1) * v2; }
		INLINE friend Vector3 operator/ (float   v1, Vector3 v2) { return Scalar(v1) / v2; }
	};

	
}