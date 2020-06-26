#include "Light.h"
#include "GraphicCore.h"


namespace Game {
	
	DirectionLight::DirectionLight(UINT width, UINT height)
		:  m_DepthBuffer(width, height, Graphic::TEXTURE_DSV | Graphic::TEXTURE_SRV), 
		   m_Camera(width, height)
	{ }

	void DirectionLight::Initialize(LightState* state) 
	{
		m_state = state;
		// Initialize dsv
		// Should create srv else where
		m_DepthBuffer.Initialize();

	}

	void DirectionLight::UseLight()
	{
		

	}

	void DirectionLight::SetLightData(const LightData& data) 
	{
		assert(m_state != nullptr && "Haven't init yet");
		XMStoreFloat4(&m_state->strength, data.strength);
		XMStoreFloat4(&m_state->direction, data.direction);

		m_Camera.LookAt(data.pos, data.pos + data.direction, Vector3(0.0f, 1.0f, 0.0f));

		const Transform& view = m_Camera.GetView();
		const Transform& orth = m_Camera.GetToScreen();

		XMStoreFloat4x4(&m_state->view, view);
		XMStoreFloat4x4(&m_state->proj, orth);
	}


	SpotLight::SpotLight(UINT width, UINT height)
		: m_DepthBuffer(width, height, Graphic::TEXTURE_DSV | Graphic::TEXTURE_SRV), 
		  m_Camera(width, height)
	{ }

	void SpotLight::Initialize(LightState* state) 
	{
		m_state = state;
		// Initialize dsv
		// Should create srv else where
		m_DepthBuffer.Initialize();
		
	}

	void SpotLight::UseLight()
	{
		const Transform& view = m_Camera.GetView();
		const Transform& proj = m_Camera.GetToScreen();

	}

	void SpotLight::SetLightData(const LightData& data) 
	{
		assert(m_state != nullptr && "Haven't init yet");

	}
}