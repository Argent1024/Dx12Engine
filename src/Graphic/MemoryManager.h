#pragma once
#include "GPUBuffer.h"
#include "CommandQueue.h"

namespace Graphic {
	extern GPU::MemoryAllocator EngineGPUMemoryAllocator;
	namespace GPU {
		class MemoryAllocator {
		public:
			void Initialize(ComPtr<ID3D12Device> device);

			// TODO manage cpu memory by myself
			ptrGPUMem CreateCommittedBuffer(const UINT bufferSize, const D3D12_HEAP_TYPE m_HeapType);
			
			void UploadData(GPUMemory& dest, void* data, UINT size, UINT offset=0);

		private:
			void _UploadData(GPUMemory& buffer, void* data, UINT size, UINT offset=0);

			void _CopyBuffer(GPUMemory& dest, GPUMemory& src);

			ptrGPUMem m_Upload;
			CommandManager m_CopyHelper;
			ComPtr<ID3D12Device> m_device;
		};
	}
}

