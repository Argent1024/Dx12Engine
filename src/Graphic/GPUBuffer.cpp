#include "GPUBuffer.h"

namespace Graphic {
	
	//template<class Data>
	void VertexBuffer::Initialize() {
		m_memOffset = m_gpuMem->MemAlloc(m_bufferSize);
		m_view.BufferLocation = m_gpuMem->GetGPUAddr() + m_memOffset;
		m_view.StrideInBytes = m_strideSize;
		m_view.SizeInBytes = m_bufferSize;
	}

	void VertexBuffer::copyData(void* data) {
		m_gpuMem->copyData(data, m_bufferSize, m_memOffset);
	}

	void IndexBuffer::Initialize() {
		m_memOffset = m_gpuMem->MemAlloc(m_bufferSize);
		m_view.BufferLocation = m_gpuMem->GetGPUAddr() + m_memOffset;
		m_view.Format = DXGI_FORMAT_R32_UINT;
		m_view.SizeInBytes = m_bufferSize;
	}

	void IndexBuffer::copyData(void* data) {
		m_gpuMem->copyData(data, m_bufferSize, m_memOffset);
	}

}