#pragma once
#include "GPUMemory.h"
#include "GPUHeap.h"

namespace Graphic {
	namespace GPU {
		class CommittedBuffer : public GPUMemory {
		public:
			CommittedBuffer(const UINT size, D3D12_HEAP_TYPE type) 
				: GPUMemory(size), m_HeapType(type) {}
			
			CommittedBuffer(const D3D12_RESOURCE_DESC& desc, D3D12_HEAP_TYPE type)
				: GPUMemory(desc), m_HeapType(type) {}

			void Initialize() override;

			void Destroy() override;

			inline D3D12_HEAP_TYPE GetHeapType() override { return m_HeapType;}

		private:
			// Committed buffer need a heap type to distinguish default or upload
			D3D12_HEAP_TYPE m_HeapType;
		};
		
		// TODO Test this buffer
		class PlacedBuffer : public GPUMemory {
		public:
			PlacedBuffer(GPUHeap* const GPUHeap, const UINT size)
				:GPUMemory(size), m_Heap(GPUHeap) {}

			void Initialize() override;
			void Destroy() override;
			
			inline UINT GetHeapOffset() const { return m_HeapOffset; }

		private:
			static const D3D12_HEAP_TYPE m_HeapType = D3D12_HEAP_TYPE_DEFAULT;
			
			GPUHeap* const m_Heap;
			UINT m_HeapOffset;
		
		};

	}
}