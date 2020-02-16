#include "DepthBuffer.h"

namespace Graphic 
{
	DepthBuffer::DepthBuffer(const UINT width, const UINT height)
		: m_width(width), m_height(height),
		  m_dsvHeap(D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 1, D3D12_DESCRIPTOR_HEAP_FLAG_NONE)
	{
		m_dsvHeap.Initialize();

		m_depthStencilDesc = {};
		m_depthStencilDesc.Format = DXGI_FORMAT_D32_FLOAT;
        m_depthStencilDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
        m_depthStencilDesc.Flags = D3D12_DSV_FLAG_NONE;

		m_depthOptimizedClearValue = {};
		m_depthOptimizedClearValue.Format = DXGI_FORMAT_D32_FLOAT;
        m_depthOptimizedClearValue.DepthStencil.Depth = 1.0f;
        m_depthOptimizedClearValue.DepthStencil.Stencil = 0;
	
		ID3D12Device* device = Engine::GetDevice();
		ThrowIfFailed(device->CreateCommittedResource(
            &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
            D3D12_HEAP_FLAG_NONE,
            &CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_D32_FLOAT, m_width, m_height, 1, 0, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL),
            D3D12_RESOURCE_STATE_DEPTH_WRITE,
            &m_depthOptimizedClearValue,
            IID_PPV_ARGS(&m_depthStencil)
        ));
		NAME_D3D12_OBJECT(m_depthStencil);
		device->CreateDepthStencilView(m_depthStencil.Get(), &m_depthStencilDesc, m_dsvHeap.GetCPUHandle(0));
	}

}