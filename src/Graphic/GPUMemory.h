#pragma once

#include "GraphicCore.h"

namespace Graphic {
	class CommandManager;
	class CommandList;
}

namespace GPU {
	class GPUBuffer;
	class TextureBuffer;
	class GPUMemory;

	class GPUMemory 
	{
	public:
		GPUMemory()
		{	}

		// TODO other resource
		// Main function for Create Committed resource
		void DestoryAndCreateCommitted(const D3D12_RESOURCE_DESC* resourceDesc, 
			const D3D12_HEAP_PROPERTIES* heapProp, const D3D12_RESOURCE_STATES& initState,
			const D3D12_CLEAR_VALUE* clearValue, const D3D12_HEAP_FLAGS& flag) 
		{
			assert(resourceDesc != nullptr && heapProp != nullptr);
			Destroy();

			ID3D12Device* device = ::Engine::GetDevice();
			ThrowIfFailed(
				device->CreateCommittedResource(
							heapProp,
							D3D12_HEAP_FLAG_NONE,
							resourceDesc,
							initState,    
							clearValue,
							IID_PPV_ARGS(&m_Resource)));
		}

		virtual void Destroy() 
		{
			m_Resource = nullptr;
		}

		// TODO: Consider more about this later!
		inline D3D12_RESOURCE_BARRIER Barrier(D3D12_RESOURCE_STATES before, D3D12_RESOURCE_STATES after)
		{
			return CD3DX12_RESOURCE_BARRIER::Transition(this->GetResource(), before, after);
		}

		ID3D12Resource* GetResource() { return m_Resource.Get(); } 
		const ID3D12Resource* GetResource() const { return m_Resource.Get(); }

	protected:
		ComPtr<ID3D12Resource> m_Resource;
	};
}