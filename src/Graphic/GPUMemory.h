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

		// TODO: Consider more about this later!
		inline D3D12_RESOURCE_BARRIER Barrier(D3D12_RESOURCE_STATES before, D3D12_RESOURCE_STATES after)
		{
			return CD3DX12_RESOURCE_BARRIER::Transition(this->GetResource(), before, after);
		}

		inline ID3D12Resource* GetResource() { return m_Resource.Get(); } 
		inline const ID3D12Resource* GetResource() const { return m_Resource.Get(); }

		inline D3D12_RESOURCE_DESC GetDesc() const { return m_Resource->GetDesc(); }
		inline HRESULT GetHeapProp(D3D12_HEAP_PROPERTIES* prop, D3D12_HEAP_FLAGS* flag) const { return m_Resource->GetHeapProperties(prop, flag); }

		virtual void Destory() {
			m_Resource = nullptr;
		}

		// Main init to call, create committed buffer
		void DestoryAndCreateCommitted(const D3D12_RESOURCE_DESC* resourceDesc, 
			const D3D12_HEAP_TYPE heapType, 
			const D3D12_RESOURCE_STATES initState,
			const D3D12_CLEAR_VALUE* clearValue)
		{	
			assert(resourceDesc != nullptr);
			// Calculate Size

			D3D12_HEAP_PROPERTIES heapProp = CD3DX12_HEAP_PROPERTIES(heapType);
			D3D12_HEAP_FLAGS flag = D3D12_HEAP_FLAG_NONE;
			Destory();
			// Creat committed resource
			ID3D12Device* device = ::Engine::GetDevice();
			ThrowIfFailed(device->CreateCommittedResource(&heapProp, flag, resourceDesc, 
					initState, clearValue, IID_PPV_ARGS(&m_Resource)));
		}

	protected:
		ComPtr<ID3D12Resource> m_Resource;
	};
}