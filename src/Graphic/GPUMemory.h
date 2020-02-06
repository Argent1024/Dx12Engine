#pragma once

#include "DXHelper.h"
#define ptrGPUMem std::shared_ptr<GPU::GPUMemory>

namespace Graphic {
	class CommandManager;
	class CommandList;
	class ShaderResource;
	class UnorderedAccess;
	namespace GPU {

		class GPUMemory {

		public:
		friend CommandManager;
		friend CommandList;
		friend ShaderResource;
		friend UnorderedAccess;
		friend class MemoryAllocator;
		

			GPUMemory(UINT Size)
				: m_GPUAddr(D3D12_GPU_VIRTUAL_ADDRESS_NULL), m_MemSize(Size) {}

			// Call CreateCommitted and CreatePlaced when init
			virtual void Initialize(ComPtr<ID3D12Device> device) = 0;
			virtual void Destroy() = 0;
			
			virtual D3D12_HEAP_TYPE GetHeapType() = 0;

			inline UINT GetBufferSize() { return m_MemSize; }

			// Return offset of the memory, the user need to stored this
			UINT MemAlloc(const UINT size) {
				assert(m_MemAllocated + size <= m_MemSize);
				UINT offset = m_MemAllocated;
				m_MemAllocated += size;
				return offset;
			}

			inline D3D12_GPU_VIRTUAL_ADDRESS GetGPUAddr() const {
				assert(m_GPUAddr != D3D12_GPU_VIRTUAL_ADDRESS_NULL);
				return m_GPUAddr;
			}

			inline D3D12_RESOURCE_BARRIER Barrier(D3D12_RESOURCE_STATES before, D3D12_RESOURCE_STATES after) const
			{
				return CD3DX12_RESOURCE_BARRIER::Transition(m_Resource.Get(), before, after);
			}

		protected:
			inline ID3D12Resource* GetResource() const {
				return m_Resource.Get();
			}

			UINT m_MemAllocated;
			const UINT m_MemSize;
			D3D12_GPU_VIRTUAL_ADDRESS m_GPUAddr;
			ComPtr<ID3D12Resource> m_Resource;
		};


		class MemoryAllocator {
		public:
			MemoryAllocator() {}

			void Initialize(ComPtr<ID3D12Device> device);

			// TODO manage cpu memory by myself
			ptrGPUMem CreateCommittedBuffer(const UINT bufferSize, const D3D12_HEAP_TYPE m_HeapType=D3D12_HEAP_TYPE_DEFAULT);
			
			void UploadData(GPUMemory& dest, void* data, UINT size, UINT offset=0);

		private:
			void _UploadData(GPUMemory& buffer, void* data, UINT size, UINT offset=0);

			void _CopyBuffer(GPUMemory& dest, GPUMemory& src);

			ptrGPUMem m_Upload;
			ComPtr<ID3D12Device> m_device;
		};
	}

	extern GPU::MemoryAllocator EngineGPUMemory;
}