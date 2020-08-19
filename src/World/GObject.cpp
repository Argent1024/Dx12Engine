#include "GObject.h"

namespace Game {
	using namespace Math;
	
	GObject::GObject() 
		: m_table(2),
		  m_CBV(Engine::MemoryAllocator.CreateCommittedBuffer(cbSize, D3D12_HEAP_TYPE_UPLOAD), 
			    cbSize)
	{
		// TODO Fix this
		m_state = new Physic::PState();

		// slot 0 in the table is for const data
		m_CBV.CreateView(m_table, 0);
	}

	void GObject::RecordCommand(Graphic::CommandList& commandList) {
		assert(m_Mesh && "Not initialized GameObj");
		// (TODO may not need to do this every frame & remove it to Physic) Upload data to the cbv
		XMFLOAT4X4 modelTransform;
		XMStoreFloat4x4(&modelTransform, XMMatrixTranspose(m_state->GetTransform()));
		m_CBV.copyData(&modelTransform);

		commandList.SetDescriptorHeap(*Engine::GetInUseHeap());
		
		CD3DX12_GPU_DESCRIPTOR_HANDLE tableHandle = m_table.BindDescriptorTable();
		commandList.SetGraphicsRootDescriptorTable(2, tableHandle);  //  slot 2  in root signature is reversed for object
			
		m_Mesh->UseMesh(commandList);
	}

		
}