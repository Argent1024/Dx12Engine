#pragma once

#include <DXHelper.h>

namespace Graphic {
	// TODO add default heap type

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

		// TODO barrier stuff
		inline D3D12_GPU_VIRTUAL_ADDRESS GetGPUAddr() const { return m_GPUAddr; }

	private:
		D3D12_HEAP_TYPE m_heapType;
		UINT m_size;
		UINT m_memAllocated;

		ComPtr<ID3D12Resource> m_resource;
		D3D12_GPU_VIRTUAL_ADDRESS m_GPUAddr;
	};

	class GPUBuffer {
	public:
		GPUBuffer(GPUMemory* gpuMem, const UINT bufferSize) 
			: m_gpuMem(gpuMem), m_bufferSize(bufferSize) {}
		virtual void Initialize() = 0;
		virtual void copyData(void* data) = 0;

	protected:
		const UINT m_bufferSize;
		GPUMemory* const m_gpuMem;
		UINT m_memOffset;
	};

	// TODO add template? More option in creating this stuff
	class VertexBuffer : public GPUBuffer {
	public:
		VertexBuffer(GPUMemory* gpuMem, const UINT bufferSize, const UINT strideSize) 
			:GPUBuffer(gpuMem, bufferSize), m_strideSize(strideSize) {}

		void Initialize();
		void copyData(void* data);
		const D3D12_VERTEX_BUFFER_VIEW* GetBufferView() const { return &m_view; }

	private:
		const UINT m_strideSize;
		D3D12_VERTEX_BUFFER_VIEW m_view;
	};


	class IndexBuffer : public GPUBuffer {
	public:
		IndexBuffer(GPUMemory* gpuMem, const UINT bufferSize)
			: GPUBuffer(gpuMem, bufferSize) {}

		void Initialize();
		void copyData(void* data);
		const D3D12_INDEX_BUFFER_VIEW* GetIndexView() const { return &m_view; }
		
	private:
		D3D12_INDEX_BUFFER_VIEW m_view;
	};
}