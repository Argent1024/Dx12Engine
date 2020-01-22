#pragma once
#include "GPUBuffer.h"
#include "GPUHeap.h"

namespace Graphic {
	class Descriptor {
	public:
		Descriptor(GPUMemory* GPUMem, const UINT Size);
		void Initialize(void* data) { Initialize(); copyData(data); }
		virtual void Initialize() = 0;
		virtual void copyData(void* data) = 0;

	protected:
		const UINT m_bufferSize;
		GPUMemory* const m_gpuMem;
		UINT m_memOffset;
	};

	// TODO add template? More option in creating this stuff
	class VertexBuffer : public Descriptor {
	public:
		VertexBuffer(GPUMemory* gpuMem, const UINT bufferSize, const UINT strideSize) 
			:Descriptor(gpuMem, bufferSize), m_strideSize(strideSize) {}

		void Initialize();
		void copyData(void* data);
		const D3D12_VERTEX_BUFFER_VIEW* GetBufferView() const { return &m_view; }

	private:
		const UINT m_strideSize;
		D3D12_VERTEX_BUFFER_VIEW m_view;
	};


	class IndexBuffer : public Descriptor {
	public:
		IndexBuffer(GPUMemory* gpuMem, const UINT bufferSize)
			: Descriptor(gpuMem, bufferSize) {}

		void Initialize();
		void copyData(void* data);
		const D3D12_INDEX_BUFFER_VIEW* GetIndexView() const { return &m_view; }
		
	private:
		D3D12_INDEX_BUFFER_VIEW m_view;
	};


}