#pragma once

#include "DXHelper.h"
#include "PipelineState.h"

namespace Graphic {
	
	class CommandList {
	public:
		CommandList(D3D12_COMMAND_LIST_TYPE type=D3D12_COMMAND_LIST_TYPE_DIRECT) : type(type) {}

		void Initialize(ComPtr<ID3D12Device> device);
		
		inline void Reset() const {
			ThrowIfFailed(m_commandAllocator->Reset());
			ThrowIfFailed(m_commandList->Reset(m_commandAllocator.Get(), nullptr));
		}

		inline void Close() const {
			ThrowIfFailed(m_commandList->Close());
		}
		
		inline void SetPipelineState(const PipelineStateObject& newPSO) const {
			ID3D12PipelineState* newPipelineState = newPSO.GetPSO();
			if(newPipelineState !=m_CurPipelineState) {
				m_commandList->SetPipelineState(newPipelineState);
			}
		}
		
		inline void SetPrimitiveTopology(D3D12_PRIMITIVE_TOPOLOGY topo) const {
			m_commandList->IASetPrimitiveTopology(topo);
		}
		
		inline void SetVertexBuffer(D3D12_VERTEX_BUFFER_VIEW* vertexBufferView) const {
			m_commandList->IASetVertexBuffers(0, 1, vertexBufferView);
		}

		inline void SetVertexBuffer(UINT startSlot, D3D12_VERTEX_BUFFER_VIEW* vertexBufferView) const {
			m_commandList->IASetVertexBuffers(startSlot, 1, vertexBufferView);
		}

		inline void SetVertexBuffers(UINT startSlot, UINT count, D3D12_VERTEX_BUFFER_VIEW* vertexBufferViews) const {
			m_commandList->IASetVertexBuffers(startSlot, count, vertexBufferViews);
		}

		// TODO Write API
		
		ID3D12GraphicsCommandList* GetCommandList() const { return m_commandList.Get(); }	
		const D3D12_COMMAND_LIST_TYPE type;

	private:
		// TODO diff between these commandlist?
		ComPtr<ID3D12GraphicsCommandList> m_commandList;
		// TODO modify allocator
		ComPtr<ID3D12CommandAllocator> m_commandAllocator;

		ID3D12PipelineState* m_CurPipelineState;
	};

		
}