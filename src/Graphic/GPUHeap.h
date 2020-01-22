#pragma once

#include "DXHelper.h"


namespace Graphic {

	class GPUHeap {
	public:
		inline void SetAlignment(UINT64 Align) { m_HeapDesc.Alignment = Align; }

		inline void SetHeapSize(UINT64 Size) { 
			assert(m_HeapDesc.Alignment != 0);
			assert(Size % m_HeapDesc.Alignment == 0);
			m_HeapDesc.SizeInBytes = Size; 
		}

		inline void SetHeapProp(const D3D12_HEAP_PROPERTIES& HeapProp) {
			m_HeapDesc.Properties = HeapProp;
		}

		inline ID3D12Heap* GetHeap() const { return m_Heap.Get(); }

		void Destory();
		void Initialize(ComPtr<ID3D12Device> device);


	private:
		const UINT64 m_Align;
		D3D12_HEAP_DESC m_HeapDesc;
		ComPtr<ID3D12Heap> m_Heap;
	};
}