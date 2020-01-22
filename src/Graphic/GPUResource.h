#pragma once

#include "DXHelper.h"
#include "GPUMemory.h"

namespace Graphic {

	class GPUUploadMemory : public GPUMemory {
	public:
		void Initialize(ComPtr<ID3D12Device> device, const UINT bufferSize);
		void Destroy();
		void copyData(void* data, size_t size, size_t offset);

	private:
		static const D3D12_HEAP_TYPE m_HeapType = D3D12_HEAP_TYPE_UPLOAD;
	};


	class GPUDefaultMemory : public GPUMemory {
	public:
		GPUDefaultMemory(ID3D12GraphicsCommandList* copyCommandList) :
			GPUMemory(), m_commandList(copyCommandList) {}

		void Initialize(ComPtr<ID3D12Device> device, const UINT bufferSize);
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