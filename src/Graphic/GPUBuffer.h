#pragma once
#include "GPUMemory.h"
#include "GPUHeap.h"

namespace Graphic {
	class GPUCommittedBuffer : public GPUMemory {
	public:
		GPUCommittedBuffer(const UINT size) : GPUMemory(size) {}
	protected:
		ComPtr<ID3D12Resource> m_resource;
	};
	
	class GPUPlacedBuffer : public GPUMemory {
	public:
		GPUPlacedBuffer(const UINT size) : GPUMemory(size) {}
	protected:
		ComPtr<ID3D12Resource> m_resource;

	};

	class GPUPlacedUploadBuffer : public GPUPlacedBuffer {
	public:
		GPUPlacedUploadBuffer(GPUHeap* const heap, D3D12_RESOURCE_DESC ResourceDesc, const UINT bufferSize) 
			: GPUPlacedBuffer(bufferSize), m_Heap(heap), m_ResourceDesc(ResourceDesc) {}

		void Initialize(ComPtr<ID3D12Device> device) override;
		void Destroy() override;
		void copyData(void* data, size_t size, size_t offset) override;
	
	private:
		GPUHeap* const m_Heap;
		UINT64 m_HeapOffset;
		const D3D12_RESOURCE_DESC m_ResourceDesc;
		static const D3D12_HEAP_TYPE m_HeapType = D3D12_HEAP_TYPE_UPLOAD;

	};


	class GPUUploadBuffer : public GPUCommittedBuffer {
	public:
		GPUUploadBuffer(const UINT bufferSize) : GPUCommittedBuffer(bufferSize) {}

		void Initialize(ComPtr<ID3D12Device> device) override;
		void Destroy() override;
		void copyData(void* data, size_t size=0, size_t offset=0) override;

	private:
		static const D3D12_HEAP_TYPE m_HeapType = D3D12_HEAP_TYPE_UPLOAD;
	};


	class GPUDefaultBuffer : public GPUCommittedBuffer {
	public:
		GPUDefaultBuffer(const UINT bufferSize, ID3D12GraphicsCommandList* copyCommandList) :
			GPUCommittedBuffer(bufferSize), m_commandList(copyCommandList) {}

		void Initialize(ComPtr<ID3D12Device> device) override;
		void Destroy() override;
		void copyData(void* data, size_t size, size_t offset) override;

	private:
		// TODO: Better way to do the copy...
		ComPtr<ID3D12Resource> m_upload; 

		ComPtr<ID3D12Device> m_device;
		ID3D12GraphicsCommandList* m_commandList;
		static const D3D12_HEAP_TYPE m_HeapType = D3D12_HEAP_TYPE_DEFAULT;
	};

}