#pragma once

#include "DXHelper.h"

namespace Graphic {
	class PipelineStateObject {
	public:
		virtual void Initialize(ComPtr<ID3D12Device> device, ID3D12RootSignature* rootSignature) = 0;

		ID3D12PipelineState* GetPSO() const { return m_pipelineState.Get(); }

		std::wstring GetName() const { return pso_name; }

	protected:
		const std::wstring pso_name;
		ComPtr<ID3D12PipelineState> m_pipelineState;
	};
	

	class GraphicsPSO : public PipelineStateObject {
	public:
		void Initialize(ComPtr<ID3D12Device> device, ID3D12RootSignature* rootSignature) override {
			#if defined(_DEBUG)
				// Enable better shader debugging with the graphics debugging tools.
				UINT compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
			#else
				UINT compileFlags = 0;
			#endif

			const std::wstring path = L"D:\\work\\tEngine\\Shaders\\shaders.hlsl";
			ComPtr<ID3DBlob> VertexShader;
			ComPtr<ID3DBlob> PixelShader;
			ThrowIfFailed(D3DCompileFromFile(path.c_str(), nullptr, nullptr, "VSMain", "vs_5_0", compileFlags, 0, &VertexShader, nullptr));
			ThrowIfFailed(D3DCompileFromFile(path.c_str(), nullptr, nullptr, "PSMain", "ps_5_0", compileFlags, 0, &PixelShader, nullptr));
			// TODO modify this
			D3D12_INPUT_ELEMENT_DESC inputElementDescs[] =
			{
				{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
				{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
			};

			// Describe and create the graphics pipeline state object (PSO).
			m_psoDesc.InputLayout = { inputElementDescs, _countof(inputElementDescs) };
			m_psoDesc.pRootSignature = rootSignature; // TODO Root Signature
			m_psoDesc.VS = CD3DX12_SHADER_BYTECODE(VertexShader.Get());
			m_psoDesc.PS = CD3DX12_SHADER_BYTECODE(PixelShader.Get());
			m_psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
			m_psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
			m_psoDesc.DepthStencilState.DepthEnable = FALSE;
			m_psoDesc.DepthStencilState.StencilEnable = FALSE;
			m_psoDesc.SampleMask = UINT_MAX;
			m_psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
			m_psoDesc.NumRenderTargets = 1;
			m_psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
			m_psoDesc.SampleDesc.Count = 1;
			ThrowIfFailed(device->CreateGraphicsPipelineState(&m_psoDesc, IID_PPV_ARGS(&m_pipelineState)));
		}
	private:

		D3D12_GRAPHICS_PIPELINE_STATE_DESC m_psoDesc;
	};

	class ComputePSO : public PipelineStateObject {
	public:
		void Initialize(ComPtr<ID3D12Device> device, ID3D12RootSignature* rootSignature) override {}

	};
}