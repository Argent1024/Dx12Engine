#include "Descriptor.h"

namespace Graphic {
	void VertexBuffer::Initialize() {
		m_Offset = m_Buffer->MemAlloc(m_BufferSize);
		m_view.BufferLocation = m_Buffer->GetGPUAddr() + m_Offset;
		m_view.StrideInBytes = m_strideSize;
		m_view.SizeInBytes = m_BufferSize;
	}

	void IndexBuffer::Initialize() {
		m_Offset = m_Buffer->MemAlloc(m_BufferSize);
		m_view.BufferLocation = m_Buffer->GetGPUAddr() + m_Offset;
		m_view.Format = DXGI_FORMAT_R32_UINT;
		m_view.SizeInBytes = m_BufferSize;
	}

	void ShaderResource::Initialize(ComPtr<ID3D12Device> device) {
		m_Offset = m_Buffer->MemAlloc(m_BufferSize);
		m_HeapIndex = m_descriptorHeap->MallocHeap();

		m_Buffer->CreateSRV(device, &m_srvDesc, m_descriptorHeap->GetCPUHandle(m_HeapIndex));
	}	

	void ConstantBuffer::Initialize(ComPtr<ID3D12Device> device) {
		m_Offset = m_Buffer->MemAlloc(m_BufferSize);
		m_HeapIndex = m_descriptorHeap->MallocHeap();

		m_cbvDesc.BufferLocation = m_Buffer->GetGPUAddr() + m_Offset;
		m_cbvDesc.SizeInBytes = (m_BufferSize+ 255) & ~255;  
		device->CreateConstantBufferView(&m_cbvDesc, m_descriptorHeap->GetCPUHandle(m_HeapIndex));
	}	
}