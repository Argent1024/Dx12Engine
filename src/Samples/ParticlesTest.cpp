#include "ParticlesTest.h"


namespace Samples {

	void ParticleTest::Init(const HWND m_appHwnd) {
		Engine::EnableDebug();
		Engine::CreateDevice();

		// Initialize Command Manager
		//CopyCommandManager.Initialize(m_device);
		CopyHelper.Initialize();
		GraphicsCommandManager.Initialize();

		DescriptorHeap* initHeap = Engine::GetInitHeap();
		initHeap->Initialize();
		DescriptorHeap* useHeap = Engine::GetInUseHeap();
		useHeap->Initialize();
		
		// Swap Chain
		m_swapChain = new SwapChain(m_appHwnd, m_width, m_height);
		m_swapChain->Initialize(GraphicsCommandManager.GetCommadnQueue());

		m_depthBuffer = new DepthBuffer(m_width, m_height);

		// Initialize Root Signature and pass constant into it
		
		m_GraphicRootSignature = std::make_shared<RootSignature>();
		m_GraphicRootSignature->Initialize();
			
		// Create PSO
		m_GraphicPSO = std::make_shared<ParticleTestPSO>();
		m_GraphicPSO->SetRootSigature(m_GraphicRootSignature->GetRootSignature());
		m_GraphicPSO->Initialize();
		
		CreateGameObject();
	}

	void ParticleTest::CreateGameObject() 
	{ 
		// Create Mesh
		std::vector<PointVertex> PointVertics;
		PointVertics.resize(NumParticles);
		for (UINT i = 0; i < NumParticles; i++) {
			PointVertics[i] = { {0.1f, 0.1f, 0.9f, 0.0f} };
		}
		m_Mesh = std::make_shared<PointMesh>(PointVertics);
		
		// Create Texture
		std::vector<ParticleData> textureData;
		GenerateParticlesTexture(textureData);
		m_texture = std::make_shared<TextureBuffer>(NumParticles, sizeof(ParticleData), TEXTURE_RW);
		D3D12_SUBRESOURCE_DATA tData = Texture::CreateTextureData<ParticleData>(textureData);
		m_texture->UploadTexture(tData);

		m_Material = std::make_shared<TextureMaterial>(m_GraphicPSO, m_GraphicRootSignature, m_texture);

		m_ParticleObject.SetMesh(m_Mesh);
		m_ParticleObject.SetMaterial(m_Material);		
	}

	void ParticleTest::Render() 
	{	
		
		GraphicsCommandManager.Start();
		DescriptorHeap* UseHeap = Engine::GetInUseHeap();
		UseHeap->Reset();

		CommandList mainCommandList;
		const float clearColor[] = { 0.0f, 0.0f, 0.0f, 0.0f };
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
			
			ThreadCommandList.ClearDepthChain(*m_depthBuffer);

			m_ParticleObject.RecordCommand(ThreadCommandList);
			m_Camera.UseCamera(ThreadCommandList);
			
			
			// Barrier Draw
			ThreadCommandList.ResourceBarrier(*m_swapChain, D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
			
			ThreadCommandList.SetSwapChain(*m_swapChain);
			//ThreadCommandList.SetSwapChain(*m_swapChain, *m_depthBuffer);
			m_ParticleObject.Draw(ThreadCommandList);

			ThreadCommandList.ResourceBarrier(*m_swapChain, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);

			GraphicsCommandManager.ExecuteCommandList(&ThreadCommandList);
		}
		
		// Wait Finish
		GraphicsCommandManager.End();
		m_swapChain->Present();
	}
}