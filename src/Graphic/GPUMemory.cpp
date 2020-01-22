#include "GPUMemory.h"
namespace Graphic {
	UINT GPUMemory::MemAlloc(const UINT size) {
		assert(m_memAllocated + size <= m_size);
		UINT offset = m_memAllocated;
		m_memAllocated += size;
		return offset;
	}
}