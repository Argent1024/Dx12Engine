#pragma once
#include "GPUMemory.h"
#include "GPUHeap.h"

namespace GPU {
	// Memory for index buffer / vertex buffer / constant buffer
	class GPUBuffer : GPUMemory
	{
	public:
		GPUBuffer(UINT p_size) :
			m_MemSize(p_size), m_MemAllocated(0), m_GPUAddr(D3D12_GPU_VIRTUAL_ADDRESS_NULL)
		{
			assert(p_size > 0 && "GPU Buffer size < 0");
		}

		// Return offset of the memory, the user need to stored this
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

		// Create Committed Resource
		void Initialize(const D3D12_HEAP_TYPE heapType, const D3D12_RESOURCE_STATES initState)
		{
			D3D12_RESOURCE_DESC resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(m_MemSize);
			D3D12_HEAP_PROPERTIES heapProp = CD3DX12_HEAP_PROPERTIES(heapType);
			D3D12_HEAP_FLAGS flag = D3D12_HEAP_FLAG_NONE;
			DestoryAndCreateCommitted(&resourceDesc, &heapProp, initState, nullptr, flag);
		}

		void Destroy() override
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


	// Memory for Texture (srv, uav, rtv, dsv ...)
	class TextureBuffer : GPUMemory
	{
	public:
		TextureBuffer() { }

		// Create Committed Resource
		void Initialize(const D3D12_RESOURCE_DESC& resourceDesc,
			const D3D12_HEAP_TYPE heapType, 
			const D3D12_RESOURCE_STATES initState,
			const D3D12_CLEAR_VALUE* clearValue)
		{
			D3D12_HEAP_PROPERTIES heapProp = CD3DX12_HEAP_PROPERTIES(heapType);
			D3D12_HEAP_FLAGS flag = D3D12_HEAP_FLAG_NONE;
			DestoryAndCreateCommitted(&resourceDesc, &heapProp, initState, clearValue, flag);
		}
	};
}