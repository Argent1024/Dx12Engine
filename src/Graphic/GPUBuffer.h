#pragma once
#include "GPUMemory.h"
#include "GPUHeap.h"

namespace Graphic {
	namespace GPU {
		class CommittedBuffer : public GPUMemory {
		public:
			CommittedBuffer(const UINT size, D3D12_HEAP_TYPE type, D3D12_RESOURCE_STATES state) 
				: GPUMemory(size), 
				  m_HeapType(type), 
				  m_useClear(false), 
				  m_ClearValue(),
				  m_initState(state) {}
			
			CommittedBuffer(const D3D12_RESOURCE_DESC& desc, D3D12_HEAP_TYPE type,
						   D3D12_RESOURCE_STATES state)
				: GPUMemory(desc), 
				  m_HeapType(type), 
				  m_useClear(false),
				  m_ClearValue(),
				  m_initState(state) {}

			CommittedBuffer(const UINT size, D3D12_HEAP_TYPE type, 
				            D3D12_RESOURCE_STATES state, const D3D12_CLEAR_VALUE& clear) 
				: GPUMemory(size), 
				  m_HeapType(type), 
				  m_useClear(true), 
				  m_ClearValue(clear), 
				  m_initState(state) {}
			
			CommittedBuffer(const D3D12_RESOURCE_DESC& desc, D3D12_HEAP_TYPE type, 
							D3D12_RESOURCE_STATES state, const D3D12_CLEAR_VALUE& clear)
				: GPUMemory(desc), 
				  m_HeapType(type), 
				  m_useClear(true), 
				  m_ClearValue(clear), 
				  m_initState(state) {}

			void Initialize() override;

			void Destroy() override;

			inline D3D12_HEAP_TYPE GetHeapType() override { return m_HeapType;}

		private:
			// Committed buffer need a heap type to distinguish default or upload
			D3D12_HEAP_TYPE m_HeapType;
			
			// Use a clear value or not
			bool m_useClear;
			const D3D12_CLEAR_VALUE m_ClearValue;

			D3D12_RESOURCE_STATES m_initState;

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