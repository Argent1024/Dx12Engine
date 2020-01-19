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
		// Describe and create the swap chain.
		DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
		swapChainDesc.BufferCount = FrameCount;
		swapChainDesc.Width = m_width;
		swapChainDesc.Height = m_height;
		swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		swapChainDesc.SampleDesc.Count = 1;

		ComPtr<IDXGISwapChain1> swapChain;
		ThrowIfFailed(m_factory->CreateSwapChainForHwnd(
			m_commandQueue->GetCommadnQueue(),   // Swap chain needs the queue so that it can force a flush on it.
			t_appHwnd,					// The window handle in os
			&swapChainDesc,
			nullptr,
			nullptr,
			&swapChain
		));

		// This sample does not support fullscreen transitions.
		ThrowIfFailed(m_factory->MakeWindowAssociation(t_appHwnd, DXGI_MWA_NO_ALT_ENTER));

		ThrowIfFailed(swapChain.As(&m_swapChain));
		m_frameIndex = m_swapChain->GetCurrentBackBufferIndex();

	}

	void DXCore::CreateDescriptorHeap() {
		// Describe and create a render target view (RTV) descriptor heap.
		D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
		rtvHeapDesc.NumDescriptors = FrameCount;
		rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		ThrowIfFailed(m_device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&m_rtvHeap)));

		m_rtvDescriptorSize = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	}

	void DXCore::CreateRTV() {
		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_rtvHeap->GetCPUDescriptorHandleForHeapStart());

		// Create a RTV for each frame.
		for (UINT n = 0; n < FrameCount; n++)
		{
			ThrowIfFailed(m_swapChain->GetBuffer(n, IID_PPV_ARGS(&m_renderTargets[n])));
			m_device->CreateRenderTargetView(m_renderTargets[n].Get(), nullptr, rtvHandle);
			rtvHandle.Offset(1, m_rtvDescriptorSize);
		}
		
	}

	void DXCore::Init(const HWND t_appHwnd) {
		EnableDebug();
		// Create factory
		ThrowIfFailed(CreateDXGIFactory1(IID_PPV_ARGS(&m_factory)));
		CreateDevice();
		
		// Create Command Queue
		m_commandQueue = new CommandQueue();
		m_commandQueue->Initialize(m_device);

		m_commandList = new CommandList(D3D12_COMMAND_LIST_TYPE_DIRECT);
		m_commandList->Initialize(m_device);
		
		// Create an empty root signature.
		{
			CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc;
			rootSignatureDesc.Init(0, nullptr, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

			ComPtr<ID3DBlob> signature;
			ComPtr<ID3DBlob> error;
			ThrowIfFailed(D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &error));
			ThrowIfFailed(m_device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&m_rootSignature)));
		}

		pso = new GraphicsPSO();
		pso->Initialize(m_device, m_rootSignature.Get());
	
		// TODO abstract this
		CreateSwapChain(t_appHwnd);
		CreateDescriptorHeap();
		CreateRTV();


		// Init asset below
		// Define the geometry for a triangle.
		Vertex triangleVertices[] =
		{
			{ { 0.5f,  0.5f, 0.0f }, { 0.0f, 1.0f, 0.0f, 1.0f } },
			{ { 0.5f, -0.5f, 0.0f }, { 0.0f, 0.0f, 1.0f, 1.0f } },
			{ { -0.5f, 0.5f, 0.0f }, { 1.0f, 1.0f, 1.0f, 1.0f } },
			{ { -0.5f, -0.5f, 0.0f }, { 1.0f, 0.0f, 0.0f, 1.0f} }
		};

		UINT index_list[] = {
			0, 1, 2,
			3, 2, 1
		};

		const UINT vertexBufferSize = sizeof(triangleVertices);
		const UINT indexBufferSize = sizeof(index_list);

		m_GPUmem = new GPUMemory();
		m_GPUmem->Initialize(m_device, vertexBufferSize+indexBufferSize, D3D12_HEAP_TYPE_UPLOAD);

		m_vertexBuffer = new VertexBuffer(m_GPUmem, vertexBufferSize, sizeof(Vertex));
		m_vertexBuffer->Initialize();
		m_vertexBuffer->copyData(triangleVertices);

		m_indexBuffer = new IndexBuffer(m_GPUmem, indexBufferSize);
		m_indexBuffer->Initialize();
		m_indexBuffer->copyData(index_list);

	}

	void DXCore::RecordCommandList() 
	{
		ID3D12GraphicsCommandList* list = m_commandList->GetCommandList();
		// Record Start
		m_commandList->Reset();
		list->SetGraphicsRootSignature(m_rootSignature.Get());
		list->RSSetViewports(1, &m_viewport);
		list->RSSetScissorRects(1, &m_scissorRect);
		list->SetPipelineState(pso->GetPSO());


		// Indicate that the back buffer will be used as a render target.
		list->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_renderTargets[m_frameIndex].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));

		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_rtvHeap->GetCPUDescriptorHandleForHeapStart(),m_frameIndex, m_rtvDescriptorSize);
		list->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);

		// Record commands.
		const float clearColor[] = { 0.0f, 0.2f, 0.4f, 1.0f };
		list->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
		list->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		list->IASetVertexBuffers(0, 1, m_vertexBuffer->GetBufferView());
		list->IASetIndexBuffer(m_indexBuffer->GetIndexView());
		list->DrawIndexedInstanced(6, 1, 0, 0, 0);

		// Indicate that the back buffer will now be used to present.
		list->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_renderTargets[m_frameIndex].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));

		m_commandList->Close();
	}

	void DXCore::Render() {
		RecordCommandList();
		uint64_t fenceValue = m_commandQueue->Execute(m_commandList->GetCommandList());
		m_commandQueue->WaitCPU(fenceValue);	
		ThrowIfFailed(m_swapChain->Present(1, 0));
		m_frameIndex = m_swapChain->GetCurrentBackBufferIndex();
	}
}