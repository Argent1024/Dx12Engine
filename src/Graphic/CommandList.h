#pragma once

#include "GraphicCore.h"
#include "PipelineState.h"
#include "RootSignature.h"
#include "Descriptor.h"
#include "SwapChain.h"
#include "DepthBuffer.h"


namespace Graphic {
	
	class CommandList {
	public:
		CommandList(D3D12_COMMAND_LIST_TYPE type = D3D12_COMMAND_LIST_TYPE_DIRECT) : type(type) {}

		void Initialize(ID3D12CommandAllocator* allocator);

		inline void Reset(ID3D12CommandAllocator* allocator) const {
			ThrowIfFailed(m_commandList->Reset(allocator, nullptr));
		}

		inline void Close() const {
			ThrowIfFailed(m_commandList->Close());
		}

		// Avoid set PSO twice
		inline void SetPipelineState(ptrPSO newPSO) {
			
			if (newPSO != m_CurPipelineState) {
				ID3D12PipelineState* newPipelineState = newPSO->GetPSO();
				m_commandList->SetPipelineState(newPipelineState);
				m_CurPipelineState = newPSO;
			}
		}

		// Avoid set root signature twice
		inline void SetGraphicsRootSignature(ptrRootSignature newRS) {
			if (newRS != m_CurRootSignature) {
				assert("");
				ID3D12RootSignature* newRootSignature = newRS->GetRootSignature();
				m_commandList->SetGraphicsRootSignature(newRootSignature);
				m_CurRootSignature = newRS;
			}
		}

		// Avoid set root signature twice
		inline void SetComputeRootSignature(ptrRootSignature newRS) {
			if (newRS != m_CurRootSignature) {
				assert("");
				ID3D12RootSignature* newRootSignature = newRS->GetRootSignature();
				m_commandList->SetComputeRootSignature(newRootSignature);
				m_CurRootSignature = newRS;
			}
		}

		inline void SetPrimitiveTopology(D3D12_PRIMITIVE_TOPOLOGY topo) const {m_commandList->IASetPrimitiveTopology(topo);}

		inline void SetIndexBuffer(const IndexBuffer& ib) const {m_commandList->IASetIndexBuffer(ib.GetIndexView());}

		inline void SetVertexBuffer(const VertexBuffer& vb) const {m_commandList->IASetVertexBuffers(0, 1, vb.GetBufferView());}

		inline void SetGraphicRootCBV(ConstantBuffer* cbv, UINT rootParaIndex) const {
			m_commandList->SetGraphicsRootConstantBufferView(rootParaIndex, cbv->GetRootCBVGPUAdder()); 
		}

		inline void DrawInstanced(UINT VertexCountPerInstance, UINT InstanceCount, UINT StartVertexLocation, UINT StartInstanceLocation) const { m_commandList->DrawInstanced(VertexCountPerInstance, InstanceCount, StartVertexLocation, StartInstanceLocation);}

		inline void DrawIndexedInstanced(UINT IndexCountPerInstance, UINT InstanceCount, UINT StartIndexLocation, INT BaseVertexLocation, UINT StartInstanceLocation) const {m_commandList->DrawIndexedInstanced(IndexCountPerInstance, InstanceCount, StartIndexLocation, BaseVertexLocation, StartInstanceLocation);}

		// Swap Chain
		inline void SetSwapChain(const SwapChain& swapChain) { m_commandList->OMSetRenderTargets(1, &(swapChain.GetBackBufferCPUHandle()), FALSE, nullptr); }
		// swap chain with depth buffer
		inline void SetSwapChain(const SwapChain& swapChain, const DepthBuffer& depthBuffer)
		{ m_commandList->OMSetRenderTargets(1, &(swapChain.GetBackBufferCPUHandle()), FALSE, &(depthBuffer.GetDepthBufferCPUHandle())); }
		
		// Set only the depth buffer
		inline void SetDepthBuffer(const DepthBuffer& depthBuffer)
		{ m_commandList->OMSetRenderTargets(0, nullptr,FALSE, &(depthBuffer.GetDepthBufferCPUHandle())); }

		inline void ClearSwapChain(const SwapChain& swapChain, const float color[4]) const { m_commandList->ClearRenderTargetView(swapChain.GetBackBufferCPUHandle(), color, 0, nullptr);}
		
		inline void ResourceBarrier(const SwapChain& swapChain, D3D12_RESOURCE_STATES before, D3D12_RESOURCE_STATES after) { m_commandList->ResourceBarrier(1, &(swapChain.Barrier(before, after))); }
		
		inline void ClearDepthBuffer(DepthBuffer& depthBuffer) { m_commandList->ClearDepthStencilView(depthBuffer.GetDepthBufferCPUHandle(), D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr); }

		// ProjectiveCamera
		inline void SetViewPorts(const CD3DX12_VIEWPORT* viewPort) { m_commandList->RSSetViewports(1, viewPort); }
		inline void SetScissorRects(const CD3DX12_RECT* scissorRect) { m_commandList->RSSetScissorRects(1, scissorRect); }
		
		// GPU memory 
		inline void ResourceBarrier(GPU::GPUMemory& gpumem, D3D12_RESOURCE_STATES before, D3D12_RESOURCE_STATES after) { m_commandList->ResourceBarrier(1, &(gpumem.Barrier(before, after))); }
		
		inline void CopyBufferRegion(GPU::GPUMemory& dest, UINT destOffset, GPU::GPUMemory& src, UINT srcOffset, UINT sizeBytes)  {
			m_commandList->CopyBufferRegion(dest.GetResource(), destOffset, src.GetResource(), srcOffset, sizeBytes); 
		}

		inline void CopyTextureRegion() {
			throw std::runtime_error("not implemented!");
		}

		// Descriptor stuff
		inline void SetDescriptorHeap(DescriptorHeap& descriptorHeap) 
		{
			ID3D12DescriptorHeap* heap = descriptorHeap.GetDescriptorHeap();
			if (heap != InUseHeap) {
				InUseHeap = heap;
				m_commandList->SetDescriptorHeaps(1, &heap);
			}
		}

		inline void SetGraphicsRootDescriptorTable(UINT rootSlot, CD3DX12_GPU_DESCRIPTOR_HANDLE handle)
		{
			m_commandList->SetGraphicsRootDescriptorTable(rootSlot, handle);
		}

		// Set root signature
		inline void SetGraphicsRootConstants(UINT ParaIndex, UINT Size32Bit, const void* data, UINT Offset32Bit=0) { m_commandList->SetGraphicsRoot32BitConstants(ParaIndex, Size32Bit, data, Offset32Bit); }
	
		// TODO Write API
		
		ID3D12GraphicsCommandList* GetCommandList() const { return m_commandList.Get(); }	
		const D3D12_COMMAND_LIST_TYPE type;

	private:
		// TODO diff between these commandlist?
		ComPtr<ID3D12GraphicsCommandList> m_commandList;
		ID3D12DescriptorHeap* InUseHeap;
		ptrPSO m_CurPipelineState;
		ptrRootSignature m_CurRootSignature;
	};

		
}