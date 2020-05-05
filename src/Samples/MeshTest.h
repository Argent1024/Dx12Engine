#pragma once

#include "GraphicCore.h"
#include "CommandQueue.h"
#include "../World/Camera.h"
#include "../World/GObject.h"
#include "../World/Scene.h"

namespace Samples {
	using namespace Graphic;
	using namespace Game;

	class MeshTestPSO : public GraphicsPSO {
	public:
		void Initialize() 
		{
			assert(m_rootSignature != nullptr);
			m_psoDesc.pRootSignature = m_rootSignature;

			ComPtr<ID3DBlob> VS;
			ComPtr<ID3DBlob> PS;
			const std::wstring path = L"D:\\work\\tEngine\\Shaders\\MeshTest.hlsl";
			ThrowIfFailed(D3DCompileFromFile(path.c_str(), nullptr, nullptr, "VSMain", "vs_5_0", CompileFlags, 0, &VS, nullptr));
			ThrowIfFailed(D3DCompileFromFile(path.c_str(), nullptr, nullptr, "PSMain", "ps_5_0", CompileFlags, 0, &PS, nullptr));

			D3D12_INPUT_ELEMENT_DESC  inputElementDescs[] =
			{
				{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,  0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
				{ "NORMAL",	  0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
				{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,       0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
			};

			this->SetVertexShader(CD3DX12_SHADER_BYTECODE(VS.Get()));
			this->SetPixelShader(CD3DX12_SHADER_BYTECODE(PS.Get()));
			this->SetTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
			this->SetInoutLayout(_countof(inputElementDescs), inputElementDescs);

			// configrations
			CD3DX12_DEPTH_STENCIL_DESC depthStencilDesc(D3D12_DEFAULT);
			depthStencilDesc.DepthEnable = TRUE;
			depthStencilDesc.StencilEnable = FALSE;
			this->SetDepthStencilState(depthStencilDesc);
			this->SetBlendState();
			this->SetRasterState();

			this->SetStuffThatIdontKnowYet();
			this->CreatePSO();
		}

	};

	class MeshTest : public GraphicCore {
	public:
		MeshTest(UINT t_width, UINT t_height, LPCTSTR t_title = L"playground")
			: GraphicCore(t_width, t_height, t_title) {}
			// m_Camera(t_width, t_height, Vector3(0.0f, 0.0f, 10.0f), Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f, 1.0f, 0.0f)) {}

		void Init(const HWND m_appHwnd) override;
		void Render() override;

	private:
		void LoadAssert();

		float angle = 0.0;
		float t = 10.0;

		Scene* m_Scene;
		
		ptrRootSignature m_rootSignature;
		ptrPSO m_GraphicPSO;

		std::shared_ptr<TriangleMesh> m_Mesh;
		ptrMaterial m_Material;
	};

}