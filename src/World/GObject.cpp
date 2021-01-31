#include "GObject.h"

namespace Game {
	using namespace Math;
	
	GObject::GObject() 
		: m_table(4), // TODO table size according to material
		  m_CBV()
	{
		// TODO Fix this
		m_state = new Physic::PState();

		// Slot 0 in the table is for const data
		// Init CBV and put CBV to slot 0 in the descriptor table
		ptrGBuffer buffer = GPU::MemoryManager::CreateGBuffer();
		buffer->Initialize(cbSize);	// TODO need upload? or default is fine?
		m_CBV.Initialze(buffer, cbSize);
		m_CBV.CreateView(&m_table, 0);
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