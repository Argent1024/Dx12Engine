#include "RayMarching.h"

namespace Samples {
	void RayMarchingRootSignature::Initialize() 
	{
		ID3D12Device* device = Engine::GetDevice();
		D3D12_FEATURE_DATA_ROOT_SIGNATURE featureData = {};
		// This is the highest version the sample supports. If CheckFeatureSupport succeeds, the HighestVersion returned will not be greater than this.
		featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_1;
		if (FAILED(device->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE, &featureData, sizeof(featureData))))
		{
			featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_0;
		}

		// Define Slot
		CD3DX12_ROOT_PARAMETER1 rootParameters[1];
		rootParameters[0].InitAsConstants(sizeof(ScreenConstantData) / 4, 0, 0, D3D12_SHADER_VISIBILITY_ALL);
		D3D12_ROOT_SIGNATURE_FLAGS rsFlag = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
			
		// Create RootSignature
		CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
		rootSignatureDesc.Init_1_1(_countof(rootParameters), rootParameters, 0, nullptr, rsFlag);
		ComPtr<ID3DBlob> signature;
		ComPtr<ID3DBlob> error;
		ThrowIfFailed(D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, featureData.HighestVersion, &signature, &error));
		ThrowIfFailed(device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&m_RootSignature)));
	}

	void RayMarching::Init(const HWND t_appHwnd) {
		Engine::EnableDebug();
		Engine::CreateDevice();
	
		// Initialize Command Manager
		//CopyCommandManager.Initialize(m_device);
		CopyHelper.Initialize();
		GraphicsCommandManager.Initialize();
		
		// Swap Chain
		m_swapChain = new SwapChain(t_appHwnd, m_width, m_height);
		m_swapChain->Initialize(GraphicsCommandManager.GetCommadnQueue());

		// Initialize Root Signature and pass constant into it
		
		m_rootSignature = std::make_shared<RayMarchingRootSignature>();
		m_rootSignature->Initialize();
		

		// PSO
		m_GraphicPSO = std::make_shared<RayMarchingPSO>();
		m_GraphicPSO->SetRootSigature(m_rootSignature->GetRootSignature());
		m_GraphicPSO->Initialize();
		

		CreateGameObject();
	}

	void RayMarching::CreateGameObject() 
	{
		// Create Mesh
		{
			struct RayMarchVertex
			{
				XMFLOAT3 position;
				XMFLOAT4 color;
			};

			// Define the geometry for a triangle.
			std::vector<RayMarchVertex> triangleVertices =
			{
				{ { 1.0f,  1.0f, 0.0f }, { 1.0f, 1.0f, 0.0f, 1.0f } },
				{ { 1.0f, -1.0f, 0.0f }, { 1.0f, 0.0f, 0.0f, 1.0f } },
				{ { -1.0f, 1.0f, 0.0f }, { 0.0f, 1.0f, 0.0f, 1.0f } },
				{ { -1.0f, -1.0f, 0.0f }, { 0.0f, 0.0f, 0.0f, 1.0f} }
			};
			std::vector<UINT> index_list = { 0, 1, 2, 3, 2, 1 };
			
			m_Mesh = std::make_shared<TriangleMesh>(triangleVertices, index_list);
		}

		m_Material = std::make_shared<NoMaterial>(m_GraphicPSO, m_rootSignature);

		m_rayMarchScreen.SetMesh(m_Mesh);
		m_rayMarchScreen.SetMaterial(m_Material);		
	}

	void RayMarching::Render() 
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
			
			m_rayMarchScreen.RecordCommand(ThreadCommandList);
			m_Camera.UseCamera(ThreadCommandList);
			
			// Set data to root constant
			ScreenConstantData data;
			data.ScreenSize = XMFLOAT4((float)m_width, (float)m_height, 0.0, 0.0);
			ThreadCommandList.SetGraphicsRootConstants(0, sizeof(ScreenConstantData) / 4, &data);
			
			// Barrier Draw
			ThreadCommandList.ResourceBarrier(*m_swapChain, D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
			
			ThreadCommandList.SetSwapChain(*m_swapChain);
			m_rayMarchScreen.Draw(ThreadCommandList);

			ThreadCommandList.ResourceBarrier(*m_swapChain, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);

			GraphicsCommandManager.ExecuteCommandList(&ThreadCommandList);
		}
		
		// Wait Finish
		GraphicsCommandManager.End();
		m_swapChain->Present();
	}
}