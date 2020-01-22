#pragma once

#include "DXHelper.h"


namespace Graphic {
	class GPUMemory {
	public:
		GPUMemory() : m_GPUAddr(D3D12_GPU_VIRTUAL_ADDRESS_NULL), m_size(0), m_memAllocated(0) {}

		virtual void Initialize(ComPtr<ID3D12Device> device, const UINT bufferSize) = 0;
		virtual void copyData(void* data, size_t size, size_t offset) = 0;
		virtual void Destroy() = 0;
		
		// Return offset of the memory, the user need to stored this
		UINT MemAlloc(const UINT size);

		// TODO barrier stuff
		inline D3D12_GPU_VIRTUAL_ADDRESS GetGPUAddr() const { 
			assert(m_GPUAddr != D3D12_GPU_VIRTUAL_ADDRESS_NULL);
			return m_GPUAddr; 
		}

	protected:
		UINT m_size;
		UINT m_memAllocated;

		ComPtr<ID3D12Resource> m_resource;
		D3D12_GPU_VIRTUAL_ADDRESS m_GPUAddr;
	};




}