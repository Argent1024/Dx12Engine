#include "ParticlesTest.h"


namespace Samples {

	void ParticleTest::Init(const HWND m_appHwnd) {
		Engine::EnableDebug();
		Engine::CreateDevice();
	
		// Initialize Command Manager
		//CopyCommandManager.Initialize(m_device);
		CopyHelper.Initialize();
		GraphicsCommandManager.Initialize();
		
		m_swapChain = new SwapChain(m_appHwnd, m_width, m_height);
		m_swapChain->Initialize(GraphicsCommandManager.GetCommadnQueue());

		// Initialize Root Signature and pass constant into it
		
		m_rootSignature = std::make_shared<RootSignature>();
		m_rootSignature->Initialize();
		
		

		// TODO do more thing on pso
		// Compile Shader and Initialize PSO
		{
			UINT compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
			ComPtr<ID3DBlob> VS;
			ComPtr<ID3DBlob> PS;
			const std::wstring path = L"D:\\work\\tEngine\\Shaders\\ray.hlsl";
			ThrowIfFailed(D3DCompileFromFile(path.c_str(), nullptr, nullptr, "VSMain", "vs_5_0", compileFlags, 0, &VS, nullptr));
			ThrowIfFailed(D3DCompileFromFile(path.c_str(), nullptr, nullptr, "PSMain", "ps_5_0", compileFlags, 0, &PS, nullptr));

			// Input for vertex 
			D3D12_INPUT_ELEMENT_DESC inputElementDescs[] =
			{
				{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
			};

			m_GraphicPSO = std::make_shared<GraphicsPSO>();
			m_GraphicPSO->SetRootSigature(m_rootSignature->GetRootSignature());
			m_GraphicPSO->SetVertexShader(CD3DX12_SHADER_BYTECODE(VS.Get()));
			m_GraphicPSO->SetPixelShader(CD3DX12_SHADER_BYTECODE(PS.Get()));
			m_GraphicPSO->SetTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
			m_GraphicPSO->SetInoutLayout(_countof(inputElementDescs), inputElementDescs);
			m_GraphicPSO->Initialize();
		}

		CreateGameObject();
	}

	void ParticleTest::CreateGameObject() 
	{ 
		// Create Mesh
		std::vector<PointVertex> PointVertics(NumParticles, { {0.0f, 0.0f, 0.0f, 0.0f} });
		m_Mesh = std::make_shared<PointMesh>(PointVertics);
		
		m_Material = std::make_shared<NoMaterial>(m_GraphicPSO, m_rootSignature);

		m_ParticleObject.SetMesh(m_Mesh);
		m_ParticleObject.SetMaterial(m_Material);		
	}

	void ParticleTest::Render() 
	{	
		
		GraphicsCommandManager.Start();
		CommandList mainCommandList;
		const float clearColor[] = { 0.0f, 0.2f, 0.4f, 1.0f };
		GraphicsCommandManager.InitCommandList(&mainCommandList);
		mainCommandList.SetSwapChain(*m_swapChain);
		mainCommandList.ResourceBarrier(*m_swapChain, D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
		mainCommandList.ClearSwapChain(*m_swapChain, clearColor);
		mainCommandList.ResourceBarrier(*m_swapChain, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
		GraphicsCommandManager.ExecuteCommandList(&mainCommandList);

		// Record commands for each object, only have one object here
		{
			CommandList ThreadCommandList;
			GraphicsCommandManager.InitCommandList(&ThreadCommandList);
			
			m_ParticleObject.RecordCommand(ThreadCommandList);
			m_Camera.UseCamera(ThreadCommandList);
			
			
			// Barrier Draw
			ThreadCommandList.ResourceBarrier(*m_swapChain, D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
			
			ThreadCommandList.SetSwapChain(*m_swapChain);
			m_ParticleObject.Draw(ThreadCommandList);

			ThreadCommandList.ResourceBarrier(*m_swapChain, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);

			GraphicsCommandManager.ExecuteCommandList(&ThreadCommandList);
		}
		
		// Wait Finish
		GraphicsCommandManager.End();
		m_swapChain->Present();
	}
}