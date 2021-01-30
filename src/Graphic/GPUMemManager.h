#pragma once
#include "GPUBuffer.h"
#include "CommandQueue.h"

using ptrGMem = std::shared_ptr<GPU::GPUMemory>;
using ptrGBuffer = std::shared_ptr<GPU::GPUBuffer>;
using ptrTBuffer = std::shared_ptr<GPU::Texturebuffer>;

namespace GPU 
{

	// TODO need to change all these somehow
	class MemoryManager 
	{
	public:
		// TODO need to check memory usage / page(...?)
		static ptrGBuffer CreateGBuffer() 
		{
			return ptrGBuffer();
		}

		static ptrTBuffer CreateTBuffer()
		{
			return ptrTBuffer();
		}

		static void UploadData(GPUBuffer& dest, const void* data, UINT dataSize, UINT offset)
		{
			// Always assume dest always in default heap
			D3D12_HEAP_PROPERTIES heapProp;
			dest.GetHeapProp(&heapProp, nullptr);
			assert(heapProp.Type == D3D12_HEAP_TYPE_DEFAULT);

			// Create a temp upload buffer
			GPUBuffer TempUploadBuffer;
			TempUploadBuffer.Initialize(dataSize, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_GENERIC_READ);
			
			ID3D12Resource* resource = TempUploadBuffer.GetResource();
			UINT8* memDataBegin;
			CD3DX12_RANGE readRange(0, 0);        // We do not intend to read from this resource on the CPU.
			ThrowIfFailed(resource->Map(0, &readRange, reinterpret_cast<void**>(&memDataBegin)));
			memcpy(memDataBegin + offset, data, dataSize);
			resource->Unmap(0, nullptr);

			// Copy it to dest
			_CopyBuffer(dest, TempUploadBuffer);
		}

		// TODO still need to change this later
		static void UploadTexure(Texturebuffer& dest, D3D12_SUBRESOURCE_DATA* textureData)
		{	
			// Always assume dest always in default heap
			D3D12_HEAP_PROPERTIES heapProp;
			dest.GetHeapProp(&heapProp, nullptr);
			assert(heapProp.Type == D3D12_HEAP_TYPE_DEFAULT);

			ID3D12Device* device = Engine::GetDevice();
			const UINT64 uploadBufferSize = GetRequiredIntermediateSize(dest.GetResource(), 0, 1);
			ComPtr<ID3D12Resource> textureUploadHeap;
			ThrowIfFailed(device->CreateCommittedResource(
							&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
							D3D12_HEAP_FLAG_NONE,
							&CD3DX12_RESOURCE_DESC::Buffer(uploadBufferSize),
							D3D12_RESOURCE_STATE_GENERIC_READ,
							nullptr,
							IID_PPV_ARGS(&textureUploadHeap)));
			
			Graphic::CommandList copycl;
			CopyHelper.Start();
			CopyHelper.InitCommandList(&copycl);
			UpdateSubresources<1>(copycl.GetCommandList(), dest.GetResource(), textureUploadHeap.Get(), 0, 0, 1, textureData);
			CopyHelper.ExecuteCommandList(&copycl);
			CopyHelper.End();
			CopyHelper.Start();
		}

	private:

		static void _CopyBuffer(GPUBuffer& dest, GPUBuffer& src)  
		{
			UINT size = src.GetBufferSize();
			UINT destSize = dest.GetBufferSize();
			UINT destOffset = dest.MemAlloc(0);

			Graphic::CommandList copycl;
			CopyHelper.Start();
			CopyHelper.InitCommandList(&copycl);
			copycl.CopyBufferRegion(dest, destOffset - size, src, 0, size);
			CopyHelper.ExecuteCommandList(&copycl);
			CopyHelper.End();
			CopyHelper.Start();
		}
	};



}
