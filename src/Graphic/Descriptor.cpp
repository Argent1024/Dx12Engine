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

	void ConstantBuffer::Initialize() {
		ID3D12Device* device = Engine::GetDevice();
		DescriptorHeap* descriptorHeap = Engine::GetInitHeap();
		m_Offset = m_Buffer->MemAlloc(m_BufferSize);
		m_HeapIndex = descriptorHeap->MallocHeap();

		m_cbvDesc.BufferLocation = m_Buffer->GetGPUAddr() + m_Offset;
		m_cbvDesc.SizeInBytes = (m_BufferSize+ 255) & ~255;  
		device->CreateConstantBufferView(&m_cbvDesc, descriptorHeap->GetCPUHandle(m_HeapIndex));
	}	

	void ShaderResource::Initialize() {
		ID3D12Device* device = Engine::GetDevice();
		DescriptorHeap* descriptorHeap = Engine::GetInitHeap();
		m_Offset = m_Buffer->MemAlloc(m_BufferSize);
		m_HeapIndex = descriptorHeap->MallocHeap();
		device->CreateShaderResourceView(m_Buffer->GetResource(), &m_srvDesc, descriptorHeap->GetCPUHandle(m_HeapIndex));
	}

	
	void UnorderedAccess::Initialize() {
		ID3D12Device* device = Engine::GetDevice();
		DescriptorHeap* descriptorHeap = Engine::GetInitHeap();
		m_Offset = m_Buffer->MemAlloc(m_BufferSize);
		m_HeapIndex = descriptorHeap->MallocHeap();
		device->CreateUnorderedAccessView(m_Buffer->GetResource(), nullptr, &m_uavDesc, descriptorHeap->GetCPUHandle(m_HeapIndex));
	}


}