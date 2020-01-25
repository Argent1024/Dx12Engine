#pragma once

#include "DXHelper.h"

namespace Graphic {

	class GPUHeap {
	public:
		GPUHeap(UINT Size, UINT Align=D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT) 
			: m_Size(Size), m_Align(Align), m_memAllocated(0), m_NumOfBuffers(0)
		{ 
			assert(m_Size % m_Align == 0);
			m_HeapDesc.Alignment = m_Align;
			m_HeapDesc.SizeInBytes = Size;
		}

		inline void SetHeapProp(const D3D12_HEAP_PROPERTIES& HeapProp) {
			m_HeapDesc.Properties = HeapProp;
		}

		UINT MemAlloc(const UINT size);

		inline ID3D12Heap* GetHeap() const { return m_Heap.Get(); }

		void Destroy();

	private:
		UINT m_NumOfBuffers;
		UINT m_memAllocated;

		const UINT64 m_Size;
		const UINT64 m_Align;
		D3D12_HEAP_DESC m_HeapDesc;
		ComPtr<ID3D12Heap> m_Heap;
	};
}