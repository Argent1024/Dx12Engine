#pragma once

#include "Texture.h"
#include "DepthBuffer.h"
#include "Camera.h"

namespace Game {

	enum LightType {
		DIRECTION_LIGHT,
		POINT_LIGHT,
		SPOT_LIGHT
	};

	using namespace Math;

	struct LightData
	{
		Vector3 strength;
		Vector3 pos;
		Vector3 direction;
	};

	struct LightState
	{
		XMFLOAT4 strength;
		XMFLOAT4 pos;		// Point / Spot light
		XMFLOAT4 direction;	// Dir / Spot light
		XMFLOAT4X4 view;	
		XMFLOAT4X4 proj;	// Projective // orthnormal matrix
	};

	class Light {

	public:
		Light() {}

		inline void SetState(LightState* state) { m_state = state; }

		inline LightState* GetLightState() { return m_state; }

		virtual Graphic::DepthBuffer& GetDepthBuffer() = 0;

		virtual Graphic::Texture* GetDepthTexture() = 0;
		
		virtual void SetLightData(const LightData& data) = 0;

		// Put Data into m_state
		virtual void UseLight() = 0;

		virtual Camera& GetCamera() = 0;

	private:
		LightState* m_state;
	};


	class DirectionLight : public Light 
	{
	public:
		DirectionLight(UINT width=256, UINT height=256);

		inline Graphic::Texture* GetDepthTexture() override { return m_DepthBuffer.GetTexture(); }

		Graphic::DepthBuffer& GetDepthBuffer() override { return m_DepthBuffer; }

		void SetLightData(const LightData& data) override;

		inline Camera& GetCamera() override { return m_Camera; }

		void UseLight() override;

	private:
		Graphic::DepthBuffer m_DepthBuffer;
		OrthonormalCamera m_Camera;
	};


	class SpotLight : public Light
	{
	public:
		SpotLight(Vector3 pos, Vector3 dir, UINT width=256, UINT height=256);

		inline Graphic::Texture* GetDepthTexture() override { return m_DepthBuffer.GetTexture(); }

		Graphic::DepthBuffer& GetDepthBuffer() override { return m_DepthBuffer; }

		void SetLightData(const LightData& data) override;

		inline Camera& GetCamera() override { return m_Camera; }

		void UseLight() override;

	private:
		Graphic::DepthBuffer m_DepthBuffer;
		ProjectiveCamera m_Camera;
	};

}