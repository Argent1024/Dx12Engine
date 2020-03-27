#pragma once
#include "Vector.h"

namespace Math 
{
	class BoundingBox 
	{
	public:
		bool CollideWith(const BoundingBox* box) const {
			return false;
		}

	private:
		Vector3 m_center;
		// Store x, y, z direction for the box
		// dir(z) = dir(x) X dir(y)
		// These vector also store length in it, which means length(m_x) != 1
		Vector3 m_x;
		Vector3 m_y;
		Vector3 m_z;
	};
}