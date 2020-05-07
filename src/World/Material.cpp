#include "Material.h"
#include "DescriptorHeap.h"
namespace Game {

	void SimpleMaterial::UseMaterial(Graphic::CommandList& commandList) 
	{
		CD3DX12_GPU_DESCRIPTOR_HANDLE tableHandle = m_table.BindDescriptorTable();
		// TODO Slot??
		commandList.SetGraphicsRootDescriptorTable(1, tableHandle);
	}

}