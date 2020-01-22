#include "CommandList.h"

namespace Graphic {

	void CommandList::Initialize(ComPtr<ID3D12Device> device) 
	{
		ThrowIfFailed(device->CreateCommandAllocator(type, IID_PPV_ARGS(&m_commandAllocator)));
		// TODO what is gpu node?
		ThrowIfFailed(device->CreateCommandList(1, type, m_commandAllocator.Get(), nullptr, IID_PPV_ARGS(&m_commandList)));
		m_commandList->Close();
	}


}