#include "GPUBuffer.h"
namespace Graphic {
	namespace GPU {
		void CommittedBuffer::Initialize() {
			Destroy();
			ID3D12Device* device = Engine::GetDevice();
			D3D12_RESOURCE_STATES bufferState;

			switch (m_HeapType)
			{
			case D3D12_HEAP_TYPE_DEFAULT:
				bufferState = D3D12_RESOURCE_STATE_COPY_DEST;
				break;
			case D3D12_HEAP_TYPE_UPLOAD:
				bufferState = D3D12_RESOURCE_STATE_GENERIC_READ;
				break;
			case D3D12_HEAP_TYPE_READBACK:
				break;
			case D3D12_HEAP_TYPE_CUSTOM:
				break;
			default:
				break;
			}
			
			ThrowIfFailed(
				device->CreateCommittedResource(
							&CD3DX12_HEAP_PROPERTIES(m_HeapType),
							D3D12_HEAP_FLAG_NONE,
							&m_ResourceDesc,
							bufferState,    
							nullptr,
							IID_PPV_ARGS(&m_Resource))
			);
			// TODO Don't call this method if this memory is going to be used as texture
			m_GPUAddr = m_Resource->GetGPUVirtualAddress();
		}

		void CommittedBuffer::Destroy() {  // TODO Check usage?? barrier??
			m_Resource = nullptr;
			m_GPUAddr = D3D12_GPU_VIRTUAL_ADDRESS_NULL;
			m_MemAllocated = 0;
		}


		void PlacedBuffer::Initialize() {
			Destroy();
			ID3D12Device* device = Engine::GetDevice();
			m_HeapOffset = m_Heap->MemAlloc(m_MemSize);
			ThrowIfFailed(
				device->CreatePlacedResource(
					m_Heap->GetHeap(),
					m_HeapOffset,
					&m_ResourceDesc,
					D3D12_RESOURCE_STATE_COPY_DEST,
					nullptr,
					IID_PPV_ARGS(&m_Resource)
				)
			);
			NAME_D3D12_OBJECT(m_Resource);
		}


		void PlacedBuffer::Destroy() {
			m_Resource = nullptr;
			m_GPUAddr = D3D12_GPU_VIRTUAL_ADDRESS_NULL;
			m_MemAllocated = 0;
			UINT m_HeapOffset = 0;
		}

	}
}