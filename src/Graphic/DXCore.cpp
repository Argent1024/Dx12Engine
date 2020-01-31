#include "DXCore.h"

namespace Graphic {

	

	// Enable the debug layer
	// NOTE: Enabling the debug layer after device creation will invalidate the active device.
	void DXCore::EnableDebug()
	{
		ComPtr<ID3D12Debug> debugController;
		if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
		{
			debugController->EnableDebugLayer();
			// Enable additional debug layers.
			dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
		}
			
	}

	// Helper function for acquiring the first available hardware adapter that supports Direct3D 12.
	// If no such adapter can be found, *ppAdapter will be set to nullptr.
	void DXCore::GetHardwareAdapter(IDXGIFactory2* pFactory, IDXGIAdapter1** ppAdapter)
	{
		ComPtr<IDXGIAdapter1> adapter;
		*ppAdapter = nullptr;
		for (UINT adapterIndex = 0; DXGI_ERROR_NOT_FOUND != pFactory->EnumAdapters1(adapterIndex, &adapter); ++adapterIndex)
		{
			DXGI_ADAPTER_DESC1 desc;
			adapter->GetDesc1(&desc);

			if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
			{
				// Don't select the Basic Render Driver adapter.
				// If you want a software adapter, pass in "/warp" on the command line.
				continue;
			}
				// Check to see if the adapter supports Direct3D 12, but don't create the
			// actual device yet.
			if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), nullptr)))
			{
				break;
			}
		}

		*ppAdapter = adapter.Detach();
	}
		
	// Create DX12 Device
	void DXCore::CreateDevice() 
	{
		ThrowIfFailed(CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&m_factory)));
			
		if (m_useWarpDevice) {
			ComPtr<IDXGIAdapter> warpAdapter;
			ThrowIfFailed(m_factory->EnumWarpAdapter(IID_PPV_ARGS(&warpAdapter)));

			ThrowIfFailed(D3D12CreateDevice(
				warpAdapter.Get(),
				D3D_FEATURE_LEVEL_11_0,
				IID_PPV_ARGS(&m_device)
			));
		} else {
			ComPtr<IDXGIAdapter1> hardwareAdapter;
			GetHardwareAdapter(m_factory.Get(), &hardwareAdapter);

			ThrowIfFailed(D3D12CreateDevice(
				hardwareAdapter.Get(),
				D3D_FEATURE_LEVEL_11_0,
				IID_PPV_ARGS(&m_device)
			));
		}
	}

	// Create DX12 SwapChain
	void DXCore::CreateSwapChain(const HWND t_appHwnd) {
		m_swapChain = new SwapChain(t_appHwnd, m_width, m_height);
		m_swapChain->Initialize(m_factory, m_device, GraphicsCommandManager.GetCommadnQueue());
	}

	void DXCore::Init(const HWND t_appHwnd) {

		EnableDebug();
		// Create factory
		ThrowIfFailed(CreateDXGIFactory1(IID_PPV_ARGS(&m_factory)));
		CreateDevice();

		// Init command manager
		CopyCommandManager.Initialize(m_device);
		GraphicsCommandManager.Initialize(m_device);

		m_commandList = new CommandList(D3D12_COMMAND_LIST_TYPE_DIRECT);
		// Create a root signature consisting of a descriptor table with a single CBV.
		{
			D3D12_FEATURE_DATA_ROOT_SIGNATURE featureData = {};

			// This is the highest version the sample supports. If CheckFeatureSupport succeeds, the HighestVersion returned will not be greater than this.
			featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_1;

			if (FAILED(m_device->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE, &featureData, sizeof(featureData))))
			{
				featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_0;
			}

			CD3DX12_DESCRIPTOR_RANGE1 ranges[1];
			CD3DX12_ROOT_PARAMETER1 rootParameters[1];

			ranges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC);
			rootParameters[0].InitAsDescriptorTable(1, &ranges[0], D3D12_SHADER_VISIBILITY_ALL); // TODO root signature & descriptor table?

			// Allow input layout and deny uneccessary access to certain pipeline stages.
			D3D12_ROOT_SIGNATURE_FLAGS rootSignatureFlags =
				D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
			
			CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
			rootSignatureDesc.Init_1_1(_countof(rootParameters), rootParameters, 0, nullptr, rootSignatureFlags);

			ComPtr<ID3DBlob> signature;
			ComPtr<ID3DBlob> error;
			ThrowIfFailed(D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, featureData.HighestVersion, &signature, &error));
			ThrowIfFailed(m_device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&m_rootSignature)));
		}

		UINT compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
		
		const std::wstring path = L"D:\\work\\tEngine\\Shaders\\ray.hlsl";
		ComPtr<ID3DBlob> VS;
		ComPtr<ID3DBlob> PS;
		ThrowIfFailed(D3DCompileFromFile(path.c_str(), nullptr, nullptr, "VSMain", "vs_5_0", compileFlags, 0, &VS, nullptr));
		ThrowIfFailed(D3DCompileFromFile(path.c_str(), nullptr, nullptr, "PSMain", "ps_5_0", compileFlags, 0, &PS, nullptr));

		// Input for vertex 
		D3D12_INPUT_ELEMENT_DESC inputElementDescs[] =
		{
				{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
				{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
		};

		pso = new GraphicsPSO();
		pso->SetRootSigature(m_rootSignature.Get());
		pso->SetVertexShader(CD3DX12_SHADER_BYTECODE(VS.Get()));
		pso->SetPixelShader(CD3DX12_SHADER_BYTECODE(PS.Get()));
		pso->SetTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
		pso->SetInoutLayout(_countof(inputElementDescs), inputElementDescs);
		pso->Initialize(m_device);

		CreateSwapChain(t_appHwnd);
		{
			// Init asset below
			// Define the geometry for a triangle.
			Vertex triangleVertices[] =
			{
				{ { 1.0f,  1.0f, 0.0f }, { 1.0f, 1.0f, 0.0f, 1.0f } },
				{ { 1.0f, -1.0f, 0.0f }, { 1.0f, 0.0f, 0.0f, 1.0f } },
				{ { -1.0f, 1.0f, 0.0f }, { 0.0f, 1.0f, 0.0f, 1.0f } },
				{ { -1.0f, -1.0f, 0.0f }, { 0.0f, 0.0f, 0.0f, 1.0f} }
			};

			UINT index_list[] = {
				0, 1, 2,
				3, 2, 1
			};

			const UINT vertexBufferSize = sizeof(triangleVertices);
			const UINT indexBufferSize = sizeof(index_list);

			// Use upload type buffer
			m_GPUmem = new GPU::UploadBuffer(vertexBufferSize + indexBufferSize);
			m_GPUmem->Initialize(m_device);

			// Create Mesh
			ptrVertexBuffer pVertex = std::make_shared<VertexBuffer>(m_GPUmem, vertexBufferSize, sizeof(Vertex));
			pVertex->Initialize();
			pVertex->copyData(triangleVertices);

			ptrIndexBuffer pIndex = std::make_shared<IndexBuffer>(m_GPUmem, indexBufferSize);
			pIndex->Initialize();
			pIndex->copyData(index_list);


			SceneConstantBuffer cbData;
			cbData.offset = XMFLOAT4((float)m_width, (float)m_height, 1.0, 1.0);
			const UINT cbSize = sizeof(cbData);

			const UINT cbBufferSize = (sizeof(SceneConstantBuffer) + 255) & ~255;
			cbvHeap = new DescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 1, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE);
			cbvHeap->Initialize(m_device);

			m_cbGPUmem = new GPU::UploadBuffer(cbBufferSize);
			m_cbGPUmem->Initialize(m_device);

			m_ConstantBuffer = new ConstantBuffer(m_cbGPUmem, cbSize, cbvHeap);
			m_ConstantBuffer->Initialize(m_device);
			m_ConstantBuffer->copyData(&cbData);

			// Create mesh
			m_mesh = new Game::TriangleMesh(pVertex, pIndex);
		}
	}

	void DXCore::RecordCommandList() 
	{
		ID3D12GraphicsCommandList* list = m_commandList->GetCommandList();
		// Record Start
		list->SetGraphicsRootSignature(m_rootSignature.Get());

		ID3D12DescriptorHeap* ppHeaps[] = { cbvHeap->GetDescriptorHeap() };
		list->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

		list->SetGraphicsRootDescriptorTable(0, cbvHeap->GetGPUHandle(0));

		list->SetPipelineState(pso->GetPSO());

		list->RSSetViewports(1, &m_viewport);
		list->RSSetScissorRects(1, &m_scissorRect);

		// Indicate that the back buffer will be used as a render target.
		list->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_swapChain->GetBackBufferResource(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));

		// Set & clear swapchain
		const float clearColor[] = { 0.0f, 0.2f, 0.4f, 1.0f };
		m_commandList->SetSwapChain(*m_swapChain);
		m_commandList->ClearSwapChain(*m_swapChain, clearColor);

		// Record commands.
		{
			// Draw Mesh
			m_mesh->SetMesh(*m_commandList);
			m_mesh->Draw(*m_commandList);
		}

		// Indicate that the back buffer will now be used to present.
		list->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_swapChain->GetBackBufferResource(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));
		m_commandList->Close();
	}

	void DXCore::Render() {
		GraphicsCommandManager.Start();
		GraphicsCommandManager.InitCommandList(m_commandList);
		RecordCommandList();
		GraphicsCommandManager.ExecuteCommandList(m_commandList);
		GraphicsCommandManager.End();
		/*uint64_t fenceValue = m_commandQueue->Execute(m_commandList->GetCommandList());
		m_commandQueue->WaitCPU(fenceValue);*/
		m_swapChain->Present();
	}
}