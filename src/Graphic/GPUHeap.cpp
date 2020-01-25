#include "GPUHeap.h"

namespace Graphic {	
	void GPUHeap::Destroy() {
		m_Heap = nullptr;
	}

	UINT GPUHeap::MemAlloc(const UINT size, ComPtr<ID3D12Resource>& resource)  {
		assert(m_memAllocated + size <= m_Size);
		UINT offset = m_memAllocated;
		m_memAllocated += size;


		return offset;
	}
}