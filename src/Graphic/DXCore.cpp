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

		// Create DX12 Command Queue
		void DXCore::CreateQueue() {
			D3D12_COMMAND_QUEUE_DESC queueDesc = {};
			queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
			queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

			ThrowIfFailed(m_device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_commandQueue)));
		
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
				m_commandQueue.Get(),        // Swap chain needs the queue so that it can force a flush on it.
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
			ThrowIfFailed(m_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_commandAllocator)));
		}

	

	void DXCore::InitPipeline(const HWND t_appHwnd) {
#ifdef DEBUG
		EnableDebug();
#endif
		// Create factory
		ThrowIfFailed(CreateDXGIFactory1(IID_PPV_ARGS(&m_factory)));
		CreateDevice();
		CreateQueue();
		CreateSwapChain(t_appHwnd);
		CreateDescriptorHeap();
		CreateRTV();
	}

	void DXCore::Init(const HWND t_appHwnd) {
		InitPipeline(t_appHwnd);
		//TODO Create Resource???
	}

}