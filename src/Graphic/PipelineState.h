/*
	Shader Class
	Help to compile and load shaders, both graphics and compute


*/
#pragma once

#include "Utility/Logger.h"
#include "GraphicCore.h"

//TODO fix this and Solution dir
#define SHADER_DIR SOLUTION_DIR"Shaders\\"

#define ptrPSO std::shared_ptr<Graphic::PipelineStateObject>
#define ptrGraphicsPSO std::shared_ptr<Graphic::GraphicsPSO>
#define ptrComputePSO std::shared_ptr<Graphic::ComputePSO>

namespace Graphic {

	class PipelineStateObject {
	public:
		virtual void Initialize() = 0;
		// TODO Is this ok..?
		void SetRootSigature(ID3D12RootSignature* rootSignature) { m_rootSignature = rootSignature; } 
		ID3D12PipelineState* GetPSO() const { return m_pipelineState.Get(); }

	protected:
		ComPtr<ID3D12PipelineState> m_pipelineState;	
		ID3D12RootSignature* m_rootSignature;
		static const UINT CompileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
	};

	// Base class for Graphics PSO
	// Usage: 
	// 1. Create the a pso class for every pso needed
	// 2. Implement the GraphicsPSO::Initialize() function
	//	  (Create Shaders, set configrations and call device->createPSO)	  
	// 3. Create an UNIQUE PSO for each type of PSO in the app
	// 4. Call pso->SetRootSignature(m_GraphicRootSignature->GetRootSignature())
	// 5. Call pso->Initialize()
	class GraphicsPSO : public PipelineStateObject {
	public:
		GraphicsPSO(std::wstring path): 
			m_ShaderPath(path), m_psoDesc() { }

	protected:
		std::wstring m_ShaderPath;

		// Compile and set shaders
		//TODO fix string problems, doesn't work yet
		inline void SetVertexShader(const std::wstring& path, const std::string& funcName) 
		{
			ComPtr<ID3DBlob> VS;
			ThrowIfFailed(
				D3DCompileFromFile(path.c_str(), nullptr, nullptr, 
					funcName.c_str(), "vs_5_0", CompileFlags, 0, &VS, nullptr)
			);
			m_psoDesc.VS = CD3DX12_SHADER_BYTECODE(VS.Get());
		}

		inline void SetPixelShader(const std::wstring& path, const std::string& funcName)
		{
			ComPtr<ID3DBlob> PS;
			ThrowIfFailed(
				D3DCompileFromFile(path.c_str(), nullptr, nullptr,
					funcName.c_str(), "ps_5_0", CompileFlags, 0, &PS, nullptr)
			);
			m_psoDesc.PS = CD3DX12_SHADER_BYTECODE(PS.Get());
		}

		inline void SetGeometryShader(const std::wstring& path, const std::string& funcName)
		{
			ComPtr<ID3DBlob> GS;
			ThrowIfFailed(
				D3DCompileFromFile(path.c_str(), nullptr, nullptr,
					funcName.c_str(), "gs_5_0", CompileFlags, 0, &GS, nullptr)
			);
			m_psoDesc.GS = CD3DX12_SHADER_BYTECODE(GS.Get());
		}
		
		inline void SetHullShader(const std::wstring& path, const std::string& funcName)
		{
			ComPtr<ID3DBlob> HS;
			ThrowIfFailed(
				D3DCompileFromFile(path.c_str(), nullptr, nullptr,
					funcName.c_str(), "hs_5_0", CompileFlags, 0, &HS, nullptr)
			);
			m_psoDesc.HS = CD3DX12_SHADER_BYTECODE(HS.Get());
		}

		inline void SetDomainShader(const std::wstring& path, const std::string& funcName)
		{
			ComPtr<ID3DBlob> DS;
			ThrowIfFailed(
				D3DCompileFromFile(path.c_str(), nullptr, nullptr,
					funcName.c_str(), "ds_5_0", CompileFlags, 0, &DS, nullptr)
			);
			m_psoDesc.DS = CD3DX12_SHADER_BYTECODE(DS.Get());
		}

