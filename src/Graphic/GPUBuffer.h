#pragma once

#include <DXHelper.h>

namespace Graphic {
	// TODO Change the design (Add different resources)
	//		and use heap stuff?
	// A simple class abstract commited resource

	//template <class Data>
	class VertexGPUBuffer {
	public:
		VertexGPUBuffer(const UINT bufferSize, const UINT strideSize) 
			: m_bufferSize(bufferSize), m_strideSize(strideSize) {}

		void Initialize(ComPtr<ID3D12Device> device);

		void copyData(void* data, size_t size);

		const D3D12_VERTEX_BUFFER_VIEW* GetBufferView() const { return &m_view; }

	private:
		void CreateResource(ComPtr<ID3D12Device> device);

		const UINT m_strideSize;
		const UINT m_bufferSize;
		ComPtr<ID3D12Resource> m_buffer;
		D3D12_VERTEX_BUFFER_VIEW m_view;

	};

}