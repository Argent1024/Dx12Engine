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
		ProjectiveCamera(const UINT width, const UINT height)
			: Camera(width, height) {}

		ProjectiveCamera(const UINT width, const UINT height, 
			const Vector3& Position, const Vector3& Target, const Vector3& WorldUp): Camera(width, height)
		{ LookAt(Position, Target, WorldUp); }

		inline void LookAt(const Vector3& Position, const Vector3& Target, const Vector3& WorldUp)
		{
			Matrix4 view(DirectX::XMMatrixLookAtRH(Position, Target, WorldUp));
			// TODO fix view width & height 
			Matrix4 proj(DirectX::XMMatrixPerspectiveRH(10.0f, 10.0f, 0.1f, 10.0f));
			
			// ***IMPORTANT***
			// In DirectX, vector's dimension is 1 * 4.
			// So a matrix(transform) M mutiply(apply to) a vector v should be written as v * M
			m_ViewProjective = Transform(view * proj);
			//m_InvTransform = Transform(DirectX::XMMatrixInverse(cameraTransform));
		}

		void UseCamera(Graphic::CommandList& commmandList) override;

	private:
		// world to view & projective
		Transform m_ViewProjective;
		Transform m_InvTransform;
	};
}

