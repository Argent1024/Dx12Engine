#pragma once

#include "GPUBuffer.h"
#include "DescriptorHeap.h"

#define ptrVertexBuffer std::shared_ptr<Graphic::VertexBuffer>
#define ptrIndexBuffer std::shared_ptr<Graphic::IndexBuffer>
#define ptrCBV std::shared_ptr<Graphic::ConstantBuffer>

using ptrGMem = std::shared_ptr<GPU::GPUMemory>;
using ptrGBuffer = std::shared_ptr<GPU::GPUBuffer>;
using ptrTBuffer = std::shared_ptr<GPU::Texturebuffer>;

namespace Graphic {
	// Only bind srv_uav_cbv
	// Copy descriptors from init heap to inuse heap
	// Make sure that in init heap, srcIndex .. srcIndex + size - 1 should be those we want to bind
	inline void BindMultiDescriptor(UINT srcIndex, UINT size, UINT destIndex) 
	{
		DescriptorHeap* InUseHeap = Engine::GetInUseHeap();
		CD3DX12_CPU_DESCRIPTOR_HANDLE destCPUHandle = InUseHeap->GetCPUHandle(destIndex);

		DescriptorHeap* InitHeap = Engine::GetInitHeap();
		CD3DX12_CPU_DESCRIPTOR_HANDLE srcCPUHandle = InitHeap->GetCPUHandle(srcIndex);


		ID3D12Device* device = ::Engine::GetDevice();
		// Free threaded as long as different threads don't write to a same place
		device->CopyDescriptorsSimple(size, destCPUHandle, srcCPUHandle, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	}

	// Manage a list of Heap descriptors
	// This class malloc n slots from the descriptor heap
	// Should only be used in one thread
	class DescriptorTable {

	public:
		DescriptorTable(UINT size, DescriptorHeap* heap=Engine::GetInitHeap())  
			: m_size(size), m_heap(heap) 
		{ 
			m_heapIndexStart = m_heap->MallocHeap(m_size);
		}

		// Only Bind SRV & UAV & CBV
		inline CD3DX12_GPU_DESCRIPTOR_HANDLE BindDescriptorTable() const
		{
			// assert(m_heap && "Descriptor Table havn't initialized");
			DescriptorHeap* InUseHeap = Engine::GetInUseHeap();
			UINT destHeapIndex = InUseHeap->MallocHeap(m_size);
			BindMultiDescriptor(m_heapIndexStart, m_size, destHeapIndex);
			return InUseHeap->GetGPUHandle(destHeapIndex);
		}

		inline D3D12_CPU_DESCRIPTOR_HANDLE GetSlot(UINT index) const 
		{
			assert(index < m_size && "Accessing index out of bound");
			return m_heap->GetCPUHandle(m_heapIndexStart + index);
		}

		inline DescriptorHeap* GetHeap() const {
			return m_heap;
		}

		inline UINT GetHeapIndex(UINT index) const 
		{
			assert(index < m_size && "Accessing index out of bound");
			return m_heapIndexStart + index;
		}

		inline UINT size() { return m_size; }

	private:
		DescriptorHeap* m_heap;

		UINT m_heapIndexStart;
		const UINT m_size;
	};

	// Most Descriptor here shouldn't be used directly, use Mesh & Material & Texture class instead
	class Descriptor 
	{

	public:
		Descriptor() { m_handle.ptr = D3D12_GPU_VIRTUAL_ADDRESS_NULL; }

		D3D12_CPU_DESCRIPTOR_HANDLE GetHandle() const { return m_handle; }

		// For debug/assert use
		virtual bool IsCreated() { return  m_handle.ptr != D3D12_GPU_VIRTUAL_ADDRESS_NULL;  }

		// Bind Root descriptor, not sure if useful
		/*inline void BindDescriptor(UINT destIndex) const
		{
			assert(m_RootHeapIndex != -1 && "Descriptor not created");
			BindMultiDescriptor(m_RootHeapIndex, 1, destIndex);
		}*/

	protected:
		D3D12_CPU_DESCRIPTOR_HANDLE m_handle;

	};


	// For 1d stuff: Constant, Vertex & index buffer
	class BufferDescriptor : public Descriptor {
	public:
		//void copyData(const void* data, UINT size);

		//inline void copyData(const void* data) { copyData(data, m_BufferSize); }
		UINT Offset() const { return m_Offset; }
	protected:
		UINT m_Offset;
	};


/*****************************************************************

**		Implementation of different Descriptors below			**

******************************************************************/


	class VertexBuffer : public BufferDescriptor {
	public:
		VertexBuffer() { }
		
		void CreateView(ptrGBuffer buffer, UINT size, UINT stride)
		{
			m_Offset = buffer->MemAlloc(size);
			m_view.BufferLocation = buffer->GetGPUAddr() + m_Offset;
			m_view.StrideInBytes = stride;
			m_view.SizeInBytes = size;
		}

		bool IsCreated() override { return m_view.SizeInBytes != 0; };

		const D3D12_VERTEX_BUFFER_VIEW* GetBufferView() const { return &m_view; }

		inline UINT GetStrideSize() const { return m_view.StrideInBytes; }

	private:
		D3D12_VERTEX_BUFFER_VIEW m_view;
	};


	class IndexBuffer : public BufferDescriptor  {
	public:
		IndexBuffer() { }

		// TODO: Not working after some update... and this comment is outdated after another update
		// Suballocate from a index buffer
		// Example use:
		//		1. Create an large index buffer store everything
		//		2. Suballocate from the large one by calling this constructor
		//      3. When rendering, we're going to use the m_start variable
		// IndexBuffer(IndexBuffer& buffer, const UINT start, const UINT end);

		void CreateView(ptrGBuffer buffer, const UINT size) 
		{
			m_Offset = buffer->MemAlloc(size);
			m_view.BufferLocation = buffer->GetGPUAddr() + m_Offset;
			m_view.Format = DXGI_FORMAT_R32_UINT;
			m_view.SizeInBytes = size;
		}

		bool IsCreated() override { return m_view.SizeInBytes != 0; };

		const D3D12_INDEX_BUFFER_VIEW* GetIndexView() const { return &m_view; }

	private:
		// UINT m_start;
		D3D12_INDEX_BUFFER_VIEW m_view;
	};


	class ConstantBuffer : public BufferDescriptor 
	{
	public:
		// Create a CBV, descriptor Heap is nullptr if we are creating a normal cbv, 
		// if provide decriptorHeap, we are creaing a root CBV
		ConstantBuffer() { }

		void CreateView(DescriptorTable* table, UINT slot, ptrGBuffer buffer, UINT size)
		{	
			size = CalculateConstantBufferByteSize(size);
			assert(size % 256 == 0 && "Constant buffer size not aligned");
			m_Offset = buffer->MemAlloc(size);
			m_cbvDesc.BufferLocation = buffer->GetGPUAddr() + m_Offset;
			m_cbvDesc.SizeInBytes = size;
			
			ID3D12Device* device = Engine::GetDevice();
			if (table) {
				m_handle = table->GetSlot(slot);
				device->CreateConstantBufferView(&m_cbvDesc, m_handle);
			}
			else {
				DescriptorHeap* initheap = Engine::GetInitHeap();
				UINT HeapIndex = initheap->MallocHeap();
				m_handle = initheap->GetCPUHandle(HeapIndex);
				device->CreateConstantBufferView(&m_cbvDesc, m_handle);
			}
		}
		// Method to use this CBV as a root CBV
		inline D3D12_GPU_VIRTUAL_ADDRESS GetRootCBVGPUAdder() const 
		{
			// assert(m_isRootCBV && "CBV should be a root CBV to call this function");
			return m_cbvDesc.BufferLocation; 
		}
	private:
		D3D12_CONSTANT_BUFFER_VIEW_DESC m_cbvDesc;
	};


	class ShaderResource : public Descriptor
	{
	public:
		void CreateView(DescriptorTable* table, UINT tableIndex, ID3D12Resource* resource, D3D12_SHADER_RESOURCE_VIEW_DESC* desc) 
		{
			ID3D12Device* device = Engine::GetDevice();
			if(table) {
				m_handle = table->GetSlot(tableIndex);
				device->CreateShaderResourceView(resource, desc, m_handle);
			} else {
				DescriptorHeap* initheap = Engine::GetInitHeap();
				UINT RootHeapIndex = initheap->MallocHeap();
				m_handle = initheap->GetCPUHandle(RootHeapIndex);
				device->CreateShaderResourceView(resource, desc, m_handle);
			}
		}
	};


	class UnorderedAccess : public Descriptor
	{
	public:
		void CreateView(DescriptorTable* table, UINT tableIndex, ID3D12Resource* resource, D3D12_UNORDERED_ACCESS_VIEW_DESC* desc) 
		{
			ID3D12Device* device = Engine::GetDevice();
			if(table) {
				m_handle = table->GetSlot(tableIndex);
				device->CreateUnorderedAccessView(resource, nullptr, desc, m_handle);
			} else {
				DescriptorHeap* initheap = Engine::GetInitHeap();
				UINT RootHeapIndex = initheap->MallocHeap();
				m_handle = initheap->GetCPUHandle(RootHeapIndex);
				device->CreateUnorderedAccessView(resource, nullptr, desc, m_handle);
			}
		}
	};


	class RenderTarget : public Descriptor
	{
	public:
		void CreateView(ID3D12Resource* resource, D3D12_RENDER_TARGET_VIEW_DESC* desc)
		{
			DescriptorHeap* heap = Engine::GetRTVHeap();
			ID3D12Device* device = Engine::GetDevice();

			UINT RootHeapIndex = heap->MallocHeap();
			m_handle = heap->GetCPUHandle(RootHeapIndex);

			device->CreateRenderTargetView(resource, desc, m_handle);
		}

		D3D12_CPU_DESCRIPTOR_HANDLE m_handle;
	};

	
	class DepthStencil : public Descriptor
	{
	public:
		void CreateView(ID3D12Resource* resource, D3D12_DEPTH_STENCIL_VIEW_DESC* desc)
		{
			DescriptorHeap* heap = Engine::GetDSVHeap();
			ID3D12Device* device = Engine::GetDevice();

			UINT RootHeapIndex = heap->MallocHeap();
			m_handle = heap->GetCPUHandle(RootHeapIndex);

			device->CreateDepthStencilView(resource, desc, m_handle);
		}
	};

}
