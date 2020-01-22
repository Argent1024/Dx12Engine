#pragma once
#include "GPUMemory.h"

namespace Graphic {
	class GPUBuffer : public GPUMemory {
	public:
		GPUBuffer(const UINT size) : GPUMemory(size) {}

		UINT MemAlloc(const UINT size);

	protected:
		ComPtr<ID3D12Resource> m_resource;
	};

	
	class GPUUploadBuffer : public GPUBuffer {
	public:
		GPUUploadBuffer(const UINT bufferSize) : GPUBuffer(bufferSize) {}

		void Initialize(ComPtr<ID3D12Device> device);
		void Destroy();
		void copyData(void* data, size_t size, size_t offset);

	private:
		static const D3D12_HEAP_TYPE m_HeapType = D3D12_HEAP_TYPE_UPLOAD;
	};


	class GPUDefaultBuffer : public GPUBuffer {
	public:
		GPUDefaultBuffer(const UINT bufferSize, ID3D12GraphicsCommandList* copyCommandList) :
			GPUBuffer(bufferSize), m_commandList(copyCommandList) {}

		void Initialize(ComPtr<ID3D12Device> device);
		void Destroy();
		void copyData(void* data, size_t size, size_t offset);

	private:
		// TODO: Better way to do the copy...
		ComPtr<ID3D12Resource> m_upload; 

		ComPtr<ID3D12Device> m_device;
		ID3D12GraphicsCommandList* m_commandList;
		static const D3D12_HEAP_TYPE m_HeapType = D3D12_HEAP_TYPE_DEFAULT;
	};

}