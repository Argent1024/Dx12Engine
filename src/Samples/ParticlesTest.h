#pragma once
#include "GraphicCore.h"
#include "../World/Camera.h"
#include "../World/GObject.h"


namespace Samples {

	using namespace Graphic;
	using namespace Game;

	class ParticleTest : public GraphicCore {
	public:
		ParticleTest(UINT t_width, UINT t_height,  LPCTSTR t_title=L"playground") 
			: GraphicCore(t_width, t_height, t_title), m_Camera(t_width, t_height) {}

		void Init(const HWND m_appHwnd) override;
		void Render() override;

	private:
		Camera m_Camera;
		static const std::wstring shaderPath=L"D:\\work\\tEngine\\Shaders\\particle.hlsl";
	};
}

