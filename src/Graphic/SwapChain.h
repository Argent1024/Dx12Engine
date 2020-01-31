#pragma once

#include "DXHelper.h"
#include "DescriptorHeap.h"
#include "Descriptor.h"

namespace Graphic {
	class SwapChain {
	public:
		static const D3D12_DESCRIPTOR_HEAP_TYPE HeapType = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		static const UINT FrameCount = 2;

		SwapChain(const HWND appHwnd, const UINT width, const UINT height);

		void Initialize(ComPtr<IDXGIFactory4> factory, ComPtr<ID3D12Device> device, ID3D12CommandQueue* commandQueue);
		
		// Return the handle of the back buffer
		inline CD3DX12_CPU_DESCRIPTOR_HANDLE GetBackBufferCPUHandle() const { return m_rtvHeap.GetCPUHandle(m_BackBufferIndex); }

		inline void Present(UINT SyncInterval = 1, UINT Flags = 0);

		//TODO Resource Barrier
		ComPtr<ID3D12Resource> m_renderTargets[FrameCount];
		UINT m_BackBufferIndex;

	private:
		const UINT m_width;
		const UINT m_height;
		const HWND m_appHwnd;

		ComPtr<IDXGISwapChain3> m_swapChain;
		DXGI_SWAP_CHAIN_DESC1 m_swapChainDesc;
		
		DescriptorHeap m_rtvHeap;
	};

	// TODO multi threading lock here?? But it seems only main thread will call this
	void SwapChain::Present(UINT SyncInterval, UINT Flags) {
		ThrowIfFailed(m_swapChain->Present(SyncInterval, Flags));
		m_BackBufferIndex = m_swapChain->GetCurrentBackBufferIndex();
	}
}

