#include "CommandList.h"

namespace Graphic {

	void CommandList::Initialize(ID3D12CommandAllocator* allocator, ComPtr<ID3D12Device> device) 
	{
		// TODO what is gpu node?
		ThrowIfFailed(device->CreateCommandList(1, type, allocator, nullptr, IID_PPV_ARGS(&m_commandList)));
	}


}