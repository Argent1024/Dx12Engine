#pragma once
#include "GPUMemory.h"
#include "GPUBuffer.h"
#include "GPUHeap.h"
#include "DescriptorHeap.h"

namespace Graphic {
	// Descriptors don't need to know about GPU Heap, GPU Buffer should handle all the memory stuff
	class Descriptor {
	public:
		Descriptor(GPU::GPUMemory* gpubuffer, const UINT bufferSize)
			: m_Buffer(gpubuffer), m_BufferSize(bufferSize) {}

		virtual void Initialize(ComPtr<ID3D12Device> device) = 0;
		inline void copyData(void* data) { m_Buffer->copyData(data, m_BufferSize, m_Offset); }
	
	protected:
		GPU::GPUMemory* m_Buffer;
		UINT m_BufferSize;
		UINT m_Offset;
	};

	// TODO add template? More option in creating this stuff
	class VertexBuffer : public Descriptor {
	public:
		VertexBuffer(GPU::CommittedBuffer * gpubuffer, const UINT bufferSize, const UINT strideSize) 
			:Descriptor(gpubuffer, bufferSize), m_strideSize(strideSize) {}

		void Initialize(ComPtr<ID3D12Device> device=nullptr) override;

		const D3D12_VERTEX_BUFFER_VIEW* GetBufferView() const { return &m_view; }

	private:
		const UINT m_strideSize;
		D3D12_VERTEX_BUFFER_VIEW m_view;
	};


	class IndexBuffer : public Descriptor  {
	public:
		IndexBuffer(GPU::CommittedBuffer* gpubuffer, const UINT bufferSize)
			: Descriptor(gpubuffer, bufferSize) {}

		void Initialize(ComPtr<ID3D12Device> device=nullptr) override;
	
		const D3D12_INDEX_BUFFER_VIEW* GetIndexView() const { return &m_view; }
		
	private:
		D3D12_INDEX_BUFFER_VIEW m_view;
	};


	// Texture
	class ShaderResource : public Descriptor {
	public:
		ShaderResource(GPU::GPUMemory* gpubuffer, const UINT bufferSize, DescriptorHeap*  descriptorHeap)
			: Descriptor(gpubuffer, bufferSize), m_descriptorHeap(descriptorHeap) {}

		void Initialize(ComPtr<ID3D12Device> device) override;

	private:
		DescriptorHeap*  m_descriptorHeap;
		UINT m_HeapIndex;
		D3D12_SHADER_RESOURCE_VIEW_DESC m_srvDesc;
	};


	class ConstantBuffer : public Descriptor {
	public:
		ConstantBuffer(GPU::GPUMemory* gpubuffer, const UINT bufferSize, DescriptorHeap*  descriptorHeap)
			: Descriptor(gpubuffer, bufferSize), m_descriptorHeap(descriptorHeap) {}
		
		void Initialize(ComPtr<ID3D12Device> device) override;
	private:
		DescriptorHeap*  m_descriptorHeap;
		UINT m_HeapIndex;
		D3D12_CONSTANT_BUFFER_VIEW_DESC m_cbvDesc;
	};
}
