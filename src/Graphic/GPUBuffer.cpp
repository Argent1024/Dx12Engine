#include "GPUBuffer.h"
namespace Graphic {
	namespace GPU {
	
		void UploadBuffer::Initialize(ComPtr<ID3D12Device> device) {
			Destroy();
			ThrowIfFailed(
				device->CreateCommittedResource(
					&CD3DX12_HEAP_PROPERTIES(m_HeapType),
					D3D12_HEAP_FLAG_NONE,
					&CD3DX12_RESOURCE_DESC::Buffer(m_MemSize),
					D3D12_RESOURCE_STATE_GENERIC_READ,    //TODO
					nullptr,
					IID_PPV_ARGS(&m_Resource))
			);
			m_GPUAddr = m_Resource->GetGPUVirtualAddress();
		}

		void UploadBuffer::Destroy() {  // TODO Check usage?? barrier??
			m_Resource = nullptr;
			m_GPUAddr = D3D12_GPU_VIRTUAL_ADDRESS_NULL;
			m_MemAllocated = 0;
		}

		void UploadBuffer::copyData(void* data, size_t size, size_t offset) {
			assert(offset + size <= m_MemSize);
			UINT8* memDataBegin;
			CD3DX12_RANGE readRange(0, 0);        // We do not intend to read from this resource on the CPU.
			ThrowIfFailed(m_Resource->Map(0, &readRange, reinterpret_cast<void**>(&memDataBegin)));
			memcpy(memDataBegin + offset, data, size);
			m_Resource->Unmap(0, nullptr);
		}


		// Default Type
		void DefaultBuffer::Initialize(ComPtr<ID3D12Device> device) {
			Destroy();
			m_device = device;
			ThrowIfFailed(
				device->CreateCommittedResource(
					&CD3DX12_HEAP_PROPERTIES(m_HeapType),
					D3D12_HEAP_FLAG_NONE,
					&CD3DX12_RESOURCE_DESC::Buffer(m_MemSize),
					D3D12_RESOURCE_STATE_GENERIC_READ,    //TODO
					nullptr,
					IID_PPV_ARGS(&m_Resource))
			);
			NAME_D3D12_OBJECT(m_Resource);
			m_GPUAddr = m_Resource->GetGPUVirtualAddress();
		}

		void DefaultBuffer::Destroy() {
			m_device = nullptr;
			m_Resource = nullptr;
			m_upload = nullptr;
			m_GPUAddr = D3D12_GPU_VIRTUAL_ADDRESS_NULL;
			m_MemAllocated = 0;
		}

		void DefaultBuffer::copyData(void* data, size_t size, size_t offset) {
			assert(offset + size <= m_MemSize);
			ThrowIfFailed(
				m_device->CreateCommittedResource(
					&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
					D3D12_HEAP_FLAG_NONE,
					&CD3DX12_RESOURCE_DESC::Buffer(size),
					D3D12_RESOURCE_STATE_GENERIC_READ,
					nullptr,
					IID_PPV_ARGS(&m_upload))
			);
			NAME_D3D12_OBJECT(m_upload);
			// Upload to upload buffer
			UINT8* memDataBegin;
			CD3DX12_RANGE readRange(0, 0);        // We do not intend to read from this resource on the CPU.
			ThrowIfFailed(m_upload->Map(0, &readRange, reinterpret_cast<void**>(&memDataBegin)));
			memcpy(memDataBegin, data, size);
			m_upload->Unmap(0, nullptr);

			// Copy to resource buffer
			m_commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_Resource.Get(), D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_COPY_DEST));
			m_commandList->CopyBufferRegion(m_Resource.Get(), offset, m_upload.Get(), 0, size);
			m_commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_Resource.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_GENERIC_READ));
		}
	}
}