#pragma once

#include <DirectXMath.h>
#include <intrin.h>
#include <ostream>

#define INLINE __forceinline

namespace Math {
	using namespace DirectX;

	enum ZeroTag {kZero, kOrigin};
	enum IdentityTag {kOne, kIdentity};
	enum XUnitVec { kXUnitVec };
	enum YUnitVec { kYUnitVec };
	enum ZUnitVec { kZUnitVec };
}