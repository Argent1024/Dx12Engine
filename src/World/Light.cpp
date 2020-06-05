#include "Light.h"
#include "GraphicCore.h"


namespace Game {
	
	DirectionLight::DirectionLight(UINT width, UINT height)
		: m_DepthBuffer(width, height), m_Camera(width, height)
	{
		// Initialize dsv
		// Should create srv else where
		m_DepthBuffer.Initialize(Graphic::TEXTURE_DSV & Graphic::TEXTURE_SRV);

		// Default Camera Location
		m_Camera.LookAt(Vector3(0.f, 1.f, -1.f), Vector3(0.f,0.f,0.f), Vector3(0.f, 1.f, 0.f));
	}

	void DirectionLight::UseLight()
	{
		

	}


	SpotLight::SpotLight(Vector3 pos, Vector3 dir, UINT width, UINT height)
		: m_DepthBuffer(width, height), m_Camera(width, height)
	{
		// Initialize dsv
		// Should create srv else where
		m_DepthBuffer.Initialize(Graphic::TEXTURE_DSV & Graphic::TEXTURE_SRV);
		
		m_Camera.LookAt(pos, pos + dir, Vector3(0.f, 1.f, 0.f));
	}

	void SpotLight::UseLight()
	{
		const Transform& view = m_Camera.GetView();
		const Transform& proj = m_Camera.GetToScreen();

	}
}