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
		: Descriptor(gpubuffer, bufferSize), m_start(0)
	{
		m_Offset = m_Buffer->MemAlloc(m_BufferSize);
		m_view.BufferLocation = m_Buffer->GetGPUAddr() + m_Offset;
		m_view.Format = DXGI_FORMAT_R32_UINT;
		m_view.SizeInBytes = m_BufferSize;
	}

	// TODO check whether buffersize & start is correct
	IndexBuffer::IndexBuffer(IndexBuffer& buffer, const UINT start, const UINT end)
		: Descriptor(buffer.m_Buffer, (end - start) * sizeof(UINT)) // bufferSize = (end - start) * sizeof(UINT)
	{
		assert(start + m_BufferSize <= buffer.m_BufferSize && "start position plus bufferSize too large");
		m_Offset = buffer.m_Offset + start;
		m_view.BufferLocation = m_Buffer->GetGPUAddr() + m_Offset;
		m_view.Format = DXGI_FORMAT_R32_UINT;
		m_view.SizeInBytes = m_BufferSize;
	}

	ConstantBuffer::ConstantBuffer(ptrGPUMem gpubuffer, const UINT bufferSize,  bool isRoot)
		: HeapDescriptor(gpubuffer, CalculateConstantBufferByteSize(bufferSize)), m_isRootCBV(isRoot)
	{
		assert(m_BufferSize % 256 == 0 && "Constant buffer size not aligned");
		ID3D12Device* device = Engine::GetDevice();

		/*if (m_isRootCBV) {
			D3D12_DESCRIPTOR_HEAP_TYPE  type = descriptorHeap->GetDescriptorHeapType();
			D3D12_DESCRIPTOR_HEAP_FLAGS flags = descriptorHeap->GetDescriptorHeapFlags();
			assert(type == D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV && "descriptor provide type dismatch!");
			assert(flags == D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE && "Root CBV should be shader visible");
		} else {
			descriptorHeap = Engine::GetInitHeap();
		}*/
		DescriptorHeap* descriptorHeap = Engine::GetInitHeap();

		m_Offset = m_Buffer->MemAlloc(m_BufferSize);
		m_HeapIndex = descriptorHeap->MallocHeap();

		m_cbvDesc.BufferLocation = m_Buffer->GetGPUAddr() + m_Offset;
		m_cbvDesc.SizeInBytes = m_BufferSize;  
		device->CreateConstantBufferView(&m_cbvDesc, descriptorHeap->GetCPUHandle(m_HeapIndex));
	}	

	// TODO fix buffer size
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
		// TODO what's the parameter
		device->CreateUnorderedAccessView(m_Buffer->GetResource(), nullptr, &m_uavDesc, descriptorHeap->GetCPUHandle(m_HeapIndex));
	}


	RenderTarget::RenderTarget(ptrGPUMem gpubuffer, const D3D12_RENDER_TARGET_VIEW_DESC& desc, DescriptorHeap* descriptorHeap) 
		: m_rtvDesc(desc), HeapDescriptor(gpubuffer, 0) 
	{
		ID3D12Device* device = Engine::GetDevice();
		m_Offset = m_Buffer->MemAlloc(m_BufferSize);
		m_HeapIndex = descriptorHeap->MallocHeap();
		device->CreateRenderTargetView(m_Buffer->GetResource(), &m_rtvDesc, descriptorHeap->GetCPUHandle(m_HeapIndex));
	}

	DepthStencil::DepthStencil(ptrGPUMem gpubuffer, const D3D12_DEPTH_STENCIL_VIEW_DESC & desc, DescriptorHeap* descriptorHeap)
		: m_dsvDesc(desc), HeapDescriptor(gpubuffer, 0)
	{
		ID3D12Device* device = Engine::GetDevice();
		m_Offset = m_Buffer->MemAlloc(m_BufferSize);
		m_HeapIndex = descriptorHeap->MallocHeap();
		device->CreateDepthStencilView(m_Buffer->GetResource(), &m_dsvDesc, descriptorHeap->GetCPUHandle(m_HeapIndex));
	}

}