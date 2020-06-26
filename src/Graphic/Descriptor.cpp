#include "Descriptor.h"

namespace Graphic {
	VertexBuffer::VertexBuffer(ptrGPUMem gpubuffer, const UINT bufferSize, const UINT strideSize) 
		:Descriptor(gpubuffer, bufferSize), m_strideSize(strideSize) 
	{ }

	void VertexBuffer::Initialize() 
	{
		m_Offset = m_Buffer->MemAlloc(m_BufferSize);
		m_view.BufferLocation = m_Buffer->GetGPUAddr() + m_Offset;
		m_view.StrideInBytes = m_strideSize;
		m_view.SizeInBytes = m_BufferSize;
	}

	IndexBuffer::IndexBuffer(ptrGPUMem gpubuffer, const UINT bufferSize)
		: Descriptor(gpubuffer, bufferSize), m_start(0) { }

	void IndexBuffer::Initialize() 
	{
		m_Offset = m_Buffer->MemAlloc(m_BufferSize);
		m_view.BufferLocation = m_Buffer->GetGPUAddr() + m_Offset;
		m_view.Format = DXGI_FORMAT_R32_UINT;
		m_view.SizeInBytes = m_BufferSize;
	}

	/*// TODO check whether buffersize & start is correct
	IndexBuffer::IndexBuffer(IndexBuffer& buffer, const UINT start, const UINT end)
		: Descriptor(buffer.m_Buffer, (end - start) * sizeof(UINT)), m_start(start) // bufferSize = (end - start) * sizeof(UINT)
	{
		assert(start + m_BufferSize <= buffer.m_BufferSize && "start position plus bufferSize too large");
		m_Offset = buffer.m_Offset + start;
		m_view.BufferLocation = m_Buffer->GetGPUAddr() + m_Offset;
		m_view.Format = DXGI_FORMAT_R32_UINT;
		m_view.SizeInBytes = m_BufferSize;
	}*/

	ConstantBuffer::ConstantBuffer(ptrGPUMem gpubuffer, const UINT bufferSize)
		: HeapDescriptor(gpubuffer, CalculateConstantBufferByteSize(bufferSize))
	{ assert(m_BufferSize % 256 == 0 && "Constant buffer size not aligned"); }	

	void ConstantBuffer::Initialize() 
	{
		m_Offset = m_Buffer->MemAlloc(m_BufferSize);

		m_cbvDesc.BufferLocation = m_Buffer->GetGPUAddr() + m_Offset;
		m_cbvDesc.SizeInBytes = m_BufferSize;  
	}

	void ConstantBuffer::CreateView(DescriptorTable& table, UINT slot) 
	{
		ID3D12Device* device = Engine::GetDevice();
		D3D12_CPU_DESCRIPTOR_HANDLE handle = table.GetSlot(slot);
		device->CreateConstantBufferView(&m_cbvDesc, handle);
	}

	void ConstantBuffer::CreateRootView() 
	{
		assert(m_RootHeapIndex==-1 && "Root View should be created only once");
		ID3D12Device* device = Engine::GetDevice();
		DescriptorHeap* initheap = Engine::GetInitHeap();
		m_RootHeapIndex = initheap->MallocHeap();
		device->CreateConstantBufferView(&m_cbvDesc, initheap->GetCPUHandle(m_RootHeapIndex));
	}

	/************************* Shader Resource Begin ******************************/
	// TODO fix buffer size
	ShaderResource::ShaderResource(ptrGPUMem gpubuffer, const D3D12_SHADER_RESOURCE_VIEW_DESC& desc) 
		: m_srvDesc(desc), HeapDescriptor(gpubuffer, 0)
	{ }
	
	void ShaderResource::Initialize() 
	{
		m_Offset = m_Buffer->MemAlloc(m_BufferSize);
	}

	void ShaderResource::CreateView(DescriptorTable& table, UINT slot) 
	{
		ID3D12Device* device = Engine::GetDevice();
		D3D12_CPU_DESCRIPTOR_HANDLE handle = table.GetSlot(slot);
		device->CreateShaderResourceView(m_Buffer->GetResource(), &m_srvDesc, handle);
	}

