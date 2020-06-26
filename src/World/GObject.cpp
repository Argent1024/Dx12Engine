#include "GObject.h"

namespace Game {

	void GObject::Initialize() {
		m_table.Initialize(Engine::GetInitHeap());
		
		// TODO temperate cbv
		const UINT cbSize = CalculateConstantBufferByteSize(sizeof(DirectX::XMMATRIX));
		/*m_Texture = new Graphic::TextureBuffer(cbSize);
		m_Texture->CreateView(Graphic::TEXTURE_CBV, &m_table, 0); */
		ptrGPUMem gpumem = Engine::MemoryAllocator.CreateCommittedBuffer(cbSize, D3D12_HEAP_TYPE_UPLOAD);
		m_CBV = new Graphic::ConstantBuffer(gpumem, cbSize);
		m_CBV->Initialize();
		m_CBV->CreateView(m_table, 0);
		// slot 0 in the table is for const data
	}

	void GObject::RecordCommand(Graphic::CommandList& commandList) {
		assert(m_Mesh && "Not initialized GameObj");
		// Upload data to the cbv
		m_CBV->copyData((&(DirectX::XMMATRIX)m_Transform));

		commandList.SetDescriptorHeap(*Engine::GetInUseHeap());


		Graphic::DescriptorHeap* heap = Engine::GetInUseHeap();
		CD3DX12_GPU_DESCRIPTOR_HANDLE tableHandle = m_table.BindDescriptorTable();
		commandList.SetGraphicsRootDescriptorTable(2, tableHandle);  //  slot 2  in root signature is reversed for object
			
		m_Mesh->UseMesh(commandList);
	}

		
}