#include "Descriptor.h"

namespace Graphic {
	void VertexBuffer::Initialize() {
		m_Offset = m_Buffer->MemAlloc(m_BufferSize);
		m_view.BufferLocation = m_Buffer->GetGPUAddr() + m_Offset;
		m_view.StrideInBytes = m_strideSize;
		m_view.SizeInBytes = m_BufferSize;
	}

	void IndexBuffer::Initialize() {
		m_Offset = m_Buffer->MemAlloc(m_BufferSize);
		m_view.BufferLocation = m_Buffer->GetGPUAddr() + m_Offset;
		m_view.Format = DXGI_FORMAT_R32_UINT;
		m_view.SizeInBytes = m_BufferSize;
	}
}