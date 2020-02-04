#pragma once
#include "GPUMemory.h"
#include "GPUHeap.h"

namespace Graphic {
	namespace GPU {
		class CommittedBuffer : public GPUMemory {
		public:
			CommittedBuffer(const UINT size) : GPUMemory(size) {}
		};

		class UploadBuffer : public CommittedBuffer {
		public:
			UploadBuffer(const UINT bufferSize) : CommittedBuffer(bufferSize) {}

			void Initialize(ComPtr<ID3D12Device> device) override;
			void Destroy() override;
			
			inline D3D12_HEAP_TYPE GetHeapType() override { return m_HeapType; }

		private:
			static const D3D12_HEAP_TYPE m_HeapType = D3D12_HEAP_TYPE_UPLOAD;
		};


		class DefaultBuffer : public CommittedBuffer {
		public:
			DefaultBuffer(const UINT bufferSize) :
				CommittedBuffer(bufferSize) {}

			void Initialize(ComPtr<ID3D12Device> device) override;
			void Destroy() override;
			
			inline D3D12_HEAP_TYPE GetHeapType() override { return m_HeapType; }

		private:
			ComPtr<ID3D12Device> m_device;
			static const D3D12_HEAP_TYPE m_HeapType = D3D12_HEAP_TYPE_DEFAULT;
		};

		
		// TODO Test this buffer
		class PlacedBuffer : public GPUMemory {
		public:
			PlacedBuffer(GPUHeap* const GPUHeap, const UINT size)
				:GPUMemory(size), m_Heap(GPUHeap) {}

			void Initialize(ComPtr<ID3D12Device> device) override;
			void Destroy() override;
			
			inline UINT GetHeapOffset() const { return m_HeapOffset; }
			inline D3D12_HEAP_TYPE GetHeapType() override { return m_HeapType; }

		private:
			static const D3D12_HEAP_TYPE m_HeapType = D3D12_HEAP_TYPE_DEFAULT;

			ComPtr<ID3D12Device> m_device;
			
			GPUHeap* const m_Heap;
			UINT m_HeapOffset;
			//TODO 
			D3D12_RESOURCE_DESC m_ResourceDesc;
		};

	}
}