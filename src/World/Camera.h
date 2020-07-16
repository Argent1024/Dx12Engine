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
		
		virtual void UseCamera(Graphic::CommandList& commandList)
		{
			commandList.SetViewPorts(&m_Viewport);
			commandList.SetScissorRects(&m_ScissorRect);
		}

		// Update view and ToScreen
		virtual void Look() = 0;

		// helper for user input
		inline void RotateYaw(float angle) 
		{ 
			Transform R(XMMatrixRotationAxis(m_WorldUp, angle));
			m_Direction = Normalize(R * m_Direction);
		}

		inline void RotateRoll(float angle) 
		{
			Transform R(XMMatrixRotationAxis(m_Direction, angle));
			m_WorldUp = Normalize(R * m_WorldUp);
		}

		inline void RotatePitch(float angle) 
		{
			Vector3 axis = CrossProduct(m_Direction, m_WorldUp);
			Transform R(XMMatrixRotationAxis(axis, angle));
			m_Direction = Normalize(R * m_Direction);
			m_WorldUp = Normalize(R * m_WorldUp);
		}

		//********* Setting camera's data ***********//
		inline const Transform& GetView() const { return m_View; }
		inline const Transform& GetToScreen() const { return m_ToScreen; }
		inline void SetNearZ(float z) { m_nearZ = z; }
		inline void SetFarZ(float z) { m_farZ = z; }
		inline void SetPosition(Vector3& pos) { m_Position = pos; }
		inline void SetDirection(Vector3& dir) { m_Direction = dir; }
		inline void SetWorldUp(Vector3& up) { m_WorldUp = up;}

		void LookAt(const Vector3& Position, const Vector3& Target, const Vector3& WorldUp) 
		{
			m_Position = Position;
			m_Direction = Target - m_Position;
			m_WorldUp = WorldUp;
			Look();
		}

		void LookTo(const Vector3& Position, const Vector3& Direction, const Vector3& WorldUp) 
		{
			m_Position = Position;
			m_Direction = Direction;
			m_WorldUp = WorldUp;
			Look();
		}

		void SetViewPort(CD3DX12_VIEWPORT newViewPort) { m_Viewport = newViewPort; }
		void SetScissorRect(CD3DX12_RECT newScissorRect) { m_ScissorRect = newScissorRect; } 

	protected:
		// Helper for gpu use
		Transform m_View;
		Transform m_ToScreen; // Projective or Orthnomal
		
		CD3DX12_VIEWPORT m_Viewport;
		CD3DX12_RECT m_ScissorRect;

		// Camera's data
		float m_aspectRatio;
		float m_nearZ = 0.01f;
		float m_farZ = 50.0f;
		Vector3 m_Position = Vector3(kZero);
		Vector3 m_Direction = Vector3(kZUnitVec);
		Vector3 m_WorldUp = Vector3(kYUnitVec);
	};

	
	class ProjectiveCamera : public Camera { 
	public:
		ProjectiveCamera(const UINT width, const UINT height)
			: Camera(width, height) {}

		ProjectiveCamera(const UINT width, const UINT height, 
			const Vector3& Position, const Vector3& Target, const Vector3& WorldUp)
			: Camera(width, height)
		{ LookAt(Position, Target, WorldUp); }

		inline void Look() override
		{
			m_View = Transform(Matrix4(DirectX::XMMatrixLookToRH(m_Position, m_Direction, m_WorldUp)));
			m_ToScreen = Transform(Matrix4(DirectX::XMMatrixPerspectiveFovRH(m_FOVAngleY, m_aspectRatio, m_nearZ, m_farZ)));
		}

		inline void SetViewAngle(float angle) { m_FOVAngleY = angle; }

	private:
		float m_FOVAngleY = 3.14f / 6.0f;
	};

	class OrthonormalCamera : public Camera {
	public:
		OrthonormalCamera(const UINT width, const UINT height)
			: Camera(width, height) {}

		OrthonormalCamera(const UINT width, const UINT height, 
			const Vector3& Position, const Vector3& Target, const Vector3& WorldUp)
			: Camera(width, height)
		{ LookAt(Position, Target, WorldUp); }

		inline void Look() override
		{
			m_View = Transform(Matrix4(DirectX::XMMatrixLookToLH(m_Position, m_Direction, m_WorldUp)));		
			m_ToScreen = Transform(Matrix4(DirectX::XMMatrixOrthographicLH(10.0, 10.0, m_nearZ, m_farZ)));
		}
	};
}

