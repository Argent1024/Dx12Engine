#pragma once

#include "Texture.h"
#include "DepthBuffer.h"
#include "Camera.h"

namespace Game {

	enum LightType {
		DIRECTION,
		POINT
	};

	using namespace Math;

	template<class LightState>
	class Light {

	public:
		Light() {}

		Light(LightState state) : m_state(state) {}

		inline void SetState(LightState state) { m_state = state; }

		inline LightState& GetLightState() const { return m_state; }

		virtual Graphic::DepthBuffer& GetDepthBuffer() = 0;

		virtual Graphic::Texture* GetDepthTexture() = 0;

		virtual Camera& GetCamera() = 0;

	private:
		LightState m_state;
	};


	struct DirectionLightState
	{
		XMFLOAT4 direction;
		XMFLOAT4 radiance;
		XMFLOAT4X4 view;
		XMFLOAT4X4 orthnormal;
	};

	class DirectionLight : public Light<DirectionLightState> 
	{
	public:
		DirectionLight(DirectionLightState state, UINT width=256, UINT height=256);

		inline Graphic::Texture* GetDepthTexture() override { return m_DepthBuffer.GetTexture(); }

		inline Camera& GetCamera() override { return m_Camera; }

		Graphic::DepthBuffer& GetDepthBuffer() override { return m_DepthBuffer; }

	private:
		Graphic::DepthBuffer m_DepthBuffer;
		OrthonormalCamera m_Camera;
	};

}