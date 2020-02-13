#pragma once

#include "GPUBuffer.h"
#include "DescriptorHeap.h"


namespace Graphic {

	// Most Descriptor here shouldn't be used directly, use Mesh & Material class instead
	// Descriptors don't need to know about GPU Heap, GPU Buffer should handle all the memory stuff
	class Descriptor {
	public:
		Descriptor(ptrGPUMem gpubuffer, const UINT bufferSize)
			: m_Buffer(gpubuffer), m_BufferSize(bufferSize) {}

		inline void copyData(const void* data) { Engine::MemoryAllocator.UploadData(*m_Buffer, data, m_BufferSize, m_Offset); }

		inline UINT GetSize() { return m_BufferSize; }
		
	protected:
		virtual void Initialize() = 0;

		ptrGPUMem m_Buffer;
		UINT m_BufferSize;	
		UINT m_Offset;
	};

	
	// Base class for those view who use descriptor heap to bind 
	class HeapDescriptor : public Descriptor {
	public:
		HeapDescriptor(ptrGPUMem gpubuffer, const UINT bufferSize) 
			: Descriptor(gpubuffer, bufferSize) {}

		inline UINT GetHeapIndex() const { return m_HeapIndex; }

		// Copy this descriptor to the in use descriptor heap for rendering
		inline void BindDescriptor(UINT index) const {
			DescriptorHeap* InitHeap = Engine::GetInUseHeap();
			CD3DX12_CPU_DESCRIPTOR_HANDLE destCPUHandle = InitHeap->GetCPUHandle(m_HeapIndex);

			DescriptorHeap* InUseHeap = Engine::GetInitHeap();
			CD3DX12_CPU_DESCRIPTOR_HANDLE srcCPUHandle = InUseHeap->GetCPUHandle(index);

			ID3D12Device* device = Engine::GetDevice();
			// Free threaded as long as different threads don't write to a same place
			device->CopyDescriptorsSimple(1, destCPUHandle, srcCPUHandle, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);//TODO type
		}

		inline void CopyTexture(D3D12_SUBRESOURCE_DATA* textureData) 
		{
			Engine::MemoryAllocator.UploadTexure(*m_Buffer, textureData);
		}

	protected:
		UINT m_HeapIndex;
	};



	class VertexBuffer : public Descriptor {
	public:
		VertexBuffer(ptrGPUMem gpubuffer, const UINT bufferSize, const UINT strideSize) 
			:Descriptor(gpubuffer, bufferSize), m_strideSize(strideSize) 
		{
			this->Initialize();
		}

		const D3D12_VERTEX_BUFFER_VIEW* GetBufferView() const { return &m_view; }

		inline UINT GetStrideSize() const { return m_strideSize; }

	private:
		void Initialize() override;

		const UINT m_strideSize;
		D3D12_VERTEX_BUFFER_VIEW m_view;
	};



	class IndexBuffer : public Descriptor  {
	public:
		IndexBuffer(ptrGPUMem gpubuffer, const UINT bufferSize)
			: Descriptor(gpubuffer, bufferSize) 
		{
			this->Initialize();
		}
		
		const D3D12_INDEX_BUFFER_VIEW* GetIndexView() const { return &m_view; }
		
	private:
		void Initialize() override;

		D3D12_INDEX_BUFFER_VIEW m_view;
	};



	class ConstantBuffer : public HeapDescriptor {
	public:
		ConstantBuffer(ptrGPUMem gpubuffer, const UINT bufferSize, DescriptorHeap*  descriptorHeap)
			: HeapDescriptor(gpubuffer, bufferSize) {}
		
	private:
		void Initialize() override;

		D3D12_CONSTANT_BUFFER_VIEW_DESC m_cbvDesc;
	};



	class ShaderResource : public HeapDescriptor {
	public:
		ShaderResource(ptrGPUMem gpubuffer, const D3D12_SHADER_RESOURCE_VIEW_DESC& desc) 
			: m_srvDesc(desc), HeapDescriptor(gpubuffer, 0) //TODO fix this emmmmmm
		{  this->Initialize(); }

	private:
		// Create SRV on the init descriptor heap
		void Initialize() override;

		D3D12_SHADER_RESOURCE_VIEW_DESC m_srvDesc;
	};



	class UnorderedAccess : public HeapDescriptor {
	public:
		UnorderedAccess(ptrGPUMem gpubuffer, const D3D12_UNORDERED_ACCESS_VIEW_DESC& desc)
			: m_uavDesc(desc), HeapDescriptor(gpubuffer, 0) 
		{ this->Initialize(); }

	private:
		// Create UAV on the init descriptor heap
		void Initialize() override;

		D3D12_UNORDERED_ACCESS_VIEW_DESC m_uavDesc;
	};

}
