#pragma once

#include "DXHelper.h"
#include "DescriptorHeap.h"
#include "Descriptor.h"
#include "Texture.h"

namespace Graphic {
	// TODO going to rewrite this to coorporate with Texture class
	class SwapChain {
	public:
		static const D3D12_DESCRIPTOR_HEAP_TYPE HeapType = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		static const UINT FrameCount = 2;

		SwapChain(const HWND appHwnd, const UINT width, const UINT height);

		void Initialize(ID3D12CommandQueue* commandQueue);
		
		// Return the handle of the back buffer
		inline CD3DX12_CPU_DESCRIPTOR_HANDLE GetBackBufferCPUHandle() const { return m_rtvHeap.GetCPUHandle(m_BackBufferIndex); }
		inline ID3D12Resource* GetBackBufferResource() const { return m_renderTargets[m_BackBufferIndex].Get(); }

		inline void Present(UINT SyncInterval = 1, UINT Flags = 0);

		inline D3D12_RESOURCE_BARRIER Barrier(D3D12_RESOURCE_STATES before, D3D12_RESOURCE_STATES after) const 
		{
			return CD3DX12_RESOURCE_BARRIER::Transition(m_renderTargets[m_BackBufferIndex].Get(), before, after);
		}

		// Hack: Use Swapchain just as Texture class, make it easier for Render Pass to use  (going to change)
		void CreateSRV(DescriptorTable& table, UINT slot, UINT frame) 
		{
			D3D12_CPU_DESCRIPTOR_HANDLE handle = table.GetSlot(slot);
			
			ID3D12Device* device = Engine::GetDevice();
			ID3D12Resource* resource = m_renderTargets[frame].Get();

			D3D12_SHADER_RESOURCE_VIEW_DESC desc;
			desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
			desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING; // ?
			desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			desc.Texture2D.MipLevels = 1;
			desc.Texture2D.ResourceMinLODClamp = 0.0f;
			
			device->CreateShaderResourceView(resource, &desc, handle);
		}

	private:
		const UINT m_width;
		const UINT m_height;
		const HWND m_appHwnd;

		ComPtr<IDXGISwapChain3> m_swapChain;
		DXGI_SWAP_CHAIN_DESC1 m_swapChainDesc;
		
		ComPtr<ID3D12Resource> m_renderTargets[FrameCount];
		UINT m_BackBufferIndex;

		DescriptorHeap m_rtvHeap;
	};

	// TODO multi threading lock here?? But it seems only main thread will call this
	void SwapChain::Present(UINT SyncInterval, UINT Flags) {
		ThrowIfFailed(m_swapChain->Present(SyncInterval, Flags));
		m_BackBufferIndex = m_swapChain->GetCurrentBackBufferIndex();
	}
}