		// Set Shaders
		inline void SetVertexShader( const D3D12_SHADER_BYTECODE& Binary ) { m_psoDesc.VS = Binary; }
		inline void SetPixelShader( const D3D12_SHADER_BYTECODE& Binary ) { m_psoDesc.PS = Binary; }
		inline void SetGeometryShader( const D3D12_SHADER_BYTECODE& Binary ) { m_psoDesc.GS = Binary; }
		inline void SetHullShader( const D3D12_SHADER_BYTECODE& Binary ) { m_psoDesc.HS = Binary; }
		inline void SetDomainShader( const D3D12_SHADER_BYTECODE& Binary ) { m_psoDesc.DS = Binary; }

		inline void SetTopologyType(const D3D12_PRIMITIVE_TOPOLOGY_TYPE type) { m_psoDesc.PrimitiveTopologyType = type; }
		
		// TODO maybe a new class of input layout
		inline void SetInoutLayout(UINT NumElements, D3D12_INPUT_ELEMENT_DESC* InputDescs) 
		{
			m_psoDesc.InputLayout = D3D12_INPUT_LAYOUT_DESC{ InputDescs, NumElements};
		}
		
		// Configrations
		inline void SetDepthStencilState(D3D12_DEPTH_STENCIL_DESC desc=CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT)){ m_psoDesc.DepthStencilState = desc; }
		inline void SetBlendState(D3D12_BLEND_DESC desc=CD3DX12_BLEND_DESC(D3D12_DEFAULT)) { m_psoDesc.BlendState = desc; }
		inline void SetRasterState(D3D12_RASTERIZER_DESC desc=CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT)) { m_psoDesc.RasterizerState = desc; }

		inline void SetNumRenderTargets(UINT num=1) { m_psoDesc.NumRenderTargets = 1; }

		//TODO lol
		void SetStuffThatIdontKnowYet() {
			m_psoDesc.SampleMask = UINT_MAX;
			m_psoDesc.NumRenderTargets = 1;
			m_psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
			m_psoDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
			m_psoDesc.SampleDesc.Count = 1;
		}

		// Create the PSO using m_psoDesc
		inline void CreatePSO()
		{
			ID3D12Device* device = Engine::GetDevice();
			ThrowIfFailed(device->CreateGraphicsPipelineState(&m_psoDesc, IID_PPV_ARGS(&m_pipelineState)));
		}

		D3D12_GRAPHICS_PIPELINE_STATE_DESC m_psoDesc;
		std::vector<D3D12_INPUT_ELEMENT_DESC> m_inputLayouts;
	};


	class ComputePSO : public PipelineStateObject {
	public:
		ComputePSO(const std::wstring& cs_path) : m_ComputeShaderPath(cs_path), m_psoDesc() { }

		void Initialize() override 
		{
			assert(m_rootSignature != nullptr);
			m_psoDesc.pRootSignature = m_rootSignature;

			ComPtr<ID3DBlob> CS;
			ComPtr<ID3DBlob> errorMsg;
			HRESULT hr = D3DCompileFromFile(m_ComputeShaderPath.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE , "CSMain", "cs_5_1", CompileFlags, 0, &CS, &errorMsg);
			if (errorMsg.Get()) {
				Logger::Log((char*)errorMsg.Get()->GetBufferPointer());
			}
			ThrowIfFailed(hr);
			m_psoDesc.CS = CD3DX12_SHADER_BYTECODE(CS.Get());

			ID3D12Device* device = Engine::GetDevice();
			ThrowIfFailed(device->CreateComputePipelineState(&m_psoDesc, IID_PPV_ARGS(&m_pipelineState)));	
		}

	private:
		D3D12_COMPUTE_PIPELINE_STATE_DESC m_psoDesc;
		std::wstring m_ComputeShaderPath;
	};



	/*********************************************************
	*				Implement Below                          *
	**********************************************************/


	class DefaultPSO : public GraphicsPSO {
	public:
		DefaultPSO(std::wstring shaderpath=L"D:\\work\\tEngine\\Shaders\\shaders.hlsl") 
			: GraphicsPSO(shaderpath) { }

		void Initialize() override;
	};

		// Combine some texture and output
	class MixturePSO : public Graphic::GraphicsPSO 
	{
	public:
		MixturePSO() : GraphicsPSO(L"D:\\work\\tEngine\\Shaders\\MixTexture.hlsl") { }

		void Initialize() override;
	};

}