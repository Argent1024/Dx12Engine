#include "Descriptor.h"

namespace Graphic {
	VertexBuffer::VertexBuffer(ptrGPUMem gpubuffer, const UINT bufferSize, const UINT strideSize) 
		:Descriptor(gpubuffer, bufferSize), m_strideSize(strideSize) 
	{
		m_Offset = m_Buffer->MemAlloc(m_BufferSize);
		m_view.BufferLocation = m_Buffer->GetGPUAddr() + m_Offset;
		m_view.StrideInBytes = m_strideSize;
		m_view.SizeInBytes = m_BufferSize;
	}

	IndexBuffer::IndexBuffer(ptrGPUMem gpubuffer, const UINT bufferSize)
		: Descriptor(gpubuffer, bufferSize) 
	{
		m_Offset = m_Buffer->MemAlloc(m_BufferSize);
		m_view.BufferLocation = m_Buffer->GetGPUAddr() + m_Offset;
		m_view.Format = DXGI_FORMAT_R32_UINT;
		m_view.SizeInBytes = m_BufferSize;
	}

	RootConstantBuffer::RootConstantBuffer(ptrGPUMem gpubuffer, const UINT bufferSize)
		: Descriptor(gpubuffer, CalculateConstantBufferByteSize(bufferSize)) 
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

	ConstantBuffer::ConstantBuffer(ptrGPUMem gpubuffer, const UINT bufferSize,  DescriptorHeap* descriptorHeap)
		: HeapDescriptor(gpubuffer, CalculateConstantBufferByteSize(bufferSize)),
		  m_isRootCBV(descriptorHeap != nullptr)
	{
		assert(m_BufferSize % 256 == 0 && "Constant buffer size not aligned");
		ID3D12Device* device = Engine::GetDevice();

		if (m_isRootCBV) {
			D3D12_DESCRIPTOR_HEAP_TYPE  type = descriptorHeap->GetDescriptorHeapType();
			D3D12_DESCRIPTOR_HEAP_FLAGS flags = descriptorHeap->GetDescriptorHeapFlags();
			assert(type == D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV && "descriptor provide type dismatch!");
			assert(flags == D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE && "Root CBV should be shader visible");
		} else {
			descriptorHeap = Engine::GetInitHeap();
		}

		m_Offset = m_Buffer->MemAlloc(m_BufferSize);
		m_HeapIndex = descriptorHeap->MallocHeap();

		m_cbvDesc.BufferLocation = m_Buffer->GetGPUAddr() + m_Offset;
		m_cbvDesc.SizeInBytes = m_BufferSize;  
		device->CreateConstantBufferView(&m_cbvDesc, descriptorHeap->GetCPUHandle(m_HeapIndex));
	}	


	ShaderResource::ShaderResource(ptrGPUMem gpubuffer, const D3D12_SHADER_RESOURCE_VIEW_DESC& desc) 
		: m_srvDesc(desc), HeapDescriptor(gpubuffer, 0)
	{
		ID3D12Device* device = Engine::GetDevice();
		DescriptorHeap* descriptorHeap = Engine::GetInitHeap();
		m_Offset = m_Buffer->MemAlloc(m_BufferSize);
		m_HeapIndex = descriptorHeap->MallocHeap();
		device->CreateShaderResourceView(m_Buffer->GetResource(), &m_srvDesc, descriptorHeap->GetCPUHandle(m_HeapIndex));
	}

	
	UnorderedAccess::UnorderedAccess(ptrGPUMem gpubuffer, const D3D12_UNORDERED_ACCESS_VIEW_DESC& desc)
		: m_uavDesc(desc), HeapDescriptor(gpubuffer, 0) 
	{
		ID3D12Device* device = Engine::GetDevice();
		DescriptorHeap* descriptorHeap = Engine::GetInitHeap();
		m_Offset = m_Buffer->MemAlloc(m_BufferSize);
		m_HeapIndex = descriptorHeap->MallocHeap();
		device->CreateUnorderedAccessView(m_Buffer->GetResource(), nullptr, &m_uavDesc, descriptorHeap->GetCPUHandle(m_HeapIndex));
	}


}