	void ShaderResource::CreateRootView() 
	{
		assert(m_RootHeapIndex==-1 && "Root View should be created only once");
		ID3D12Device* device = Engine::GetDevice();
		DescriptorHeap* initheap = Engine::GetInitHeap();
		m_RootHeapIndex = initheap->MallocHeap();
		D3D12_CPU_DESCRIPTOR_HANDLE handle = initheap->GetCPUHandle(m_RootHeapIndex);
		device->CreateShaderResourceView(m_Buffer->GetResource(), &m_srvDesc, handle);
	}
	/************************* Shader Resource End ******************************/


	/*************************   UnorderedAccess Resource Begin ******************************/
	UnorderedAccess::UnorderedAccess(ptrGPUMem gpubuffer, const D3D12_UNORDERED_ACCESS_VIEW_DESC& desc)
		: m_uavDesc(desc), HeapDescriptor(gpubuffer, 0) 
	{
	}

	void UnorderedAccess::Initialize() 
	{
		m_Offset = m_Buffer->MemAlloc(m_BufferSize);
	}

	void UnorderedAccess::CreateView(DescriptorTable& table, UINT slot) 
	{
		ID3D12Device* device = Engine::GetDevice();
		D3D12_CPU_DESCRIPTOR_HANDLE handle = table.GetSlot(slot);
		device->CreateUnorderedAccessView(m_Buffer->GetResource(), nullptr, &m_uavDesc, handle);
	}

	void UnorderedAccess::CreateRootView()
	{
		assert(m_RootHeapIndex==-1 && "Root View should be created only once");
		ID3D12Device* device = Engine::GetDevice();
		DescriptorHeap* initheap = Engine::GetInitHeap();
		m_RootHeapIndex = initheap->MallocHeap();
		D3D12_CPU_DESCRIPTOR_HANDLE handle = initheap->GetCPUHandle(m_RootHeapIndex);
		device->CreateUnorderedAccessView(m_Buffer->GetResource(), nullptr, &m_uavDesc, handle);
	}

	/************************* UnorderedAccess Resource End ******************************/


	RenderTarget::RenderTarget(ptrGPUMem gpubuffer, const D3D12_RENDER_TARGET_VIEW_DESC& desc, DescriptorHeap* descriptorHeap) 
		: m_rtvDesc(desc), m_DescriptorHeap(descriptorHeap), HeapDescriptor(gpubuffer, 0) 
	{ }

	void RenderTarget::Initialize() 
	{
		m_Offset = m_Buffer->MemAlloc(m_BufferSize);
	}

	void RenderTarget::CreateRootView() 
	{
		assert(m_RootHeapIndex==-1 && "Root View should be created only once");
		ID3D12Device* device = Engine::GetDevice();
		m_RootHeapIndex = m_DescriptorHeap->MallocHeap();
		device->CreateRenderTargetView(m_Buffer->GetResource(), &m_rtvDesc, m_DescriptorHeap->GetCPUHandle(m_RootHeapIndex));
	}


	DepthStencil::DepthStencil(ptrGPUMem gpubuffer, const D3D12_DEPTH_STENCIL_VIEW_DESC & desc, DescriptorHeap* descriptorHeap)
		: m_dsvDesc(desc), m_DescriptorHeap(descriptorHeap), HeapDescriptor(gpubuffer, 0)
	{ }

	void DepthStencil::Initialize() 
	{
		m_Offset = m_Buffer->MemAlloc(m_BufferSize);
	}

	void DepthStencil::CreateRootView() 
	{
		assert(m_RootHeapIndex==-1 && "Root View should be created only once");
		ID3D12Device* device = Engine::GetDevice();
		m_RootHeapIndex = m_DescriptorHeap->MallocHeap();
		device->CreateDepthStencilView(m_Buffer->GetResource(), &m_dsvDesc, m_DescriptorHeap->GetCPUHandle(m_RootHeapIndex));
		m_dsvHandle = m_DescriptorHeap->GetCPUHandle(m_RootHeapIndex);
	}
}