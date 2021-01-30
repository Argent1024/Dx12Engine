#pragma once
#include "GPUMemory.h"
#include "GPUHeap.h"

namespace GPU {

	D3D12_RESOURCE_STATES DefaultInitState(D3D12_HEAP_TYPE heapType) 
	{
		D3D12_RESOURCE_STATES bufferState = D3D12_RESOURCE_STATE_GENERIC_READ;
		switch (heapType)
		{
		case D3D12_HEAP_TYPE_DEFAULT:
			bufferState = D3D12_RESOURCE_STATE_COPY_DEST;
			break;
		case D3D12_HEAP_TYPE_UPLOAD:
			bufferState = D3D12_RESOURCE_STATE_GENERIC_READ;
			break;
		case D3D12_HEAP_TYPE_READBACK:
			bufferState = D3D12_RESOURCE_STATE_COPY_DEST;
			break;
		case D3D12_HEAP_TYPE_CUSTOM:
			break;
		default:
			break;
		}
		return bufferState;
	}

		
	class GPUBuffer : public GPUMemory
	{
	public:
		GPUBuffer() : m_MemSize(0), m_MemAllocated(0), m_GPUAddr(D3D12_GPU_VIRTUAL_ADDRESS_NULL) { }
		
		~GPUBuffer() { Destory(); }

		// Subbuffer memory allocate
		// Return offset of the memory, the user need to stored this
		// Use zero to return the memory alreay allocated
		UINT MemAlloc(const UINT size)
		{
			assert(m_MemAllocated + size <= m_MemSize);
			UINT offset = m_MemAllocated;
			m_MemAllocated += size;
			return offset;
		}

		inline UINT GetBufferSize() const { return m_MemSize; }
		
		inline D3D12_GPU_VIRTUAL_ADDRESS GetGPUAddr() const
		{
			assert(m_GPUAddr != D3D12_GPU_VIRTUAL_ADDRESS_NULL);
			return m_GPUAddr;
		}

		// ************************** Creating a buffer **************************//
		inline void Initialize(UINT size) {
			Initialize(size, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_COPY_DEST, nullptr);
		}

		void Initialize(UINT size, const D3D12_HEAP_TYPE heapType, 
			const D3D12_RESOURCE_STATES initState, 
			const D3D12_CLEAR_VALUE* clear=nullptr,
			D3D12_RESOURCE_FLAGS resourceFlag=D3D12_RESOURCE_FLAG_NONE) 
		{
			D3D12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Buffer(size);
			m_MemSize = size;

			DestoryAndCreateCommitted(&desc, heapType, initState, clear);
			m_GPUAddr = m_Resource->GetGPUVirtualAddress();	// Will need GPU addr since this is a buffer
		}

		//***********************************************************************//

		void Destory() override
		{
			m_Resource = nullptr;
			m_GPUAddr = D3D12_GPU_VIRTUAL_ADDRESS_NULL;
			m_MemAllocated = 0;
			m_MemSize = 0;
		}

	private:
		UINT m_MemAllocated;
		UINT m_MemSize;		// Total Memory Size
		D3D12_GPU_VIRTUAL_ADDRESS m_GPUAddr;
	};


	// TODO Same name with Texture.h
	class Texturebuffer : public GPUMemory
	{
	public:
		Texturebuffer()  { }
		
		~Texturebuffer() { Destory(); }

		inline void Initialize(const D3D12_RESOURCE_DESC* resourceDesc, 
			const D3D12_CLEAR_VALUE* clearValue = nullptr) 
		{
			DestoryAndCreateCommitted(resourceDesc, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_COPY_DEST, clearValue);
		}

	};
}