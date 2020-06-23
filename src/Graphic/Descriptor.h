#pragma once

#include "GPUBuffer.h"
#include "DescriptorHeap.h"

#define ptrVertexBuffer std::shared_ptr<Graphic::VertexBuffer>
#define ptrIndexBuffer std::shared_ptr<Graphic::IndexBuffer>
#define ptrCBV std::shared_ptr<Graphic::ConstantBuffer>
#define ptrSRV std::shared_ptr<Graphic::ShaderResource>
#define ptrUAV std::shared_ptr<Graphic::UnorderedAccess>

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


		ID3D12Device* device = Engine::GetDevice();
		// Free threaded as long as different threads don't write to a same place
		device->CopyDescriptorsSimple(size, destCPUHandle, srcCPUHandle, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	}


	// Most Descriptor here shouldn't be used directly, use Mesh & Material class instead
	// Descriptors don't need to know about GPU Heap, GPU Buffer should handle all the memory stuff
	class Descriptor {
	public:
		Descriptor(ptrGPUMem gpubuffer, const UINT bufferSize)
			: m_Buffer(gpubuffer), m_BufferSize(bufferSize) {}

		inline void copyData(const void* data, UINT size) { Engine::MemoryAllocator.UploadData(*m_Buffer, data, size, m_Offset); }
		inline void copyData(const void* data) { copyData(data, m_BufferSize); }

		inline UINT GetSize() { return m_BufferSize; }

	protected:
		ptrGPUMem m_Buffer;
		UINT m_BufferSize;
		UINT m_Offset;
	};

	// TODO fix gpubuffer size
	// Base class for those view who use descriptor heap to bind 
	class HeapDescriptor : public Descriptor {
	public:
		HeapDescriptor(ptrGPUMem gpubuffer, const UINT bufferSize)
			: Descriptor(gpubuffer, bufferSize) {}

		inline UINT GetHeapIndex() const { return m_HeapIndex; }

		// Simple Bind function, just bind one descriptor to InUse heap (Use Graphics::BindMultiDescriptor instead)
		// Copy this descriptor to the in use descriptor heap for rendering
		inline void BindDescriptor(UINT destIndex) const
		{
			BindMultiDescriptor(m_HeapIndex, 1, destIndex);
		}

		inline void CopyTexture(D3D12_SUBRESOURCE_DATA* textureData)
		{
			Engine::MemoryAllocator.UploadTexure(*m_Buffer, textureData);
		}

	protected:
		// TODO seem useless if we required every Heap Descriptor is used through DescriptorTable
		UINT m_HeapIndex;
	};


	// Manage a list of Heap descriptors
	// This class malloc n slots from the descriptor heap
	// Should only be used in one thread
	class DescriptorTable {

	public:
		DescriptorTable(UINT size)  : m_size(size) { }

		void Initialize(DescriptorHeap* heap) 
		{
			m_heap = heap;
			m_heapIndexStart = m_heap->MallocHeap(m_size);
		}

		// Only Bind SRV & UAV & CBV
		inline CD3DX12_GPU_DESCRIPTOR_HANDLE BindDescriptorTable() const
		{
			assert(m_heap && "Descriptor Table havn't initialized");
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


/*****************************************************************

**		Implementation of different Descriptors below			**

******************************************************************/


	class VertexBuffer : public Descriptor {
	public:
		VertexBuffer(ptrGPUMem gpubuffer, const UINT bufferSize, const UINT strideSize);
		
		const D3D12_VERTEX_BUFFER_VIEW* GetBufferView() const { return &m_view; }

		inline UINT GetStrideSize() const { return m_strideSize; }

	private:
		const UINT m_strideSize;
		D3D12_VERTEX_BUFFER_VIEW m_view;
	};


	class IndexBuffer : public Descriptor  {
	public:
		IndexBuffer(ptrGPUMem gpubuffer, const UINT bufferSize);
		
		// Suballocate from a index buffer
		// Example use:
		//		1. Create an large index buffer store everything
		//		2. Suballocate from the large one by calling this constructor
		//      3. When rendering, we're going to use the m_start variable
		IndexBuffer(IndexBuffer& buffer, const UINT start, const UINT end);

		const D3D12_INDEX_BUFFER_VIEW* GetIndexView() const { return &m_view; }
		
	private:
		UINT m_start;
		D3D12_INDEX_BUFFER_VIEW m_view;
	};


	class ConstantBuffer : public HeapDescriptor {
	public:
		// Create a CBV, descriptor Heap is nullptr if we are creating a normal cbv, 
		// if provide decriptorHeap, we are creaing a root CBV
		ConstantBuffer(ptrGPUMem gpubuffer, const UINT bufferSize, bool isRoot=false);
		ConstantBuffer(ptrGPUMem gpubuffer, const UINT bufferSize, DescriptorTable& table, UINT tableIndex);
		inline D3D12_GPU_VIRTUAL_ADDRESS GetRootCBVGPUAdder() const 
		{
			assert(m_isRootCBV && "CBV should be a root CBV to call this function");
			return m_cbvDesc.BufferLocation; 
		}

	private:
		bool m_isRootCBV;
		D3D12_CONSTANT_BUFFER_VIEW_DESC m_cbvDesc;
	};


	class ShaderResource : public HeapDescriptor {
	public:
		ShaderResource(ptrGPUMem gpubuffer, const D3D12_SHADER_RESOURCE_VIEW_DESC& desc);
		// Create the SRV in descriptor table's tableIndex slot
		ShaderResource(ptrGPUMem gpubuffer, const D3D12_SHADER_RESOURCE_VIEW_DESC& desc, 
						DescriptorTable& table, UINT tableIndex);

	private:
		// Create SRV on the init descriptor heap
		void Initialize(ptrGPUMem gpubuffer, D3D12_CPU_DESCRIPTOR_HANDLE handle);

		D3D12_SHADER_RESOURCE_VIEW_DESC m_srvDesc;
	};



	class UnorderedAccess : public HeapDescriptor {
	public:
		UnorderedAccess(ptrGPUMem gpubuffer, const D3D12_UNORDERED_ACCESS_VIEW_DESC& desc);
		UnorderedAccess(ptrGPUMem gpubuffer, const D3D12_UNORDERED_ACCESS_VIEW_DESC& desc,
						DescriptorTable& table, UINT tableIndex);
	private:
		// Create UAV on the init descriptor heap
		void Initialize(ptrGPUMem gpubuffer, D3D12_CPU_DESCRIPTOR_HANDLE handle);

		D3D12_UNORDERED_ACCESS_VIEW_DESC m_uavDesc;
	};

	// TODO add Decriptor table constructor
	class RenderTarget : public HeapDescriptor {
	public:
		RenderTarget(ptrGPUMem gpubuffer, const D3D12_RENDER_TARGET_VIEW_DESC& desc, DescriptorHeap* descriptorHeap);
		
	private:
		D3D12_RENDER_TARGET_VIEW_DESC m_rtvDesc;

	};


	class DepthStencil : public HeapDescriptor {
	public:
		DepthStencil(ptrGPUMem gpubuffer, const D3D12_DEPTH_STENCIL_VIEW_DESC & desc, DescriptorHeap* descriptorHeap);

		inline CD3DX12_CPU_DESCRIPTOR_HANDLE GetDSVCPUHandle() const { return m_dsvHandle; }

	private:
		D3D12_DEPTH_STENCIL_VIEW_DESC m_dsvDesc;
		CD3DX12_CPU_DESCRIPTOR_HANDLE m_dsvHandle;
	};
}
