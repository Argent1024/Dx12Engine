#include "MemoryManager.h"
namespace Graphic {
	
	GPU::MemoryAllocator EngineGPUMemoryAllocator;

	namespace GPU {
		void MemoryAllocator::Initialize(ComPtr<ID3D12Device> device)
		{
			m_CopyHelper.Initialize(device);
		}

		ptrGPUMem MemoryAllocator::CreateCommittedBuffer(const UINT bufferSize, const D3D12_HEAP_TYPE heapType)
		{
			ptrGPUMem ptr;
			switch (heapType)
			{
			case D3D12_HEAP_TYPE_DEFAULT:
				ptr = std::make_shared<GPU::DefaultBuffer>(bufferSize);
				ptr->Initialize(m_device);

			case D3D12_HEAP_TYPE_UPLOAD:
				ptr = std::make_shared<GPU::UploadBuffer>(bufferSize);
				ptr->Initialize(m_device);

			//TODO Placed heap
			default:
				assert(FALSE && "Wrong heap type");
			}
			return ptr;
		}

		void MemoryAllocator::UploadData(GPUMemory& dest, void* data, UINT size, UINT offset) {
			switch (dest.GetHeapType()) 
			{
				case D3D12_HEAP_TYPE_DEFAULT:
					m_Upload = CreateCommittedBuffer(size, D3D12_HEAP_TYPE_UPLOAD);
					_UploadData(*m_Upload, data, size, offset);
					_CopyBuffer(dest, *m_Upload);

				case D3D12_HEAP_TYPE_UPLOAD:
					_UploadData(dest, data, size, offset);

				//TODO Placed heap
				default:
					assert(FALSE && "Wrong heap type");
			}
		}

		void MemoryAllocator::_UploadData(GPUMemory& buffer, void* data, UINT size, UINT offset) {
			assert(buffer.GetHeapType() == D3D12_HEAP_TYPE_UPLOAD);
			assert(offset + size <= buffer.m_MemSize);
			ID3D12Resource* resource = buffer.GetResource();
			UINT8* memDataBegin;
			CD3DX12_RANGE readRange(0, 0);        // We do not intend to read from this resource on the CPU.
			ThrowIfFailed(resource->Map(0, &readRange, reinterpret_cast<void**>(&memDataBegin)));
			memcpy(memDataBegin + offset, data, size);
			resource->Unmap(0, nullptr);
		}

		void MemoryAllocator::_CopyBuffer(GPUMemory& dest, GPUMemory& src)  {
			UINT size = src.GetBufferSize();
			UINT destSize = dest.GetBufferSize();
			UINT destOffset = dest.m_MemAllocated;
			assert(destOffset + size <= destSize);

			CommandList copycl;
			m_CopyHelper.Start();
			m_CopyHelper.InitCommandList(&copycl);
			copycl.CopyBufferRegion(dest, destOffset, src, 0, size);
			m_CopyHelper.ExecuteCommandList(&copycl);
			m_CopyHelper.End();
			m_CopyHelper.Start();
		}
	}
}