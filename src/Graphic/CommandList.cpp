#include "CommandList.h"

namespace Graphic {

	void CommandList::Initialize(ID3D12CommandAllocator* allocator) 
	{
		// TODO what is gpu node?
		ID3D12Device* device = Engine::GetDevice();
		ThrowIfFailed(device->CreateCommandList(1, type, allocator, nullptr, IID_PPV_ARGS(&m_commandList)));
	}


}