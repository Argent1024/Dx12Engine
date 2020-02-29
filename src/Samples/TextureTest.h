#pragma once

#include "GraphicCore.h"
#include "CommandQueue.h"
#include "../World/Camera.h"
#include "../World/GObject.h"


namespace Samples {
	using namespace Graphic;
	using namespace Game;

	class TextureTestSample : public GraphicCore {
	public:
		TextureTestSample(UINT t_width, UINT t_height, LPCTSTR t_title=L"playground")
			: GraphicCore(t_width, t_height, t_title), 
			m_Camera(t_width, t_height)
		{}
		
		void Init(const HWND m_appHwnd) override;
		void Render() override;
	
	private:
		const std::wstring m_shaderPath = L"D:\\work\\tEngine\\Shaders\\shaders.hlsl";

		SwapChain* m_swapChain;
		ptrRootSigature m_rootSignature;
		ptrPSO m_GraphicPSO;

		std::shared_ptr<TriangleMesh> m_Mesh;
		ptrTexture m_texture;
		ptrMaterial m_Material;
		GObject m_textureObject;

		Camera m_Camera;
		//ProjectiveCamera m_Camera;

		void CreatSwapChain(const HWND t_appHwnd) 
		{
			m_swapChain = new SwapChain(t_appHwnd, m_width, m_height);
			m_swapChain->Initialize(GraphicsCommandManager.GetCommadnQueue());
		}
	};

}