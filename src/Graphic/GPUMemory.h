#pragma once

#include "GraphicCore.h"
#define ptrGPUMem std::shared_ptr<Graphic::GPU::GPUMemory>

namespace Graphic {
	class CommandManager;
	class CommandList;

	class ShaderResource;
	class UnorderedAccess;
	class RenderTarget;
	class DepthStencil;

	namespace GPU {

		class GPUMemory {

		public:
			friend CommandManager;
			friend CommandList;
			friend ShaderResource;
			friend UnorderedAccess;
			friend RenderTarget;
			friend DepthStencil;
			friend class MemoryAllocator;
		
			GPUMemory(const D3D12_RESOURCE_DESC& desc)
				:m_ResourceDesc(desc), m_GPUAddr(D3D12_GPU_VIRTUAL_ADDRESS_NULL), m_MemSize(0)
			{}
		
			GPUMemory(UINT Size)
				: m_GPUAddr(D3D12_GPU_VIRTUAL_ADDRESS_NULL), 
				  m_MemSize(Size), m_ResourceDesc(CD3DX12_RESOURCE_DESC::Buffer(Size)) {}

			// Call CreateCommitted and CreatePlaced when init
			virtual void Initialize() = 0;
			virtual void Destroy() = 0;
			
			virtual inline D3D12_HEAP_TYPE GetHeapType() = 0;

			inline UINT GetBufferSize() { return m_MemSize; }

			// Return offset of the memory, the user need to stored this
			UINT MemAlloc(const UINT size) {
				if (m_MemSize == 0) { return 0; } // Only store one stuff, don't care memory size TODO modify this...
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
			inline ID3D12Resource* GetResource() const { return m_Resource.Get(); }

			UINT m_MemAllocated;
			UINT m_MemSize;		// Total Memory Size
			D3D12_GPU_VIRTUAL_ADDRESS m_GPUAddr;


			D3D12_RESOURCE_DESC m_ResourceDesc;
			ComPtr<ID3D12Resource> m_Resource;
		};


		class MemoryAllocator {
		public:
			MemoryAllocator() {}

			// TODO manage cpu memory by myself

			// Simple Create CommittedBuffer, Consider nothing
			ptrGPUMem CreateCommittedBuffer(const UINT bufferSize, 
				const D3D12_HEAP_TYPE m_HeapType=D3D12_HEAP_TYPE_DEFAULT);

			inline ptrGPUMem CreateCommittedBuffer(const D3D12_RESOURCE_DESC& desc,
				const D3D12_HEAP_TYPE heapType = D3D12_HEAP_TYPE_DEFAULT) 
			{
				D3D12_RESOURCE_STATES initState = DefaultInitState(heapType);
				return CreateCommittedBuffer(desc, heapType, initState);
			}

			// Complex Create CommittedBuffer below
			// Use for creating textures
			ptrGPUMem CreateCommittedBuffer(const D3D12_RESOURCE_DESC& desc, 
				const D3D12_HEAP_TYPE heapType,
				D3D12_RESOURCE_STATES state);

			ptrGPUMem CreateCommittedBuffer(const D3D12_RESOURCE_DESC& desc,
				const D3D12_CLEAR_VALUE& clearValue,
				const D3D12_HEAP_TYPE heapType,
				D3D12_RESOURCE_STATES state);
			
			void UploadData(GPUMemory& dest, const void* data, UINT bufferize, UINT offset=0);

			void UploadTexure(GPUMemory& dest, D3D12_SUBRESOURCE_DATA* textureData);

		private:
			inline static D3D12_RESOURCE_STATES DefaultInitState(D3D12_HEAP_TYPE heapType) 
			{
				D3D12_RESOURCE_STATES bufferState = D3D12_RESOURCE_STATE_GENERIC_READ;
				// TODO
				switch (heapType)
				{
				case D3D12_HEAP_TYPE_DEFAULT:
					bufferState = D3D12_RESOURCE_STATE_COPY_DEST;
					break;
				case D3D12_HEAP_TYPE_UPLOAD:
					bufferState = D3D12_RESOURCE_STATE_GENERIC_READ;
					break;
				case D3D12_HEAP_TYPE_READBACK:
					break;
				case D3D12_HEAP_TYPE_CUSTOM:
					break;
				default:
					break;
				}
				return bufferState;
			}


			void _UploadData(GPUMemory& buffer, const void* data, UINT size, UINT offset=0);

			void _CopyBuffer(GPUMemory& dest, GPUMemory& src);

			// Only use one thread...?
			ptrGPUMem m_Upload;
		};
	}
}

namespace Engine 
{
	// TODO rewrite this without using class?
	extern Graphic::GPU::MemoryAllocator MemoryAllocator;
	
}