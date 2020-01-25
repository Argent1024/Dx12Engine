#pragma once
#include "GPUMemory.h"
#include "GPUHeap.h"

namespace Graphic {
	namespace GPU {
		class CommittedBuffer : public GPUMemory {
		public:
			CommittedBuffer(const UINT size) : GPUMemory(size) {}

			virtual void copyData(void* data, size_t size, size_t offset) = 0;

			// Return offset of the memory, the user need to stored this
			UINT MemAlloc(const UINT size) {
				assert(m_MemAllocated + size <= m_MemSize);
				UINT offset = m_MemAllocated;
				m_MemAllocated += size;
				return offset;
			}

		protected:
			UINT m_MemAllocated;
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
	}
}