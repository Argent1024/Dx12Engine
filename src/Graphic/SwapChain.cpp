#include "SwapChain.h"

namespace Graphic {
	SwapChain::SwapChain(const HWND appHwnd, const UINT width, const UINT height) 
		: m_appHwnd(appHwnd), m_width(width), m_height(height), 
		  m_rtvHeap(HeapType, FrameCount)
	{
	
	}

	void SwapChain::Initialize(ComPtr<IDXGIFactory4> factory, ComPtr<ID3D12Device> device, ID3D12CommandQueue* commandQueue) {
		{
			// Create RTV descriptor heap
			m_rtvHeap.Initialize(device);
		}

		{
			// Create Swap Chain
			m_swapChainDesc.BufferCount = FrameCount;
			m_swapChainDesc.Width = m_width;
			m_swapChainDesc.Height = m_height;
			m_swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			m_swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
			m_swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
			m_swapChainDesc.SampleDesc.Count = 1;

			ComPtr<IDXGISwapChain1> swapChain;
			ThrowIfFailed(factory->CreateSwapChainForHwnd(
				commandQueue,   // Swap chain needs the queue so that it can force a flush on it.
				m_appHwnd,	    // The window handle in os
				&m_swapChainDesc,
				nullptr,
				nullptr,
				&swapChain
			));

			ThrowIfFailed(factory->MakeWindowAssociation(m_appHwnd, DXGI_MWA_NO_ALT_ENTER));
			ThrowIfFailed(swapChain.As(&m_swapChain));
			m_BackBufferIndex = m_swapChain->GetCurrentBackBufferIndex();
			
		}

		{
			// Create RTV for each frame
			CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle;
			for (UINT n = 0; n < FrameCount; ++n) {
				rtvHandle = m_rtvHeap.GetCPUHandle(n);
				ThrowIfFailed(m_swapChain->GetBuffer(n, IID_PPV_ARGS(&m_renderTargets[n])));
				device->CreateRenderTargetView(m_renderTargets[n].Get(), nullptr, rtvHandle);
			}
		}
	}

}