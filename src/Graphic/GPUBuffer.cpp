#include "GPUBuffer.h"
namespace Graphic {
	namespace GPU {
	
		void UploadBuffer::Initialize(ComPtr<ID3D12Device> device, D3D12_RESOURCE_DESC* desc) {
			Destroy();
			if (!desc) {
				desc = &CD3DX12_RESOURCE_DESC::Buffer(m_MemSize);
			}
			ThrowIfFailed(
				device->CreateCommittedResource(
					&CD3DX12_HEAP_PROPERTIES(m_HeapType),
					D3D12_HEAP_FLAG_NONE,
					desc,
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


		// Default Type
		void DefaultBuffer::Initialize(ComPtr<ID3D12Device> device, D3D12_RESOURCE_DESC* desc) {
			Destroy();
			if (!desc) {
				desc = &CD3DX12_RESOURCE_DESC::Buffer(m_MemSize);
			}
			m_device = device;
			ThrowIfFailed(
				device->CreateCommittedResource(
					&CD3DX12_HEAP_PROPERTIES(m_HeapType),
					D3D12_HEAP_FLAG_NONE,
					desc,
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
			m_GPUAddr = D3D12_GPU_VIRTUAL_ADDRESS_NULL;
			m_MemAllocated = 0;
		}


		void PlacedBuffer::Initialize(ComPtr<ID3D12Device> device, D3D12_RESOURCE_DESC* desc) {
			Destroy();
			if (!desc) {
				desc = &CD3DX12_RESOURCE_DESC::Buffer(m_MemSize);//TODO??
			}
			m_HeapOffset = m_Heap->MemAlloc(m_MemSize);
			m_device = device;
			ThrowIfFailed(
				m_device->CreatePlacedResource(
					m_Heap->GetHeap(),
					m_HeapOffset,
					desc,
					D3D12_RESOURCE_STATE_COPY_DEST,
					nullptr,
					IID_PPV_ARGS(&m_Resource)
				)
			);
			NAME_D3D12_OBJECT(m_Resource);
		}


		void PlacedBuffer::Destroy() {
			m_device = nullptr;
			m_Resource = nullptr;
			m_GPUAddr = D3D12_GPU_VIRTUAL_ADDRESS_NULL;
			m_MemAllocated = 0;
			UINT m_HeapOffset = 0;
		}

	}
}