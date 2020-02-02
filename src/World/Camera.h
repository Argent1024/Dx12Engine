#pragma once

#include "MathCommon.h"
#include "Transform.h"
#include "Descriptor.h"

#include "CommandList.h"

namespace Game {
	using namespace Math;

	class Camera
	{ 

	public:
		Camera(const Vector3& Position, const Vector3& ViewDir, const Vector3& WorldUp)
		{ LookAt(Position, ViewDir, WorldUp); }

		inline void LookAt(const Vector3& Position, const Vector3& ViewDir, const Vector3& WorldUp)
		{
			Vector3 z = -ViewDir;
			Vector3 x = Normalize(CrossProduct(WorldUp, z));
			Vector3 y = CrossProduct(z, x);
			m_ViewTransform = Transform(x, y ,z, -Position);
		}

		void UseCamera(Graphic::CommandList& commmandList);

	private:
		CD3DX12_VIEWPORT m_Viewport;
		CD3DX12_RECT m_ScissorRect;

		Transform m_ViewTransform;
	};
}

