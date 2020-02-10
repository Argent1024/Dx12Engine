#include "Material.h"
#include "DescriptorHeap.h"
namespace Game {
	TextureMaterial::TextureMaterial(ptrPSO pso, ptrRootSigature rootSignature, ptrTexture texture)
		: Material(pso, rootSignature), m_texture(texture), m_HeapIndex(0)
	{
	
	}

	void TextureMaterial::_UseMaterial(Graphic::CommandList& commandList) const 
	{	
		// TODO place else where
		// Set Descriptor Heap
		Graphic::DescriptorHeap* descriptorHeap = Engine::GetInUseHeap();
		commandList.SetDescriptorHeap(*descriptorHeap);
		// Set descriptor handle
		CD3DX12_GPU_DESCRIPTOR_HANDLE Hanlde = descriptorHeap->GetGPUHandle(m_HeapIndex);
		m_texture->BindTexture(m_HeapIndex);
		commandList.SetGraphicsRootDescriptorTable(1, Hanlde); // TODO emmmm
	}

	void TextureMaterial::LoadMaterial()  
	{
		// Malloc a continous blocks from descriptor heap to bind resource
		UINT numTexture = 1;
		Graphic::DescriptorHeap* descriptorHeap = Engine::GetInUseHeap();
		UINT index = descriptorHeap->MallocHeap(numTexture);
		m_HeapIndex = index;
	}

}