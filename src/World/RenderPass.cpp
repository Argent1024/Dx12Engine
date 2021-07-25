#include "stdafx.h"
#include "RenderPass.h"
#include "../Utility/MeshReader.h"
#include "FFTOcean.h"

namespace Game {
	using namespace Math;
	using namespace Graphic;
	void DefaultRenderPass::Initialize(ptrGraphicsPSO pso) 
	{
		// TODO Use only one root signature
		m_rootSignature = std::make_shared<Graphic::RootSignature>();
		m_rootSignature->Initialize();

		// TODO move pso to material
		// m_PSO = std::make_shared<Graphic::DefaultPSO>();
		// m_PSO = std::make_shared<OceanPSO>();
		m_PSO = pso;
		m_PSO->SetRootSigature(m_rootSignature->GetRootSignature());
		m_PSO->Initialize();

		
		// Create Descriptor Table
		// TODO determine how large the table should be
		const UINT tableSize = 4;
		m_DescriptorTable = new Graphic::DescriptorTable(tableSize);
	

		// Creata CBV 
		m_ConstBuffer.Initialize();
		m_ConstBuffer.CreateView(m_DescriptorTable, 0);
		

		// The other textures needed is binded outside by render engine
	}

	void DefaultRenderPass::PrepareData(const Scene& scene)
	{
		// TODO no need to update this frequently
		// Copy to CBV
		m_ConstBuffer.UpdateData();//, sizeof(CameraBufferData));
	}

	void DefaultRenderPass::Render(Graphic::CommandList& commandList, const Scene& scene) {
		
		commandList.SetDescriptorHeap(*Engine::GetInUseHeap());
		commandList.SetPipelineState(m_PSO);
		commandList.SetGraphicsRootSignature(m_rootSignature);
		
		auto& CameraCB = scene.GetCameraCB();
		commandList.SetGraphicsRootCBV(RootSignature::CameraCBV, CameraCB);
		const Camera& camera = scene.GetMainCamera();
		camera.UseCamera(commandList);


		CD3DX12_GPU_DESCRIPTOR_HANDLE RenderPassTable = m_DescriptorTable->BindDescriptorTable();
		commandList.SetGraphicsRootDescriptorTable(RootSignature::RenderpassDTable, RenderPassTable);
		
		const Graphic::DescriptorTable* sceneTable = scene.GetSceneTable();
		CD3DX12_GPU_DESCRIPTOR_HANDLE sceneTableHandle = sceneTable->BindDescriptorTable();
		commandList.SetGraphicsRootDescriptorTable(RootSignature::SceneDTable, sceneTableHandle);

		
		if (m_ObjRenderType == RenderTypeSkyBox) {
			auto const& skybox = scene.GetSkyBox();
			if (skybox != nullptr) {
				skybox->RecordCommand(commandList);
				skybox->Draw(commandList);
			}
		} else {
			const std::vector<GObject*>& objList = scene.GetGameObjects(m_ObjRenderType);
			for (auto const& g_obj : objList)
			{
				g_obj->RecordCommand(commandList);
				g_obj->Draw(commandList);
			}
		}
	}

	MixtureRenderPass::MixtureRenderPass(UINT num_texture, const UINT width, const UINT height)
		: m_Viewport(0.0f, 0.0f, static_cast<float>(width), static_cast<float>(height)),
		  m_ScissorRect(0, 0, static_cast<LONG>(width), static_cast<LONG>(height))
		 { m_DescriptorTable = new Graphic::DescriptorTable(num_texture); }

	void MixtureRenderPass::Initialize(ptrGraphicsPSO) 
	{

		// TODO Use only one root signature
		m_rootSignature = std::make_shared<Graphic::RootSignature>();
		m_rootSignature->Initialize();

		m_PSO = std::make_shared<Graphic::MixturePSO>();
		m_PSO->SetRootSigature(m_rootSignature->GetRootSignature());
		m_PSO->Initialize();

		// Create Mesh
		// Normal are useless here
		// Tex Coor map Top left to (0, 0), Top right to (1, 0), Bottom left to (0, 1)
		std::vector<DefaultVertex> triangleVertices =
		{
			{ { 1.0f,  1.0f, 0.0f }, { 1.0f, 1.0f, 1.0f }, { 1.0f, 0.0f  } },
			{ { 1.0f, -1.0f, 0.0f }, { 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f } },
			{ { -1.0f, 1.0f, 0.0f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f } },
			{ { -1.0f, -1.0f, 0.0f }, { 1.0f, 1.0f, 1.0f}, { 0.0f, 1.0f } }
		};
		std::vector<UINT> index_list = { 0, 1, 2, 3, 2, 1 };
		m_RenderScreen = std::make_shared<TriangleMesh>(triangleVertices, index_list);

	}

	void MixtureRenderPass::Render(Graphic::CommandList& commandList, const Scene& scene) 
	{
		commandList.SetViewPorts(&m_Viewport);
		commandList.SetScissorRects(&m_ScissorRect);
		commandList.SetPipelineState(m_PSO);
		commandList.SetGraphicsRootSignature(m_rootSignature);
		commandList.SetDescriptorHeap(*Engine::GetInUseHeap());

		CD3DX12_GPU_DESCRIPTOR_HANDLE handle = m_DescriptorTable->BindDescriptorTable();
		commandList.SetGraphicsRootDescriptorTable(2, handle);

		m_RenderScreen->UseMesh(commandList);
		m_RenderScreen->Draw(commandList);
	}
}