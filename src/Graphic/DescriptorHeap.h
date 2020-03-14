#pragma once

#include "GraphicCore.h"

namespace Graphic {

	class DescriptorHeap {
	public:
		DescriptorHeap(const D3D12_DESCRIPTOR_HEAP_TYPE type, const UINT num, 
			           const D3D12_DESCRIPTOR_HEAP_FLAGS flag) 
			:m_NumDescriptors(num)
		{
			m_HeapDesc.NumDescriptors = num; 
			m_HeapDesc.Type = type;
			m_HeapDesc.Flags = flag;
		}

		~DescriptorHeap() { Destory(); }

		void Destory() {
			m_heap = nullptr;
			m_Alloced = 0;
		}

		inline void Reset() { m_Alloced = 0; }

		// TODO Multithreading lock here
		// Ask num descriptors and return the first offset
		UINT MallocHeap(UINT num=1) {
			assert(m_Alloced + num <= m_NumDescriptors);
			UINT index = m_Alloced;
			m_Alloced += num;
			return index;
		}

		void Initialize() {
			Destory();
			ID3D12Device* device = Engine::GetDevice();
			ThrowIfFailed(device->CreateDescriptorHeap(&m_HeapDesc, IID_PPV_ARGS(&m_heap)));
			m_DescriptorSize = device->GetDescriptorHandleIncrementSize(m_HeapDesc.Type);
			m_CPUHandleStart = m_heap->GetCPUDescriptorHandleForHeapStart();
			m_GPUHandleStart = m_heap->GetGPUDescriptorHandleForHeapStart();
		}

		ID3D12DescriptorHeap* GetDescriptorHeap() { return m_heap.Get(); }

		CD3DX12_CPU_DESCRIPTOR_HANDLE GetCPUHandle(UINT offset) const { 
			assert(offset <  m_NumDescriptors);
			CD3DX12_CPU_DESCRIPTOR_HANDLE handle = m_CPUHandleStart;
			return handle.Offset(offset, m_DescriptorSize); 
		}

		CD3DX12_GPU_DESCRIPTOR_HANDLE GetGPUHandle(UINT offset) const {
			assert(offset <  m_NumDescriptors);
			CD3DX12_GPU_DESCRIPTOR_HANDLE handle = m_GPUHandleStart;
			return handle.Offset(offset, m_DescriptorSize); 
		}

		inline D3D12_DESCRIPTOR_HEAP_TYPE GetDescriptorHeapType() const { return m_HeapDesc.Type; }
		inline D3D12_DESCRIPTOR_HEAP_FLAGS GetDescriptorHeapFlags() const { return m_HeapDesc.Flags; }
	private:
		D3D12_DESCRIPTOR_HEAP_DESC m_HeapDesc;
		ComPtr<ID3D12DescriptorHeap> m_heap;
		UINT m_NumDescriptors;
		UINT m_DescriptorSize;
		UINT m_Alloced;
		CD3DX12_CPU_DESCRIPTOR_HANDLE m_CPUHandleStart;
		CD3DX12_GPU_DESCRIPTOR_HANDLE m_GPUHandleStart;
	};

}


// The engine use two kind of descriptor heaps, large descriptor heaps to store all stuff
// and small descriptor heaps to create descriptor table and to use by shaders. 
// Call reset on small descriptor heap every frame
// When record commands, the command list should do copy between these two heaps.
namespace Engine {

	// Return the descriptor heap for creating views
	inline Graphic::DescriptorHeap* GetInitHeap() { return &InitHeap; }
	inline Graphic::DescriptorHeap* GetInUseHeap() { return &InUseHeap;}

};