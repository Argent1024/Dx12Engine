#include "Light.h"
#include "GraphicCore.h"


namespace Game {
	
	DirectionLight::DirectionLight(DirectionLightState state, UINT width=256, UINT height=256)
		: Light(state), m_DepthBuffer(width, height), m_Camera(width, height)
	{
		// Initialize dsv
		// Should create srv else where
		m_DepthBuffer.Initialize(Graphic::TEXTURE_DSV & Graphic::TEXTURE_SRV);
	}
}