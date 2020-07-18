#pragma once

#include "Vector.h"
#include "Matrix3.h"
#include "Matrix4.h"

namespace Math {
	
	const Scalar PI = 3.1415926f;

	INLINE Vector3 CrossProduct(const Vector3& a, const Vector3& b) 
	{
		return Vector3(DirectX::XMVector3Cross(a, b));
	}

	INLINE Vector3 Normalize(const Vector3& v) 
	{
		return Vector3(DirectX::XMVector3Normalize(v));
	}

	INLINE Scalar Dot(const Vector3& a, const Vector3& b) 
	{
		return Scalar(DirectX::XMVector3Dot(a, b));
	}

	INLINE Scalar Length2(const Vector3& v) 
	{
		return Scalar(DirectX::XMVector3LengthSq(v));
	}
}