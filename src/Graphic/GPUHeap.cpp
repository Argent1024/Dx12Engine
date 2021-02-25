#include "stdafx.h"
#include "GPUHeap.h"

namespace Graphic {	
	void GPUHeap::Destroy() {
		m_Heap = nullptr;
	}

	UINT GPUHeap::MemAlloc(const UINT size)  {
		assert(m_memAllocated + size <= m_Size);
		//TODO assert align stuff
		UINT HeapOffset = m_NumOfBuffers;
		m_memAllocated += size;
		m_NumOfBuffers += 1;
		return HeapOffset;
	}
}