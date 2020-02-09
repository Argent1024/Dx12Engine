#pragma once

#include "GraphicCore.h"
#include "CommandQueue.h"
#include "../World/Camera.h"
#include "../World/GObject.h"


namespace Samples {
	using namespace Graphic;
	using namespace Game;

	struct Vertex
    {
        XMFLOAT3 position;
        XMFLOAT2 texcoor;
    };


	class TextureTestSample : public GraphicCore {
	public:
		TextureTestSample(UINT t_width, UINT t_height, 
					const std::wstring shaderPath=L"D:\\work\\tEngine\\Shaders\\ray.hlsl",
					LPCTSTR t_title=L"playground")
			: GraphicCore(t_width, t_height, t_title), m_shaderPath(m_shaderPath), m_Camera(t_width, t_height) {}
		
		void Init(const HWND m_appHwnd) override;
		void Render() override;
	
	private:
		const std::wstring m_shaderPath;

		std::shared_ptr<VertexBuffer> m_vertexBuffer;
		std::shared_ptr<IndexBuffer> m_indexBuffer;

		SwapChain* m_swapChain;
		ptrRootSigature m_rootSignature;
		ptrPSO m_GraphicPSO;

		ptrGPUMem m_GPUmem;
		ptrMesh m_Mesh;
		ptrTexture m_texture;
		ptrMaterial m_Material;
		GObject m_textureObject;

		Camera m_Camera;

		void CreatSwapChain(const HWND t_appHwnd) {
			m_swapChain = new SwapChain(t_appHwnd, m_width, m_height);
			m_swapChain->Initialize(GraphicsCommandManager.GetCommadnQueue());
		}
	};

}