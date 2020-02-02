#pragma once

#include "DXHelper.h"


namespace Graphic {
	namespace GPU {
		class GPUMemory {
		public:
			GPUMemory(UINT Size)
				: m_GPUAddr(D3D12_GPU_VIRTUAL_ADDRESS_NULL), m_MemSize(Size) {}

			// Call CreateCommitted and CreatePlaced when init
			virtual void Initialize(ComPtr<ID3D12Device> device) = 0;
			virtual void Destroy() = 0;
			virtual void copyData(void* data, size_t size, size_t offset) = 0;

			// Return offset of the memory, the user need to stored this
			UINT MemAlloc(const UINT size) {
				assert(m_MemAllocated + size <= m_MemSize);
				UINT offset = m_MemAllocated;
				m_MemAllocated += size;
				return offset;
			}

			// TODO barrier stuff

			inline D3D12_GPU_VIRTUAL_ADDRESS GetGPUAddr() const {
				assert(m_GPUAddr != D3D12_GPU_VIRTUAL_ADDRESS_NULL);
				return m_GPUAddr;
			}

			inline void CreateSRV(ComPtr<ID3D12Device> device, D3D12_SHADER_RESOURCE_VIEW_DESC* srvDesc, D3D12_CPU_DESCRIPTOR_HANDLE srvHandle) {
				device->CreateShaderResourceView(m_Resource.Get(), srvDesc, srvHandle);
			}

			inline D3D12_RESOURCE_BARRIER Barrier(D3D12_RESOURCE_STATES before, D3D12_RESOURCE_STATES after) const
			{
				return CD3DX12_RESOURCE_BARRIER::Transition(m_Resource.Get(), before, after);
			}

		protected:
			UINT m_MemAllocated;
			const UINT m_MemSize;
			D3D12_GPU_VIRTUAL_ADDRESS m_GPUAddr;
			ComPtr<ID3D12Resource> m_Resource;


		};

	}
}