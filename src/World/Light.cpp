#include "Light.h"
#include "GraphicCore.h"


namespace Game {
	
	DirectionLight::DirectionLight(DirectionLightState state, UINT width, UINT height)
		: Light(state), m_DepthBuffer(width, height), m_Camera(width, height)
	{
		// Initialize dsv
		// Should create srv else where
		m_DepthBuffer.Initialize(Graphic::TEXTURE_DSV & Graphic::TEXTURE_SRV);

		// Default Camera Location
		m_Camera.LookAt(Vector3(0.f, 1.f, -1.f), Vector3(0.f,0.f,0.f), Vector3(0.f, 1.f, 0.f));
	}
}