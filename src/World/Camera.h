#pragma once

#include "MathCommon.h"
#include "Transform.h"
#include "Descriptor.h"

#include "CommandList.h"

namespace Game {
	using namespace Math;
	// ***IMPORTANT***
	// In DirectX's cpu code, vector's dimension is 1 * 4.
	// So a matrix(transform) M mutiply(apply to) a vector v should be written as v * M
	class Camera {
	public:
		Camera(const UINT width, const UINT height) :
			 m_aspectRatio(float(width) / (float)height),
			 m_Viewport(0.0f, 0.0f, static_cast<float>(width), static_cast<float>(height)),
			 m_ScissorRect(0, 0, static_cast<LONG>(width), static_cast<LONG>(height))
		{ }

		virtual void UseCamera(Graphic::CommandList& commandList, Transform ModelTransformation = Transform())
		{
			commandList.SetViewPorts(&m_Viewport);
			commandList.SetScissorRects(&m_ScissorRect);
		}

		void SetViewPort(CD3DX12_VIEWPORT newViewPort) { m_Viewport = newViewPort; }
		void SetScissorRect(CD3DX12_RECT newScissorRect) { m_ScissorRect = newScissorRect; } 

	protected:
		float m_aspectRatio;
		CD3DX12_VIEWPORT m_Viewport;
		CD3DX12_RECT m_ScissorRect;
	};


	class ProjectiveCamera : public Camera { 
	public:
		ProjectiveCamera(const UINT width, const UINT height)
			: Camera(width, height) {}

		ProjectiveCamera(const UINT width, const UINT height, 
			const Vector3& Position, const Vector3& Target, const Vector3& WorldUp)
			: Camera(width, height)
		{ LookAt(Position, Target, WorldUp); }

		inline void LookAt(const Vector3& Position, const Vector3& Target, const Vector3& WorldUp)
		{
			Matrix4 view(DirectX::XMMatrixLookAtRH(Position, Target, WorldUp));			
			Matrix4 proj(DirectX::XMMatrixPerspectiveFovRH(3.14f / 3.0f, m_aspectRatio, 1.0f, 1000.0f));
			m_ViewProjective = Transform(view * proj);
			
		}

		void UseCamera(Graphic::CommandList& commmandList, Transform model=Transform()) override;

		void CreateCBV();
	private:

		// CBV where transformations are stored
		std::shared_ptr<Graphic::ConstantBuffer> m_RootCBV;

		// world to view & projective
		Transform m_ViewProjective;
		//Transform m_InvTransform;
	};

	class OrthonormalCamera : public Camera {
	public:
		OrthonormalCamera(const UINT width, const UINT height)
			: Camera(width, height) {}

		OrthonormalCamera(const UINT width, const UINT height, 
			const Vector3& Position, const Vector3& Target, const Vector3& WorldUp)
			: Camera(width, height)
		{ LookAt(Position, Target, WorldUp); }

		inline void LookAt(const Vector3& Position, const Vector3& Target, const Vector3& WorldUp)
		{
			Matrix4 view(DirectX::XMMatrixLookAtRH(Position, Target, WorldUp));			
			Matrix4 orthonormal(DirectX::XMMatrixOrthographicRH(3.14f / 1.5f, m_aspectRatio, 1.0f, 1000.0f));
			m_ViewOrthormal = Transform(view * orthonormal);
			
		}
		
		void UseCamera(Graphic::CommandList& commmandList, Transform model=Transform()) override;

		void CreateCBV();
	
	private:	
		// CBV where transformations are stored
		std::shared_ptr<Graphic::ConstantBuffer> m_RootCBV;

		// world to view & orthonormal 
		Transform m_ViewOrthormal;
		// Transform m_InvTransform;
	};
}

