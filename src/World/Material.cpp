#include "Material.h"
#include "DescriptorHeap.h"
namespace Game {
	TextureMaterial::TextureMaterial(ptrPSO pso, ptrRootSigature rootSignature, ptrTexture texture)
		: Material(pso, rootSignature), m_texture(texture)
	{
	
	}

	void TextureMaterial::_UseMaterial(Graphic::CommandList& commandList) const 
	{
		// TODO need to remove code here to other place
		
		// Malloc a continous blocks from descriptor heap to bind resource
		UINT numTexture = 1;
		Graphic::DescriptorHeap* descriptorHeap = Engine::GetInUseHeap();
		UINT heapIndex = descriptorHeap->MallocHeap(numTexture);
		
		// Set Descriptor Heap
		commandList.SetDescriptorHeap(*descriptorHeap);
		// Set descriptor handle
		CD3DX12_GPU_DESCRIPTOR_HANDLE Hanlde = descriptorHeap->GetGPUHandle(heapIndex);
		m_texture->BindTexture(heapIndex);
		commandList.SetGraphicsRootDescriptorTable(1, Hanlde); // TODO emmmm
	}
}