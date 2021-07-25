#include "stdafx.h"
#include "GObject.h"

namespace Game {
	using namespace Math;
	
	GObject::GObject() 
		: m_CBV()
	{
		// TODO Fix this
		m_state = new Physics::PhysicsState();

		m_CBV.Initialize();
		m_CBV.CreateRootView();
	}

	void GObject::RecordCommand(Graphic::CommandList& commandList) 
	{
		assert(m_Mesh && "Not initialized GameObj");
		// (TODO may not need to do this every frame & remove it to Physics) Upload data to the cbv
		auto& cbdata = m_CBV.GetData();
		XMStoreFloat4x4(&cbdata.T, XMMatrixTranspose(m_state->GetTransform()));
		m_CBV.UpdateData();

		commandList.SetGraphicsRootCBV(Graphic::RootSignature::ObjectCBV, m_CBV);

		// Mesh
		m_Mesh->UseMesh(commandList);

		// Material 
		Graphic::DescriptorTable& matTable = m_Material->BindMaterialAt();
		CD3DX12_GPU_DESCRIPTOR_HANDLE matTableHandle = matTable.BindDescriptorTable();
		commandList.SetGraphicsRootDescriptorTable(Graphic::RootSignature::MaterialDTable, matTableHandle);
	}

		
}