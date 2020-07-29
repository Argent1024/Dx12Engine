#include "Material.h"


namespace Game {

	/*void SimpleMaterial::UseMaterial(Graphic::CommandList& commandList, UINT slot) 
	{
		CD3DX12_GPU_DESCRIPTOR_HANDLE tableHandle = m_table.BindDescriptorTable();
		commandList.SetGraphicsRootDescriptorTable(slot, tableHandle);
	}*/

	void PrincipleMaterial::Initialize() 
	{
		assert(sizeof(PrincipleMaterial::Data) % 16 == 0);
		UINT cbvSize = CalculateConstantBufferByteSize(sizeof(PrincipleMaterial::Data));
		ptrGPUMem gpumem = Engine::MemoryAllocator.CreateCommittedBuffer(cbvSize);
		m_MatCBV = new Graphic::ConstantBuffer(gpumem, cbvSize);
		m_MatCBV->Initialize();
	}

	void PrincipleMaterial::BindMaterialAt(Graphic::DescriptorTable& table) 
	{
		m_MatCBV->CreateView(table, MatCBV);

		if (m_MatData.NTexture) {
			BindTexture(Normal, table);
		}
		
		// TODO other textures
	}

}