#pragma once

#include "GraphicCore.h"
#include "CommandQueue.h"
#include "../World/Camera.h"
#include "../World/GObject.h"


namespace Samples {
	using namespace Graphic;
	using namespace Game;

	class TextureTestPSO : public GraphicsPSO {
	public:
		void Initialize() 
		{
			assert(m_rootSignature != nullptr);
			m_psoDesc.pRootSignature = m_rootSignature;

			ComPtr<ID3DBlob> VS;
			ComPtr<ID3DBlob> PS;
			const std::wstring path = L"D:\\work\\tEngine\\Shaders\\TextureTest.hlsl";
			ThrowIfFailed(D3DCompileFromFile(path.c_str(), nullptr, nullptr, "VSMain", "vs_5_0", CompileFlags, 0, &VS, nullptr));
			ThrowIfFailed(D3DCompileFromFile(path.c_str(), nullptr, nullptr, "PSMain", "ps_5_0", CompileFlags, 0, &PS, nullptr));

			// Input for vertex 
			D3D12_INPUT_ELEMENT_DESC inputElementDescs[] =
			{
					{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
					{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
			};

			this->SetVertexShader(CD3DX12_SHADER_BYTECODE(VS.Get()));
			this->SetPixelShader(CD3DX12_SHADER_BYTECODE(PS.Get()));
			this->SetTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
			this->SetInoutLayout(_countof(inputElementDescs), inputElementDescs);
			
			// Configrations
			CD3DX12_DEPTH_STENCIL_DESC depthStencilDesc(D3D12_DEFAULT);
			depthStencilDesc.DepthEnable = FALSE;
			depthStencilDesc.StencilEnable = FALSE;
			this->SetDepthStencilState(depthStencilDesc);

			this->SetBlendState();
			this->SetRasterState();

			this->SetStuffThatIdontKnowYet();

			this->CreatePSO();
		}

	};

	class TextureTestSample : public GraphicCore {
	public:
		TextureTestSample(UINT t_width, UINT t_height, LPCTSTR t_title=L"playground")
			: GraphicCore(t_width, t_height, t_title), 
			  m_Camera(t_width, t_height, Vector3(0.0, 0.0, 1.0), Vector3(0.0, 0.0, 0.0), Vector3(0.0, 1.0, 0.0))
		{}
		
		void Init(const HWND m_appHwnd) override;
		void Render() override;
	
	private:
		SwapChain* m_swapChain;
		ptrRootSignature m_rootSignature;
		ptrPSO m_GraphicPSO;

		std::shared_ptr<TriangleMesh> m_Mesh;
		ptrTexture m_texture;
		ptrMaterial m_Material;
		GObject m_textureObject;

		//Camera m_Camera;
		ProjectiveCamera m_Camera;

		void CreatSwapChain(const HWND t_appHwnd) 
		{
			m_swapChain = new SwapChain(t_appHwnd, m_width, m_height);
			m_swapChain->Initialize(GraphicsCommandManager.GetCommadnQueue());
		}
	};

}