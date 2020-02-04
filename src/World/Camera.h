#pragma once

#include "MathCommon.h"
#include "Transform.h"
#include "Descriptor.h"

#include "CommandList.h"

namespace Game {
	using namespace Math;

	class Camera {
	public:
		Camera(const UINT width, const UINT height) :
			 m_Viewport(0.0f, 0.0f, static_cast<float>(width), static_cast<float>(height)),
			 m_ScissorRect(0, 0, static_cast<LONG>(width), static_cast<LONG>(height))
		{ }

		virtual void UseCamera(Graphic::CommandList& commandList) 
		{
			commandList.SetViewPorts(&m_Viewport);
			commandList.SetScissorRects(&m_ScissorRect);
		}
		
		void SetViewPort(CD3DX12_VIEWPORT newViewPort) { m_Viewport = newViewPort; }
		void SetScissorRect(CD3DX12_RECT newScissorRect) { m_ScissorRect = newScissorRect; } 

	protected:
		CD3DX12_VIEWPORT m_Viewport;
		CD3DX12_RECT m_ScissorRect;
	};


	class ProjectiveCamera : public Camera { 
	public:
		ProjectiveCamera(const UINT width, const UINT height, 
			const Vector3& Position, const Vector3& ViewDir, const Vector3& WorldUp): Camera(width, height)
		{ LookAt(Position, ViewDir, WorldUp); }

		inline void LookAt(const Vector3& Position, const Vector3& ViewDir, const Vector3& WorldUp)
		{
			Vector3 z = -ViewDir;
			Vector3 x = Normalize(CrossProduct(WorldUp, z));
			Vector3 y = CrossProduct(z, x);
			m_ViewTransform = Transform(x, y ,z, -Position);
		}

		void UseCamera(Graphic::CommandList& commmandList) override;

	private:
		Transform m_ViewTransform;
	};
}
