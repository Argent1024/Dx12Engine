#pragma once
#include "GPUMemory.h"
#include "GPUBuffer.h"
#include "GPUHeap.h"
#include "DescriptorHeap.h"

namespace Graphic {
	// TODO add template? More option in creating this stuff
	class VertexBuffer {
	public:
		VertexBuffer(GPU::CommittedBuffer * gpubuffer, const UINT bufferSize, const UINT strideSize) 
			:m_Buffer(gpubuffer), m_BufferSize(bufferSize), m_strideSize(strideSize) {}

		void Initialize();
		inline void copyData(void* data) { m_Buffer->copyData(data, m_BufferSize, m_Offset); }
		const D3D12_VERTEX_BUFFER_VIEW* GetBufferView() const { return &m_view; }

	private:
		GPU::CommittedBuffer* m_Buffer;
		UINT m_BufferSize;
		UINT m_Offset;
		const UINT m_strideSize;
		D3D12_VERTEX_BUFFER_VIEW m_view;
	};


	class IndexBuffer  {
	public:
		IndexBuffer(GPU::CommittedBuffer* gpubuffer, const UINT bufferSize)
			: m_Buffer(gpubuffer), m_BufferSize(bufferSize) {}

		void Initialize();
		inline void copyData(void* data) { m_Buffer->copyData(data, m_BufferSize, m_Offset); }
		const D3D12_INDEX_BUFFER_VIEW* GetIndexView() const { return &m_view; }
		
	private:
		GPU::CommittedBuffer * m_Buffer;
		UINT m_BufferSize;
		UINT m_Offset;
		D3D12_INDEX_BUFFER_VIEW m_view;
	};


}
