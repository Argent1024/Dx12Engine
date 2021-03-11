#pragma once

#include "Vector.h"
#include "Matrix3.h"
#include "Matrix4.h"
#include "Transform.h"

namespace Math {
	
	const float PI = 3.1415926f;
	const Scalar TOLERENCE = Scalar(0.01f);

	INLINE Scalar Abs(const Scalar& s) 
	{
		return Scalar(DirectX::XMVectorAbs(s));
	}

	INLINE Vector3 Abs(const Vector3& v) 
	{
		return Vector3(DirectX::XMVectorAbs(v));
	}

	INLINE Vector3 CrossProduct(const Vector3& a, const Vector3& b) 
	{
		return Vector3(DirectX::XMVector3Cross(a, b));
	}

	INLINE Vector3 Normalize(const Vector3& v) 
	{
		return Vector3(DirectX::XMVector3Normalize(v));
	}

	INLINE Vector2 Normalize(const Vector2& v) 
	{
		return Vector2(DirectX::XMVector3Normalize(v));
	}

	INLINE Scalar Dot(const Vector3& a, const Vector3& b) 
	{
		return Scalar(DirectX::XMVector3Dot(a, b));
	}

	INLINE Scalar Dot(const Vector2& a, const Vector2& b) 
	{
		return Scalar(DirectX::XMVector3Dot(a, b));
	}

	INLINE Scalar Length2(const Vector3& v) 
	{
		return Scalar(DirectX::XMVector3LengthSq(v));
	}

	INLINE Scalar Length2(const Vector2& v) 
	{
		return Scalar(DirectX::XMVector2LengthSq(v));
	}

	INLINE Scalar Length(const Vector3& v) 
	{
		return Scalar((DirectX::XMVector3Length(v)));
	}

	INLINE Scalar Length(const Vector2& v) 
	{
		return Scalar((DirectX::XMVector2Length(v)));
	}

	template <class T>
	INLINE T Sqrt(const T& v) {
		return T(DirectX::XMVectorSqrt(v));
	}

	template <class T>
	INLINE T Exp(const T& v) {
		return T(DirectX::XMVectorExpE(v));
	}

	template <class T>
	INLINE T Pow(const T& v, const T& p) {
		return T(DirectX::XMVectorPow(v, p));
	}
}