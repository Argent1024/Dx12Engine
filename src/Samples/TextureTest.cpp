#include "TextureTest.h"

namespace Samples {

	void TextureTestSample::Init(const HWND m_appHwnd) 
	{
		Engine::EnableDebug();
		Engine::CreateDevice();
		
		CopyHelper.Initialize();
		GraphicsCommandManager.Initialize();

		DescriptorHeap* initHeap = Engine::GetInitHeap();
		initHeap->Initialize();
		DescriptorHeap* useHeap = Engine::GetInUseHeap();
		useHeap->Initialize();

		CreatSwapChain(m_appHwnd);
		{
			m_rootSignature = std::make_shared<RootSignature>();
			m_rootSignature->Initialize();
		}

		{
			UINT compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
			ComPtr<ID3DBlob> VS;
			ComPtr<ID3DBlob> PS;
			const std::wstring path = L"D:\\work\\tEngine\\Shaders\\shaders.hlsl";
			ThrowIfFailed(D3DCompileFromFile(path.c_str(), nullptr, nullptr, "VSMain", "vs_5_0", compileFlags, 0, &VS, nullptr));
			ThrowIfFailed(D3DCompileFromFile(path.c_str(), nullptr, nullptr, "PSMain", "ps_5_0", compileFlags, 0, &PS, nullptr));

			// Input for vertex 
			D3D12_INPUT_ELEMENT_DESC inputElementDescs[] =
			{
					{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
					{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
			};

			m_GraphicPSO = std::make_shared<GraphicsPSO>();
			m_GraphicPSO->SetRootSigature(m_rootSignature->GetRootSignature());
			m_GraphicPSO->SetVertexShader(CD3DX12_SHADER_BYTECODE(VS.Get()));
			m_GraphicPSO->SetPixelShader(CD3DX12_SHADER_BYTECODE(PS.Get()));
			m_GraphicPSO->SetTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
			m_GraphicPSO->SetInoutLayout(_countof(inputElementDescs), inputElementDescs);
			m_GraphicPSO->Initialize();
		}
		
		// Create Assert
		
			// Define the geometry for a triangle.
		std::vector<Vertex> triangleVertices =
		{
			{ { 0.5f, 0.5f, 0.0f }, { 1.0f, 1.0f } },
			{ { 0.5f, -0.5f, 0.0f }, { 1.0f, 0.0f } },
			{ { -0.5f, 0.5f, 0.0f }, { 0.0f, 1.0f } },
			{ { -0.5f, -0.5f, 0.0f }, { 0.0f, 0.0f } }
		};
		std::vector<UINT> index_list = { 0, 1, 2, 3, 2, 1 };
		std::cout<<sizeof(Vertex)<<std::endl;
		m_Mesh =std::make_shared<TriangleMesh>(triangleVertices, index_list);
		

		// Create Texture
		m_texture = std::make_shared<Texture>(256, 256);
		m_texture->CreateTexture();

		m_Material = std::make_shared<TextureMaterial>(m_GraphicPSO, m_rootSignature, m_texture);

		m_textureObject.SetMesh(m_Mesh);
		m_textureObject.SetMaterial(m_Material);
		std::cout<<"Finish Creating Texture"<<std::endl;
	}

	void TextureTestSample::Render() 
	{	
		
		GraphicsCommandManager.Start();
		DescriptorHeap* UseHeap = Engine::GetInUseHeap();
		UseHeap->Reset();

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
			
			m_textureObject.RecordCommand(ThreadCommandList);
			m_Camera.UseCamera(ThreadCommandList);
			

			// Barrier Draw
			ThreadCommandList.ResourceBarrier(*m_swapChain, D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
			
			ThreadCommandList.SetSwapChain(*m_swapChain);
			m_textureObject.Draw(ThreadCommandList);

			ThreadCommandList.ResourceBarrier(*m_swapChain, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);

			GraphicsCommandManager.ExecuteCommandList(&ThreadCommandList);
		}
		
		// Wait Finish
		GraphicsCommandManager.End();
		m_swapChain->Present();

	}

}