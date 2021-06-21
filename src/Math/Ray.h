#pragma once
#include "MathLib.h"

namespace Math {
	class Ray {
	public:
		Vector3 o;
		Vector3 d;
		Scalar max_T;
	};
}