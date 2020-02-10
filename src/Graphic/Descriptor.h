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

		virtual void Initialize() = 0;

		inline void copyData(void* data) { Engine::MemoryAllocator.UploadData(*m_Buffer, data, m_BufferSize, m_Offset); }

		inline UINT GetSize() { return m_BufferSize; }
		
	protected:
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

	protected:
		UINT m_HeapIndex;
	};



	class VertexBuffer : public Descriptor {
	public:
		VertexBuffer(ptrGPUMem gpubuffer, const UINT bufferSize, const UINT strideSize) 
			:Descriptor(gpubuffer, bufferSize), m_strideSize(strideSize) {}

		void Initialize() override;

		const D3D12_VERTEX_BUFFER_VIEW* GetBufferView() const { return &m_view; }

		inline UINT GetStrideSize() const { return m_strideSize; }

	private:
		const UINT m_strideSize;
		D3D12_VERTEX_BUFFER_VIEW m_view;
	};



	class IndexBuffer : public Descriptor  {
	public:
		IndexBuffer(ptrGPUMem gpubuffer, const UINT bufferSize)
			: Descriptor(gpubuffer, bufferSize) {}

		void Initialize() override;
		
		const D3D12_INDEX_BUFFER_VIEW* GetIndexView() const { return &m_view; }
		
	private:
		D3D12_INDEX_BUFFER_VIEW m_view;
	};



	class ConstantBuffer : public HeapDescriptor {
	public:
		ConstantBuffer(ptrGPUMem gpubuffer, const UINT bufferSize, DescriptorHeap*  descriptorHeap)
			: HeapDescriptor(gpubuffer, bufferSize) {}
		
		void Initialize() override;
	private:
		D3D12_CONSTANT_BUFFER_VIEW_DESC m_cbvDesc;
	};



	class ShaderResource : public HeapDescriptor {
	public:
		ShaderResource(ptrGPUMem gpubuffer, const UINT bufferSize=0) //TODO fix this emmmmmm
			: HeapDescriptor(gpubuffer, bufferSize) 
		{}

		inline void SetSRVDesc(const D3D12_SHADER_RESOURCE_VIEW_DESC& desc) { m_srvDesc = desc; }

		// Create SRV on the init descriptor heap
		void Initialize() override;

		void CopyTexture(D3D12_SUBRESOURCE_DATA* textureData) 
		{
			Engine::MemoryAllocator.UploadTexure(*m_Buffer, textureData);
		}

	private:
		D3D12_SHADER_RESOURCE_VIEW_DESC m_srvDesc;
	};



	class UnorderedAccess : public HeapDescriptor {
	public:
		UnorderedAccess(ptrGPUMem gpubuffer, const UINT bufferSize)
			: HeapDescriptor(gpubuffer, bufferSize) 
		{}

		inline void SetUAVDesc(const D3D12_UNORDERED_ACCESS_VIEW_DESC& desc) { m_uavDesc = desc; }

		// Create UAV on the init descriptor heap
		void Initialize() override;

	private:
		D3D12_UNORDERED_ACCESS_VIEW_DESC m_uavDesc;
	};

}
