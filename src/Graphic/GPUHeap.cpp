#include "GPUHeap.h"

namespace Graphic {	
	void GPUHeap::Destory() {
		m_Heap = nullptr;
	}

	void GPUHeap::Initialize(ComPtr<ID3D12Device> device) {
		Destory();
		ThrowIfFailed(device->CreateHeap(&m_HeapDesc, IID_PPV_ARGS(&m_Heap)));
	}

}