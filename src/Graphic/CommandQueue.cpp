#include "stdafx.h"
#include "CommandQueue.h"

namespace Graphic {
	CommandManager CopyHelper(1);

	void CommandQueue::Initialize() {
		ID3D12Device* device = Engine::GetDevice();
		D3D12_COMMAND_QUEUE_DESC queueDesc = {};
		queueDesc.Flags = flags;
		queueDesc.Type = type;
		ThrowIfFailed(device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_commandQueue)));
		ThrowIfFailed(device->CreateFence(m_fenceCompletedValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence)));
	}

	bool CommandQueue::IsComplete(uint64_t value) {
		if (value > m_fenceCompletedValue) {
			m_fenceCompletedValue = std::max(m_fence->GetCompletedValue(), m_fenceCompletedValue);
		}
		return value < m_fenceCompletedValue;
	}

	// Wait on CPU
	void CommandQueue::WaitCPU(uint64_t value) {
		// Already finish
		if (IsComplete(value)) {
			return;
		}

		// TODO multi threading here
		m_fence->SetEventOnCompletion(value, m_fenceEvent);
		WaitForSingleObject(m_fenceEvent, INFINITE);
		m_fenceCompletedValue = value;
		return;
	}

	// Wait on CPU for nothing
	void CommandQueue::WaitIdleCPU() {
		// TODO multi threading here
		m_fenceValue++;
		m_commandQueue->Signal(m_fence.Get(), m_fenceValue);
		WaitCPU(m_fenceValue);
	}

	// Wait On GPU (to synchronize different queue)
	// TODO multi threadingf here
	void CommandQueue::Stall(uint64_t value, ComPtr<ID3D12Fence> fence) {
		assert(fence.Get() != NULL);
		m_commandQueue->Wait(fence.Get(), value);
	}

	void CommandQueue::Stall(uint64_t value, CommandQueue* queue) {
		if (queue->IsComplete(value)) {
			// return if that queue has already reach the fence value
			return;
		} else {
			Stall(value, queue->m_fence);
		}
	}

	void CommandQueue::Stall(CommandQueue* queue) {
		Stall(queue->m_fenceValue, queue->m_fence);
	}

	// Tell GPU to wait until all the work finish (it seems useless...??)
	void CommandQueue::Stall(uint64_t value) {
		assert(m_fence.Get() != NULL);
		assert(value >= 0);
		m_commandQueue->Wait(m_fence.Get(), value);
	}

	uint64_t CommandQueue::Execute(ID3D12CommandList* command) {
		// TODO multi threading here
		m_fenceValue++;
		m_commandQueue->ExecuteCommandLists(1, &command);
		m_commandQueue->Signal(m_fence.Get(), m_fenceValue);
		return m_fenceValue;
	}


	CommandManager::CommandManager(UINT AllocatorNum,
		D3D12_COMMAND_LIST_TYPE Type, D3D12_COMMAND_QUEUE_FLAGS Flags)
		:  m_Queue(Type, Flags), m_Type(Type),
		m_AllocatorIndex(0), m_AllocatorNum(AllocatorNum)
	{
	}

	void CommandManager::Initialize() {
		ID3D12Device* device = Engine::GetDevice();
		m_Queue.Initialize();

		// Create allocators
		for (UINT i = 0; i < m_AllocatorNum; ++i)
		{
			ComPtr<ID3D12CommandAllocator> allocator;
			ThrowIfFailed(device->CreateCommandAllocator(m_Type, IID_PPV_ARGS(&allocator)));
			m_Allocators.push_back(allocator);
			m_Fences.push_back(0);
		}
	
	}

	void CommandManager::Start() {
		const uint64_t fence = m_Fences[m_AllocatorIndex];
		m_Queue.WaitCPU(fence);
		m_Allocators[m_AllocatorIndex]->Reset();
	}

	void CommandManager::End() {
		m_AllocatorIndex = (m_AllocatorIndex + 1) % m_AllocatorNum;
	}
}