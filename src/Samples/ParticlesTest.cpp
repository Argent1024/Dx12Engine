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
		
		m_swapChain = new SwapChain(m_appHwnd, m_width, m_height);
		m_swapChain->Initialize(GraphicsCommandManager.GetCommadnQueue());

		m_depthBuffer = new DepthBuffer(m_width, m_height);

		// Initialize Root Signature and pass constant into it
		
		m_GraphicRootSignature = std::make_shared<RootSignature>();
		m_GraphicRootSignature->Initialize();
			

		// TODO do more thing on pso
		// Compile Shader and Initialize PSO
		{
			UINT compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
			ComPtr<ID3DBlob> VS;
			ComPtr<ID3DBlob> GS;
			ComPtr<ID3DBlob> PS;
			
			const std::wstring ShaderPath=L"D:\\work\\tEngine\\Shaders\\particletest.hlsl";

			ThrowIfFailed(D3DCompileFromFile(ShaderPath.c_str(), nullptr, nullptr, "VSParticleDraw", "vs_5_0", compileFlags, 0, &VS, nullptr));
			ThrowIfFailed(D3DCompileFromFile(ShaderPath.c_str(), nullptr, nullptr, "GSParticleDraw", "gs_5_0", compileFlags, 0, &GS, nullptr));
			ThrowIfFailed(D3DCompileFromFile(ShaderPath.c_str(), nullptr, nullptr, "PSParticleDraw", "ps_5_0", compileFlags, 0, &PS, nullptr));

			// Input for vertex 
			D3D12_INPUT_ELEMENT_DESC inputElementDescs[] =
			{
				{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
			};

			m_GraphicPSO = std::make_shared<GraphicsPSO>();
			m_GraphicPSO->SetRootSigature(m_GraphicRootSignature->GetRootSignature());
			m_GraphicPSO->SetVertexShader(CD3DX12_SHADER_BYTECODE(VS.Get()));
			m_GraphicPSO->SetGeometryShader(CD3DX12_SHADER_BYTECODE(GS.Get()));
			m_GraphicPSO->SetPixelShader(CD3DX12_SHADER_BYTECODE(PS.Get()));
			m_GraphicPSO->SetTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT);
			m_GraphicPSO->SetInoutLayout(_countof(inputElementDescs), inputElementDescs);

			CD3DX12_DEPTH_STENCIL_DESC depthStencilDesc(D3D12_DEFAULT);
			depthStencilDesc.DepthEnable = FALSE;
			depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
			m_GraphicPSO->SetDepthStencilState(depthStencilDesc);

			CD3DX12_BLEND_DESC blendDesc(D3D12_DEFAULT);
			blendDesc.RenderTarget[0].BlendEnable = TRUE;
			blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
			blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
			blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ZERO;
			blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
			m_GraphicPSO->SetBlendState(blendDesc);

			m_GraphicPSO->Initialize();
		}

		CreateGameObject();
	}

	void ParticleTest::CreateGameObject() 
	{ 
		// Create Mesh
		std::vector<PointVertex> PointVertics;
		PointVertics.resize(NumParticles);
		for (UINT i = 0; i < NumParticles; i++) {
			PointVertics[i] = { {0.1, 0.1, 0.9, 0.0} };
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