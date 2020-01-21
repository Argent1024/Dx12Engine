#pragma once

#include "DXHelper.h"

namespace Graphic {
// Class manage a block of memory, use the offset as the pointer
	class GPUMemory {
	public:
		GPUMemory() : m_GPUAddr(D3D12_GPU_VIRTUAL_ADDRESS_NULL), m_size(0), m_memAllocated(0) {}

		virtual void Initialize(ComPtr<ID3D12Device> device, const UINT bufferSize) = 0;

		virtual void Destroy() = 0;
		
		// Return offset of the memory, the user need to stored this
		UINT MemAlloc(const UINT size);

		// User should choose which method to use according the heap type
		// This method only for upload heap
		virtual void copyData(void* data, size_t size, size_t offset) = 0;

		// TODO barrier stuff
		inline D3D12_GPU_VIRTUAL_ADDRESS GetGPUAddr() const { return m_GPUAddr; }

	protected:
		UINT m_size;
		UINT m_memAllocated;

		ComPtr<ID3D12Resource> m_resource;
		D3D12_GPU_VIRTUAL_ADDRESS m_GPUAddr;
	};

	
	class GPUUploadMemory : public GPUMemory {
	public:
		void Initialize(ComPtr<ID3D12Device> device, const UINT bufferSize);
		void Destroy();
		void copyData(void* data, size_t size, size_t offset=0);

	private:
		static const D3D12_HEAP_TYPE m_HeapType = D3D12_HEAP_TYPE_UPLOAD;
	};

}