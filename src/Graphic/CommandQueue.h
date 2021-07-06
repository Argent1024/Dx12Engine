#pragma once

#include "DXHelper.h"
#include "CommandList.h"

extern Graphic::CommandManager CopyHelper;
extern Graphic::CommandManager GraphicsCommandManager;
extern Graphic::CommandManager ComputeCommandManager;


namespace Graphic {

	//TODO Consider multi threading
	class CommandQueue {
	public:
		CommandQueue(D3D12_COMMAND_LIST_TYPE type=D3D12_COMMAND_LIST_TYPE_DIRECT,
					 D3D12_COMMAND_QUEUE_FLAGS flags=D3D12_COMMAND_QUEUE_FLAG_NONE):
			type(type), flags(flags), m_fenceCompletedValue(1), m_fenceValue(1)
		{}

		void Initialize();

		ID3D12CommandQueue* GetCommadnQueue() const { return m_commandQueue.Get(); }

		bool IsComplete(uint64_t value);

		// Wait On CPU
		void WaitCPU(uint64_t value);

		// Wait On CPU
		void WaitIdleCPU();

		// Wait On GPU (to synchronize different queue)
		void Stall(uint64_t value, CommandQueue* queue);
		void Stall(CommandQueue* queue);  // USELESS?

		// Wait GPU to finish all the work
		void Stall(uint64_t value);	
		
		/*	
		Return the fence value for this execute command finish
		Example usage:

			uint64_t fence = commandQueue->Execute(commandList);
			commandQueue->WaitCPU(fence);
			// the queue has finish working with the fence here

		*/
		uint64_t Execute(ID3D12CommandList* command);
			
		const  D3D12_COMMAND_LIST_TYPE type;
		const  D3D12_COMMAND_QUEUE_FLAGS flags;

	private:
		// Wait On GPU for the fence
		void Stall(uint64_t value, ComPtr<ID3D12Fence> fence);

		ComPtr<ID3D12CommandQueue> m_commandQueue;

		// Synchronization objects.
		uint64_t m_fenceCompletedValue;
		uint64_t m_fenceValue;
		HANDLE m_fenceEvent;
		ComPtr<ID3D12Fence> m_fence;
	};


	// User use this class to manage different type of Command Lists
	// Take care of command list allocators 
	// Help different command queue to wait each other
	/* Example usage:
		CommandManager Manager;
		CommandList list;

		for each frame
		{
			Manager.Start();
			Manager.Stall(AnotherManager);
			clear rtv or do other start job before the frame
			(multi thread) for each command list {
				Manager.InitCommandList(&list);
				DoRecord(list);
				Manager.ExecuteCommandList(&list);
			}
			Manager.End();
		} */

	class CommandManager {
	public:
		CommandManager(UINT AllocatorNum, 
			D3D12_COMMAND_LIST_TYPE Type=D3D12_COMMAND_LIST_TYPE_DIRECT, 
			D3D12_COMMAND_QUEUE_FLAGS Flags=D3D12_COMMAND_QUEUE_FLAG_NONE);

		void Initialize();

		inline void InitCommandList(CommandList* commandList) { commandList->Initialize(m_Allocators[m_AllocatorIndex].Get()); }
		
		void ExecuteCommandList(CommandList* commandList) 
		{
			commandList->Close();
			uint64_t fence = m_Queue.Execute(commandList->GetCommandList());
			m_Fences[m_AllocatorIndex] = std::max(m_Fences[m_AllocatorIndex], fence);
		}

		// Wait for the execution finsih before start, so that the allocator can be reset.
		void Start();
		
		//Switch the allocator after all command list have been sent to command queue
		void End();

		inline ID3D12CommandQueue* GetCommadnQueue() { return m_Queue.GetCommadnQueue(); }
		inline void WaitIdleCPU() { m_Queue.WaitIdleCPU(); }
		inline void WaitCPU(uint64_t value) { m_Queue.WaitCPU(value); }

		const D3D12_COMMAND_LIST_TYPE m_Type;

	private:
		const UINT m_AllocatorNum;
		UINT m_AllocatorIndex;

		CommandQueue m_Queue;
		std::vector<ComPtr<ID3D12CommandAllocator>> m_Allocators;
		std::vector<uint64_t> m_Fences;
	};

}