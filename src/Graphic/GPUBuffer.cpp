#include "GPUBuffer.h"

namespace Graphic {
	//template<class Data>
	void VertexGPUBuffer::CreateResource(ComPtr<ID3D12Device> device) {
		ThrowIfFailed(
			device->CreateCommittedResource(
				&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
				D3D12_HEAP_FLAG_NONE,
				&CD3DX12_RESOURCE_DESC::Buffer(m_bufferSize),
				D3D12_RESOURCE_STATE_GENERIC_READ,
				nullptr,
				IID_PPV_ARGS(&m_buffer))
		);
	}

	//template<class Data>
	void VertexGPUBuffer::Initialize(ComPtr<ID3D12Device> device) {
		CreateResource(device);
		m_view.BufferLocation = m_buffer->GetGPUVirtualAddress();
		m_view.StrideInBytes = m_strideSize;
		m_view.SizeInBytes = m_bufferSize;
	}

	//template<class Data>
	void VertexGPUBuffer::copyData(void* data, size_t size) {
		UINT8* pVertexDataBegin;
		CD3DX12_RANGE readRange(0, 0);        // We do not intend to read from this resource on the CPU.
		ThrowIfFailed(m_buffer->Map(0, &readRange, reinterpret_cast<void**>(&pVertexDataBegin)));
		memcpy(pVertexDataBegin, data, size);
		m_buffer->Unmap(0, nullptr);
	}
}