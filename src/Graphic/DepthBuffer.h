#pragma once

#include "GraphicCore.h"
#include "DescriptorHeap.h"

namespace Graphic 
{
	class DepthBuffer {
	public:
		DepthBuffer(const UINT width, const UINT height);

		inline CD3DX12_CPU_DESCRIPTOR_HANDLE GetDepthBufferCPUHandle() const { return m_dsvHeap.GetCPUHandle(0); }

		
	private:
		const UINT m_width;
		const UINT m_height;

		ComPtr<ID3D12Resource> m_depthStencil;

		DescriptorHeap m_dsvHeap;
		D3D12_DEPTH_STENCIL_VIEW_DESC m_depthStencilDesc;
		D3D12_CLEAR_VALUE m_depthOptimizedClearValue;
	};


}