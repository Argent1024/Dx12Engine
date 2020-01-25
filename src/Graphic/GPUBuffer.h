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
			void copyData(void* data, size_t size = 0, size_t offset = 0) override;

		private:
			static const D3D12_HEAP_TYPE m_HeapType = D3D12_HEAP_TYPE_UPLOAD;
		};


		class DefaultBuffer : public CommittedBuffer {
		public:
			DefaultBuffer(const UINT bufferSize, ID3D12GraphicsCommandList* copyCommandList) :
				CommittedBuffer(bufferSize), m_commandList(copyCommandList) {}

			void Initialize(ComPtr<ID3D12Device> device) override;
			void Destroy() override;
			void copyData(void* data, size_t size, size_t offset)  override;

		private:
			// TODO: Better way to do the copy...
			ComPtr<ID3D12Resource> m_upload;
			ComPtr<ID3D12Device> m_device;
			ID3D12GraphicsCommandList* m_commandList;

			static const D3D12_HEAP_TYPE m_HeapType = D3D12_HEAP_TYPE_DEFAULT;
		};

	
		// TODO Test this buffer
		class PlacedBuffer : public GPUMemory {
		public:
			PlacedBuffer(GPUHeap* const GPUHeap, const UINT size, ID3D12GraphicsCommandList* copyCommandList)
				:GPUMemory(size), m_Heap(GPUHeap), m_commandList(copyCommandList) {}

			void Initialize(ComPtr<ID3D12Device> device) override;
			void Destroy() override;
			void copyData(void* data, size_t size, size_t offset) override;
			inline UINT GetHeapOffset() const { return m_HeapOffset; }

		private:
			// TODO: Better way to do the copy...
			ComPtr<ID3D12Resource> m_upload;
			ComPtr<ID3D12Device> m_device;
			ID3D12GraphicsCommandList* m_commandList;

			GPUHeap* const m_Heap;
			UINT m_HeapOffset;
			//TODO 
			D3D12_RESOURCE_DESC m_ResourceDesc;
		};

	}
}