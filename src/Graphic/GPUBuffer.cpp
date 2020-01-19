#include "GPUBuffer.h"

namespace Graphic {
	void GPUMemory::Initialize(ComPtr<ID3D12Device> device, const UINT bufferSize, D3D12_HEAP_TYPE heapType) {
		Destroy();
		m_size = bufferSize;
		m_heapType = heapType;

		ThrowIfFailed(
			device->CreateCommittedResource(
				&CD3DX12_HEAP_PROPERTIES(m_heapType),
				D3D12_HEAP_FLAG_NONE,
				&CD3DX12_RESOURCE_DESC::Buffer(m_size),
				D3D12_RESOURCE_STATE_GENERIC_READ,    //TODO
				nullptr,
				IID_PPV_ARGS(&m_resource))
		);

		m_GPUAddr = m_resource->GetGPUVirtualAddress();
	}

	void GPUMemory::Destroy() {  // TODO Check usage?? barrier??
		m_resource = nullptr;
		m_GPUAddr = D3D12_GPU_VIRTUAL_ADDRESS_NULL;
		m_memAllocated = 0;
		m_size = 0;
	}

	UINT GPUMemory::MemAlloc(const UINT size) {
		assert(m_memAllocated + size <= m_size);
		UINT offset = m_memAllocated;
		m_memAllocated += size;
		return offset;	
	}

	void GPUMemory::UploadData(void* data, size_t size, size_t offset) {
		assert(offset + size <= m_size);
		assert(m_heapType == D3D12_HEAP_TYPE_UPLOAD);

		UINT8* memDataBegin;
		CD3DX12_RANGE readRange(0, 0);        // We do not intend to read from this resource on the CPU.
		ThrowIfFailed(m_resource->Map(0, &readRange, reinterpret_cast<void**>(&memDataBegin)));
		memcpy(memDataBegin + offset, data, size);
		m_resource->Unmap(0, nullptr);
	
	}

	//template<class Data>
	void VertexBuffer::Initialize() {
		m_memOffset = m_gpuMem->MemAlloc(m_bufferSize);
		m_view.BufferLocation = m_gpuMem->GetGPUAddr() + m_memOffset;

		m_view.StrideInBytes = m_strideSize;
		m_view.SizeInBytes = m_bufferSize;
	}

	void VertexBuffer::copyData(void* data) {
		m_gpuMem->UploadData(data, m_bufferSize, m_memOffset);
	}

	void IndexBuffer::Initialize() {
		m_memOffset = m_gpuMem->MemAlloc(m_bufferSize);
		m_view.BufferLocation = m_gpuMem->GetGPUAddr() + m_memOffset;
		m_view.Format = DXGI_FORMAT_R32_UINT;
		m_view.SizeInBytes = m_bufferSize;
	}

	void IndexBuffer::copyData(void* data) {
		m_gpuMem->UploadData(data, m_bufferSize, m_memOffset);
	}

}