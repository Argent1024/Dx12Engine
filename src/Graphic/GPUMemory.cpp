#include "GPUMemory.h"
#include "GPUBuffer.h"
#include "CommandQueue.h"

namespace Graphic {

	namespace GPU {
		void MemoryAllocator::Initialize(ComPtr<ID3D12Device> device)
		{
			m_device = device;
		}

		ptrGPUMem MemoryAllocator::CreateCommittedBuffer(const UINT bufferSize, const D3D12_HEAP_TYPE heapType)
		{
			ptrGPUMem ptr;
			switch (heapType)
			{
			case D3D12_HEAP_TYPE_DEFAULT:
				ptr = std::make_shared<GPU::DefaultBuffer>(bufferSize);

			case D3D12_HEAP_TYPE_UPLOAD:
				ptr = std::make_shared<GPU::UploadBuffer>(bufferSize);

			//TODO Placed heap
			default:
				break;
				//assert(FALSE && "Wrong heap type");
			}

			ptr->Initialize(m_device);
			return ptr;
		}

		void MemoryAllocator::UploadData(GPUMemory& dest, void* data, UINT bufferSize, UINT offset) {
			switch (dest.GetHeapType()) 
			{
				case D3D12_HEAP_TYPE_DEFAULT:
					m_Upload = CreateCommittedBuffer(bufferSize, D3D12_HEAP_TYPE_UPLOAD);
					_UploadData(*m_Upload, data, bufferSize, offset);
					_CopyBuffer(dest, *m_Upload);

				case D3D12_HEAP_TYPE_UPLOAD:
					_UploadData(dest, data, bufferSize, offset);

				//TODO Placed heap
				default:
					break;
					//assert(FALSE && "Wrong heap type");
			}
		}

		void MemoryAllocator::UploadTexure(GPUMemory& dest, const D3D12_SUBRESOURCE_DATA* textureData, UINT bufferSize) {
			// TODO different types later
			assert(dest.GetHeapType == D3D12_HEAP_TYPE_DEFAULT);
			const UINT64 uploadBufferSize = GetRequiredIntermediateSize(dest.GetResource(), 0, 1);
			ComPtr<ID3D12Resource> textureUploadHeap;
			ThrowIfFailed(m_device->CreateCommittedResource(
							&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
							D3D12_HEAP_FLAG_NONE,
							&CD3DX12_RESOURCE_DESC::Buffer(uploadBufferSize),
							D3D12_RESOURCE_STATE_GENERIC_READ,
							nullptr,
							IID_PPV_ARGS(&textureUploadHeap)));
			CommandList copycl;
			CopyHelper.Start();
			CopyHelper.InitCommandList(&copycl);
			UpdateSubresources(copycl.GetCommandList(), dest.GetResource(), textureUploadHeap.Get(), 0, 0, 1, textureData);
			CopyHelper.ExecuteCommandList(&copycl);
			CopyHelper.End();
			CopyHelper.Start();
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
			CopyHelper.Start();
			CopyHelper.InitCommandList(&copycl);
			copycl.CopyBufferRegion(dest, destOffset, src, 0, size);
			CopyHelper.ExecuteCommandList(&copycl);
			CopyHelper.End();
			CopyHelper.Start();
		}

	}
}