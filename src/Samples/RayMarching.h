#pragma once

#include "GraphicCore.h"
#include "CommandQueue.h"
#include "../World/Camera.h"
#include "../World/GObject.h"

namespace Samples {
	
	using namespace Graphic;
	using namespace Game;

	// helper struct for passing root signature constants
	struct ScreenConstantData 
	{
		XMFLOAT4 ScreenSize;
	};

	class RayMarchingPSO : public GraphicsPSO {
	public:
		void Initialize() override 
		{
			assert(m_rootSignature != nullptr);
			m_psoDesc.pRootSignature = m_rootSignature;

			ComPtr<ID3DBlob> VS;
			ComPtr<ID3DBlob> PS;
			const std::wstring path = L"D:\\work\\tEngine\\Shaders\\ray.hlsl";
			ThrowIfFailed(D3DCompileFromFile(path.c_str(), nullptr, nullptr, "VSMain", "vs_5_0", CompileFlags, 0, &VS, nullptr));
			ThrowIfFailed(D3DCompileFromFile(path.c_str(), nullptr, nullptr, "PSMain", "ps_5_0", CompileFlags, 0, &PS, nullptr));

			// Input for vertex 
			D3D12_INPUT_ELEMENT_DESC inputElementDescs[] =
			{
					{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
					{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
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

	class RayMarchingRootSignature : public RootSignature {
	public:
		void Initialize() override;
	};

	class RayMarching : public GraphicCore {
	public:
		RayMarching(UINT t_width, UINT t_height, 
					const std::wstring shaderPath=L"D:\\work\\tEngine\\Shaders\\ray.hlsl",
					LPCTSTR t_title=L"playground") 
			: GraphicCore(t_width, t_height, t_title), m_shaderPath(m_shaderPath), m_Camera(t_width, t_height) {}

		void Init(const HWND m_appHwnd) override;
		void Render() override;

	private :
		const std::wstring m_shaderPath;

		SwapChain* m_swapChain;
		ptrRootSignature m_rootSignature;
		ptrPSO m_GraphicPSO;

		std::shared_ptr<TriangleMesh> m_Mesh;
		ptrMaterial m_Material;
		GObject m_rayMarchScreen;
		Camera m_Camera;

		void CreateGameObject();
	};
}