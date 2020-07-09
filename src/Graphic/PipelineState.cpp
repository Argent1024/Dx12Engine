#include "PipelineState.h"

namespace Graphic {
	void DefaultPSO::Initialize() 
	{
		assert(m_rootSignature != nullptr);
		m_psoDesc.pRootSignature = m_rootSignature;

		ComPtr<ID3DBlob> VS;
		ComPtr<ID3DBlob> PS;
		const std::wstring path = L"D:\\work\\tEngine\\Shaders\\shaders.hlsl";
		ThrowIfFailed(D3DCompileFromFile(path.c_str(), nullptr, nullptr, "VSMain", "vs_5_0", CompileFlags, 0, &VS, nullptr));
		ThrowIfFailed(D3DCompileFromFile(path.c_str(), nullptr, nullptr, "PSMain", "ps_5_0", CompileFlags, 0, &PS, nullptr));

		D3D12_INPUT_ELEMENT_DESC  inputElementDescs[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,  0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "NORMAL",	  0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
		};

		this->SetVertexShader(CD3DX12_SHADER_BYTECODE(VS.Get()));
		this->SetPixelShader(CD3DX12_SHADER_BYTECODE(PS.Get()));
		this->SetTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
		this->SetInoutLayout(_countof(inputElementDescs), inputElementDescs);

		// configrations
		CD3DX12_DEPTH_STENCIL_DESC depthStencilDesc(D3D12_DEFAULT);
        depthStencilDesc.DepthEnable = true;
        depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
        depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
        depthStencilDesc.StencilEnable = FALSE;

		this->SetDepthStencilState(depthStencilDesc);

		this->SetBlendState();
		this->SetRasterState();

		this->SetStuffThatIdontKnowYet();
		this->CreatePSO();
	}

	void MixturePSO::Initialize() 
	{
		assert(m_rootSignature != nullptr);
		m_psoDesc.pRootSignature = m_rootSignature;

		ComPtr<ID3DBlob> VS;
		ComPtr<ID3DBlob> PS;
		const std::wstring path =  L"D:\\work\\tEngine\\Shaders\\MixTexture.hlsl";
		ThrowIfFailed(D3DCompileFromFile(path.c_str(), nullptr, nullptr, "VSMain", "vs_5_0", CompileFlags, 0, &VS, nullptr));
		ThrowIfFailed(D3DCompileFromFile(path.c_str(), nullptr, nullptr, "PSMain", "ps_5_0", CompileFlags, 0, &PS, nullptr));

		D3D12_INPUT_ELEMENT_DESC  inputElementDescs[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,  0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "NORMAL",	  0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
		};

		this->SetVertexShader(CD3DX12_SHADER_BYTECODE(VS.Get()));
		this->SetPixelShader(CD3DX12_SHADER_BYTECODE(PS.Get()));
		this->SetTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
		this->SetInoutLayout(_countof(inputElementDescs), inputElementDescs);

		// configrations
		CD3DX12_DEPTH_STENCIL_DESC depthStencilDesc(D3D12_DEFAULT);
		depthStencilDesc.DepthEnable = FALSE;
		depthStencilDesc.StencilEnable = FALSE;
		this->SetDepthStencilState(depthStencilDesc);
		this->SetBlendState();
		this->SetRasterState();
		this->SetStuffThatIdontKnowYet();
		this->CreatePSO();
	}
}