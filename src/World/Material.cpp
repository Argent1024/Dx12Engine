#include "Material.h"


namespace Game {

	/*void SimpleMaterial::UseMaterial(Graphic::CommandList& commandList, UINT slot) 
	{
		CD3DX12_GPU_DESCRIPTOR_HANDLE tableHandle = m_table.BindDescriptorTable();
		commandList.SetGraphicsRootDescriptorTable(slot, tableHandle);
	}*/

	void PrincipleMaterial::Initialize() 
	{
		UINT cbvSize = CalculateConstantBufferByteSize(sizeof(PrincipleMaterial::Data));
		ptrGPUMem gpumem = Engine::MemoryAllocator.CreateCommittedBuffer(cbvSize);
		m_MatCBV = new Graphic::ConstantBuffer(gpumem, cbvSize);
		m_MatCBV->Initialize();
	}

}