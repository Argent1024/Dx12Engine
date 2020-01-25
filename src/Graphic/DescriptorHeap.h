#pragma once

#include "DXHelper.h"

namespace Graphic {

	class DescriptorHeap {
	public:
		DescriptorHeap() {}
		~DescriptorHeap() { Destory(); }

		void Destory() {
			m_heap = nullptr;
		}

		inline void SetNumDescriptors(const UINT num) { m_HeapDesc.NumDescriptors = num; }
		inline void SetType(const D3D12_DESCRIPTOR_HEAP_TYPE type) { m_HeapDesc.Type = type; }
		inline void SetFlags(const D3D12_DESCRIPTOR_HEAP_FLAGS flag) { m_HeapDesc.Flags = flag; }

		void Initialize(ComPtr<ID3D12Device> device) {
			Destory();
			ThrowIfFailed(device->CreateDescriptorHeap(&m_HeapDesc, IID_PPV_ARGS(&m_heap)));
			m_DescriptorSize = device->GetDescriptorHandleIncrementSize(m_HeapDesc.Type);
			m_HandleStart = m_heap->GetCPUDescriptorHandleForHeapStart();
		}

		CD3DX12_CPU_DESCRIPTOR_HANDLE GetCPUHandle(UINT offset) const { 
			CD3DX12_CPU_DESCRIPTOR_HANDLE handle = m_HandleStart;
			return handle.Offset(offset, m_DescriptorSize); 
		}

	private:
		D3D12_DESCRIPTOR_HEAP_DESC m_HeapDesc;
		ComPtr<ID3D12DescriptorHeap> m_heap;
		UINT m_DescriptorSize;
		CD3DX12_CPU_DESCRIPTOR_HANDLE m_HandleStart;
	};

}