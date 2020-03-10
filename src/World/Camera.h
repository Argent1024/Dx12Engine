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
			Matrix4 cameraTransform(DirectX::XMMatrixLookAtRH(Position, Target, WorldUp));
			Matrix4 view(DirectX::XMMatrixPerspectiveRH(2.0f, 2.0f, 1.0f, 10.0f));
			m_ViewProjective = Transform(view);
			
			//m_ViewProjective = Transform(view * cameraTransform);
			//m_InvTransform = Transform(DirectX::XMMatrixInverse(cameraTransform));
			
			Vector3 v1(0.5f, 0.5f, 0.0f);
			Vector3 tv1 = cameraTransform * v1;
			Vector3 pv1 = view * tv1;

			Vector3 v2(0.5f, -0.5f, 0.0f);
			Vector3 tv2 = cameraTransform * v2;
			Vector3 pv2 = m_ViewProjective(v2);
		}

		void UseCamera(Graphic::CommandList& commmandList) override;

	private:
		Transform m_ViewProjective;
		Transform m_InvTransform;
	};
}

