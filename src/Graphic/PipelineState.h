/*
	Shader Class
	Help to compile and load shaders, both graphics and compute


*/
#pragma once

#include "DXHelper.h"

namespace Graphic {

	class PipelineStateObject {
	public:
		
		virtual void Initialize(ComPtr<ID3D12Device> device) = 0;

		void SetRootSigature(ID3D12RootSignature* rootSignature) { m_rootSignature = rootSignature; } 
		ID3D12PipelineState* GetPSO() const { return m_pipelineState.Get(); }

	protected:
		ComPtr<ID3D12PipelineState> m_pipelineState;	
		ID3D12RootSignature* m_rootSignature;

	};
	
	class GraphicsInputLayout {


	};

	class GraphicsPSO : public PipelineStateObject {
	public:
		// Set Shader
		void SetVertexShader( const D3D12_SHADER_BYTECODE& Binary ) { m_psoDesc.VS = Binary; }
		void SetPixelShader( const D3D12_SHADER_BYTECODE& Binary ) { m_psoDesc.PS = Binary; }
		void SetGeometryShader( const D3D12_SHADER_BYTECODE& Binary ) { m_psoDesc.GS = Binary; }
		void SetHullShader( const D3D12_SHADER_BYTECODE& Binary ) { m_psoDesc.HS = Binary; }
		void SetDomainShader( const D3D12_SHADER_BYTECODE& Binary ) { m_psoDesc.DS = Binary; }

		void SetTopologyType(const D3D12_PRIMITIVE_TOPOLOGY_TYPE type) { m_psoDesc.PrimitiveTopologyType = type; }
		// void SetInoutLayout();
		
		//TODO Modify
		void SetDefault() {
			m_psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
			m_psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
			m_psoDesc.SampleMask = UINT_MAX;
			m_psoDesc.NumRenderTargets = 1;
			m_psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
			m_psoDesc.SampleDesc.Count = 1;
		}

		void Initialize(ComPtr<ID3D12Device> device) override {
			assert(m_rootSignature != nullptr);
			m_psoDesc.pRootSignature = m_rootSignature;
			SetDefault();
			D3D12_INPUT_ELEMENT_DESC inputElementDescs[] =
			{
				{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
				{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
			};
			m_psoDesc.InputLayout = D3D12_INPUT_LAYOUT_DESC{ inputElementDescs, _countof(inputElementDescs)};
			ThrowIfFailed(device->CreateGraphicsPipelineState(&m_psoDesc, IID_PPV_ARGS(&m_pipelineState)));
		}

	private:
		D3D12_GRAPHICS_PIPELINE_STATE_DESC m_psoDesc;
		std::shared_ptr<const D3D12_INPUT_ELEMENT_DESC> m_inputLayouts;
	};

	class ComputePSO : public PipelineStateObject {
	public:
		void Initialize(ComPtr<ID3D12Device> device) override {}

	};
}