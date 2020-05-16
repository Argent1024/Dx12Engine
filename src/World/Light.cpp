#include "Light.h"
#include "GraphicCore.h"


namespace Game {
	
	DirectionLight::DirectionLight(Vector3 dir, Vector3 radiance,
		UINT shadowMapW, UINT shadowMapH, UINT textureNum)
		: Light(), m_Camera(shadowMapW, shadowMapH), 
		  m_dir(dir), m_Radiance(radiance),
		  m_DepthBuffer(shadowMapW, shadowMapH),
		  m_Table(textureNum)
	{ }
	     
	void DirectionLight::Initialize() 
	{
		m_Table.Initialize(Engine::GetInitHeap());

		m_DepthBuffer.Initialize(Graphic::TEXTURE_DSV | Graphic::TEXTURE_SRV);
		m_Camera.Initialize();

		// cbv for constant at slot 0
		UINT cbv_size = 1024;
		ptrGPUMem gpumem = Engine::MemoryAllocator.CreateCommittedBuffer(cbv_size);
		m_CBV = new Graphic::ConstantBuffer(gpumem, cbv_size, m_Table, 0);

		// depth texture at slot 1
		Graphic::Texture* depthTexture = m_DepthBuffer.GetTexture();
		depthTexture->CreateView(Graphic::TEXTURE_SRV, &m_Table, 1);

		// TODO Other Texture
	}

	void DirectionLight::UseLight(Graphic::CommandList& commandList, UINT slot)
	{
		// Write constant buffer
		XMVECTOR data[] = {(XMVECTOR)m_dir, (XMVECTOR)m_Radiance};
		const UINT size = 2 * sizeof(XMVECTOR);
		m_CBV->copyData(data, size);

		CD3DX12_GPU_DESCRIPTOR_HANDLE tableHandle = m_Table.BindDescriptorTable();
		commandList.SetGraphicsRootDescriptorTable(slot, tableHandle);
	}

	void DirectionLight::UseDepth(Graphic::CommandList& commandList)
	{
		// Set Root CBV (Transformation)
		m_Camera.UseCamera(commandList);
		commandList.SetDepthBuffer(m_DepthBuffer);
	}
}