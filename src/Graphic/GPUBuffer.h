#pragma once

#include <DXHelper.h>

namespace Graphic {
	// TODO Change the design (Add different resources)
	//		free resource stuff, more about heap prop?

	// Class manage a block of memory, use the offset as the pointer
	class GPUMemory {
	public:
		GPUMemory() : m_GPUAddr(D3D12_GPU_VIRTUAL_ADDRESS_NULL), m_size(0), m_memAllocated(0) {}

		void Initialize(ComPtr<ID3D12Device> device, const UINT bufferSize, D3D12_HEAP_TYPE heapType);
		void Destroy();
		
		// Return offset of the memory, the user need to stored this
		UINT MemAlloc(const UINT size);

		// User should choose which method to use according the heap type
		// This method only for upload heap
		void UploadData(void* data, size_t size, size_t offset=0);
		// TODO different data methods

		// TODO barrier stuff
		D3D12_GPU_VIRTUAL_ADDRESS GetGPUAddr() const { return m_GPUAddr; }

	private:
		D3D12_HEAP_TYPE m_heapType;
		UINT m_size;
		UINT m_memAllocated;

		ComPtr<ID3D12Resource> m_resource;
		D3D12_GPU_VIRTUAL_ADDRESS m_GPUAddr;
	};

	// TODO add template? More option in creating this stuff
	class VertexBuffer {
	public:
		VertexBuffer(GPUMemory* gpuMem, const UINT bufferSize, const UINT strideSize) 
			: m_gpuMem(gpuMem), m_bufferSize(bufferSize), m_strideSize(strideSize) {}

		void Initialize();

		void copyData(void* data);

		const D3D12_VERTEX_BUFFER_VIEW* GetBufferView() const { return &m_view; }

	private:
		const UINT m_strideSize;
		const UINT m_bufferSize;

		GPUMemory* const m_gpuMem;
		UINT m_memOffset;

		D3D12_VERTEX_BUFFER_VIEW m_view;
	};


	class IndexBuffer {
	public:
		IndexBuffer(GPUMemory* gpuMem, const UINT bufferSize)
			: m_gpuMem(gpuMem), m_bufferSize(bufferSize) {}

		void Initialize();

		void copyData(void* data);

		const D3D12_INDEX_BUFFER_VIEW* GetIndexView() const { return &m_view; }
		
	private:
		const UINT m_bufferSize;

		GPUMemory* const m_gpuMem;
		UINT m_memOffset;

		D3D12_INDEX_BUFFER_VIEW m_view;
	};

}