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

			// TODO barrier stuff
			inline D3D12_GPU_VIRTUAL_ADDRESS GetGPUAddr() const {
				assert(m_GPUAddr != D3D12_GPU_VIRTUAL_ADDRESS_NULL);
				return m_GPUAddr;
			}

		protected:
			const UINT m_MemSize;
			D3D12_GPU_VIRTUAL_ADDRESS m_GPUAddr;
			ComPtr<ID3D12Resource> m_Resource;
		};

	}
}