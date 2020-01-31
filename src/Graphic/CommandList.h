#pragma once

#include "DXHelper.h"
#include "PipelineState.h"
#include "RootSignature.h"
#include "Descriptor.h"
#include "SwapChain.h"

namespace Graphic {
	
	class CommandList {
	public:
		CommandList(D3D12_COMMAND_LIST_TYPE type = D3D12_COMMAND_LIST_TYPE_DIRECT) : type(type) {}

		void Initialize(ID3D12CommandAllocator* allocator, ComPtr<ID3D12Device> device);

		inline void Reset(ID3D12CommandAllocator* allocator) const {
			ThrowIfFailed(m_commandList->Reset(allocator, nullptr));
		}

		inline void Close() const {
			ThrowIfFailed(m_commandList->Close());
		}

		// Avoid set PSO twice
		inline void SetPipelineState(const PipelineStateObject& newPSO) {
			ID3D12PipelineState* newPipelineState = newPSO.GetPSO();
			if (newPipelineState != m_CurPipelineState) {
				m_commandList->SetPipelineState(newPipelineState);
				m_CurPipelineState = newPipelineState;
			}
		}

		// Avoid set root signature twice
		inline void SetGraphicsRootSignature(const RootSignature& newRS) {
			ID3D12RootSignature* newRootSignature = newRS.GetRootSignature();
			if (newRootSignature != m_CurRootSignature) {
				assert("");
				m_commandList->SetGraphicsRootSignature(newRootSignature);
				m_CurRootSignature = newRootSignature;
			}
		}

		// Avoid set root signature twice
		inline void SetComputeRootSignature(const RootSignature& newRS) {
			ID3D12RootSignature* newRootSignature = newRS.GetRootSignature();
			if (newRootSignature != m_CurRootSignature) {
				assert("");
				m_commandList->SetComputeRootSignature(newRootSignature);
				m_CurRootSignature = newRootSignature;
			}
		}

		inline void SetPrimitiveTopology(D3D12_PRIMITIVE_TOPOLOGY topo) const {m_commandList->IASetPrimitiveTopology(topo);}

		inline void SetIndexBuffer(const IndexBuffer& ib) const {m_commandList->IASetIndexBuffer(ib.GetIndexView());}

		inline void SetVertexBuffer(const VertexBuffer& vb) const {m_commandList->IASetVertexBuffers(0, 1, vb.GetBufferView());}

		inline void DrawInstanced(UINT VertexCountPerInstance, UINT InstanceCount, UINT StartVertexLocation, UINT StartInstanceLocation) const { m_commandList->DrawInstanced(VertexCountPerInstance, InstanceCount, StartVertexLocation, StartInstanceLocation);}

		inline void DrawIndexedInstanced(UINT IndexCountPerInstance, UINT InstanceCount, UINT StartIndexLocation, INT BaseVertexLocation, UINT StartInstanceLocation) const {m_commandList->DrawIndexedInstanced(IndexCountPerInstance, InstanceCount, StartIndexLocation, BaseVertexLocation, StartInstanceLocation);}

		// Swap Chain
		inline void SetSwapChain(const SwapChain& swapChain) { m_commandList->OMSetRenderTargets(1, &(swapChain.GetBackBufferCPUHandle()), FALSE, nullptr); ; }
		inline void ClearSwapChain(const SwapChain& swapChain, const float color[4]) const { m_commandList->ClearRenderTargetView(swapChain.GetBackBufferCPUHandle(), color, 0, nullptr);}

		// TODO Write API
		
		ID3D12GraphicsCommandList* GetCommandList() const { return m_commandList.Get(); }	
		const D3D12_COMMAND_LIST_TYPE type;

	private:
		// TODO diff between these commandlist?
		ComPtr<ID3D12GraphicsCommandList> m_commandList;

		ID3D12PipelineState* m_CurPipelineState;
		ID3D12RootSignature* m_CurRootSignature;
	};

		
}