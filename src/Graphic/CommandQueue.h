#pragma once

#include "DXHelper.h"
#include "CommandList.h"

namespace Graphic {

	//TODO Consider multi threading
	class CommandQueue {
	public:
		CommandQueue(D3D12_COMMAND_LIST_TYPE type=D3D12_COMMAND_LIST_TYPE_DIRECT,
					 D3D12_COMMAND_QUEUE_FLAGS flags=D3D12_COMMAND_QUEUE_FLAG_NONE):
			type(type), flags(flags), m_fenceCompletedValue(1), m_fenceValue(1)
		{}

		void Initialize(ComPtr<ID3D12Device> device);

		ID3D12CommandQueue* GetCommadnQueue() const { return m_commandQueue.Get(); }

		bool IsComplete(uint64_t value);

		// Wait On CPU
		void WaitCPU(uint64_t value);

		// Wait On CPU
		void WaitIdleCPU();

		// Wait On GPU (to synchronize different queue)
		void Stall(uint64_t value, ComPtr<ID3D12Fence> fence);
		void Stall(uint64_t value, CommandQueue* queue);
		void Stall(CommandQueue* queue);

		// Wait GPU to finish all the work
		void Stall(uint64_t value);	
		
		uint64_t Execute(ID3D12CommandList* command);
			
		const  D3D12_COMMAND_LIST_TYPE type;
		const  D3D12_COMMAND_QUEUE_FLAGS flags;

	private:
		ComPtr<ID3D12CommandQueue> m_commandQueue;

		// Synchronization objects.
		uint64_t m_fenceCompletedValue;
		uint64_t m_fenceValue;
		HANDLE m_fenceEvent;
		ComPtr<ID3D12Fence> m_fence;
	};


	class CommandAllocator {
	public:
		CommandAllocator(D3D12_COMMAND_LIST_TYPE Type, UINT Size)
			: m_Size(Size), m_Type(Type) {}

		void Initialize(ComPtr<ID3D12Device> device);
		ID3D12CommandAllocator* GetAllocator(UINT index);

	private:
		const UINT m_Size;
		const D3D12_COMMAND_LIST_TYPE m_Type;
		std::vector<ComPtr<ID3D12CommandAllocator>> m_Allocators;
	};
}