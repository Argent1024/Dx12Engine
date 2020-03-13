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

	void RootConstantBuffer::Initialize() 
	{
		ID3D12Device* device = Engine::GetDevice();
		m_Offset = m_Buffer->MemAlloc(m_BufferSize);
		m_descriptorHeap = new DescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 1, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE);
		m_descriptorHeap->Initialize();
		UINT HeapIndex = m_descriptorHeap->MallocHeap();
		assert(m_Offset == 0 && "I believe there should be a new gpumem for Root CBV?");
		assert(HeapIndex == 0 && "I believe there should be a new descriptor heap for a root cbv");
		assert(m_BufferSize % 256 == 0 && "Constant buffer size not aligned");
		m_cbvDesc.BufferLocation = m_Buffer->GetGPUAddr() + m_Offset;
		m_cbvDesc.SizeInBytes = m_BufferSize;  
		device->CreateConstantBufferView(&m_cbvDesc, m_descriptorHeap->GetCPUHandle(HeapIndex));
	}

	void ConstantBuffer::Initialize() {
		assert(m_BufferSize % 256 == 0 && "Constant buffer size not aligned");
		ID3D12Device* device = Engine::GetDevice();
		DescriptorHeap* descriptorHeap = Engine::GetInitHeap();
		m_Offset = m_Buffer->MemAlloc(m_BufferSize);
		m_HeapIndex = descriptorHeap->MallocHeap();

		m_cbvDesc.BufferLocation = m_Buffer->GetGPUAddr() + m_Offset;
		m_cbvDesc.SizeInBytes = m_BufferSize;  
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