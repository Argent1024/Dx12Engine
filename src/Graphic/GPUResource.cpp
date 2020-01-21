#include "GPUResource.h"

namespace Graphic {

	UINT GPUMemory::MemAlloc(const UINT size) {
		assert(m_memAllocated + size <= m_size);
		UINT offset = m_memAllocated;
		m_memAllocated += size;
		return offset;	
	}

	void GPUUploadMemory::Destroy() {  // TODO Check usage?? barrier??
		m_resource = nullptr;
		m_GPUAddr = D3D12_GPU_VIRTUAL_ADDRESS_NULL;
		m_memAllocated = 0;
		m_size = 0;
	}

	void GPUUploadMemory::Initialize(ComPtr<ID3D12Device> device, const UINT bufferSize) {
		Destroy();
		m_size = bufferSize;

		ThrowIfFailed(
			device->CreateCommittedResource(
				&CD3DX12_HEAP_PROPERTIES(m_HeapType),
				D3D12_HEAP_FLAG_NONE,
				&CD3DX12_RESOURCE_DESC::Buffer(m_size),
				D3D12_RESOURCE_STATE_GENERIC_READ,    //TODO
				nullptr,
				IID_PPV_ARGS(&m_resource))
		);

		m_GPUAddr = m_resource->GetGPUVirtualAddress();
	}


	void GPUUploadMemory::copyData(void* data, size_t size, size_t offset) {
		assert(offset + size <= m_size);
		UINT8* memDataBegin;
		CD3DX12_RANGE readRange(0, 0);        // We do not intend to read from this resource on the CPU.
		ThrowIfFailed(m_resource->Map(0, &readRange, reinterpret_cast<void**>(&memDataBegin)));
		memcpy(memDataBegin + offset, data, size);
		m_resource->Unmap(0, nullptr);
	
	}

}