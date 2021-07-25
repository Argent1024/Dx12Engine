#include "stdafx.h"
#include "Descriptor.h"
#include "GPUMemManager.h"

namespace Graphic {
	void BufferDescriptor::copyData(const void* data, UINT size)
	{ GPU::MemoryManager::UploadData(*m_Buffer, data, size, m_Offset); }

	VertexBuffer::VertexBuffer(ptrGBuffer gpubuffer, const UINT size, const UINT stride)
		: BufferDescriptor(gpubuffer)
	{
		m_Offset = m_Buffer->MemAlloc(size);
		m_view.BufferLocation = m_Buffer->GetGPUAddr() + m_Offset;
		m_view.StrideInBytes = stride;
		m_view.SizeInBytes = size;
	}

	IndexBuffer::IndexBuffer(ptrGBuffer gpubuffer, const UINT size)
		: BufferDescriptor(gpubuffer) // , m_start(0)
	{
		m_Offset = m_Buffer->MemAlloc(size);
		m_view.BufferLocation = m_Buffer->GetGPUAddr() + m_Offset;
		m_view.Format = DXGI_FORMAT_R32_UINT;
		m_view.SizeInBytes = size;
	}
}