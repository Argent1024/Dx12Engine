#include "GPUBuffer.h"
namespace Graphic {
	namespace GPU {
		void CommittedBuffer::Initialize() {
			Destroy();
			ID3D12Device* device = Engine::GetDevice();
			
			const D3D12_CLEAR_VALUE* optimizedClearValue = nullptr;
			if (m_useClear) {
				optimizedClearValue = &m_ClearValue;
			}

			ThrowIfFailed(
				device->CreateCommittedResource(
							&CD3DX12_HEAP_PROPERTIES(m_HeapType),
							D3D12_HEAP_FLAG_NONE,
							&m_ResourceDesc,
							m_initState,    
							optimizedClearValue,
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