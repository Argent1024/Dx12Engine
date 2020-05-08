#include "RenderPass.h"
#include "../Utility/MeshReader.h"

namespace Game {

	void DefaultRenderPass::Initialize() {
		m_PSO = std::make_shared<Graphic::DefaultPSO>();
		m_rootSignature = std::make_shared<Graphic::RootSignature>();


		m_rootSignature->Initialize();

		m_PSO->SetRootSigature(m_rootSignature->GetRootSignature());
		m_PSO->Initialize();
	}

	void DefaultRenderPass::Render(Graphic::CommandList& commandList, std::vector<GObject*>& objList) {
		assert(m_Camera != nullptr);
		commandList.SetPipelineState(m_PSO);
		commandList.SetGraphicsRootSignature(m_rootSignature);
		for (auto const& g_obj : objList)
		{
			g_obj->RecordCommand(commandList);

			m_Camera->UseCamera(commandList, g_obj->GetTransform());
			g_obj->Draw(commandList);
		}
		
	}

	MixtureRenderPass::MixtureRenderPass(UINT num_texture, const UINT width, const UINT height)
		: m_Viewport(0.0f, 0.0f, static_cast<float>(width), static_cast<float>(height)),
		  m_ScissorRect(0, 0, static_cast<LONG>(width), static_cast<LONG>(height))
	{
		m_MixtureTextures = std::make_shared<SimpleMaterial>(num_texture);
	}

	void MixtureRenderPass::Initialize() 
	{
		m_PSO = std::make_shared<Graphic::MixturePSO>();
		m_rootSignature = std::make_shared<Graphic::MixRootSignature>(m_MixtureTextures->size());

		m_rootSignature->Initialize();

		m_PSO->SetRootSigature(m_rootSignature->GetRootSignature());
		m_PSO->Initialize();

		// Create Mesh
		std::vector<DefaultVertex> triangleVertices =
		{
			{ { 1.0f,  1.0f, 0.0f }, { 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f  } },
			{ { 1.0f, -1.0f, 0.0f }, { 1.0f, 1.0f, 1.0f }, { 1.0f, 0.0f } },
			{ { -1.0f, 1.0f, 0.0f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, 1.0f } },
			{ { -1.0f, -1.0f, 0.0f }, { 1.0f, 1.0f, 1.0f}, { 0.0f, 0.0f } }
		};
		std::vector<UINT> index_list = { 0, 1, 2, 3, 2, 1 };
		ptrMesh screenMesh = std::make_shared<TriangleMesh>(triangleVertices, index_list);

		m_RenderScreen = new GObject();
		m_RenderScreen->SetMaterial(m_MixtureTextures);
		m_RenderScreen->SetMesh(screenMesh);
	}

	void MixtureRenderPass::Render(Graphic::CommandList& commandList) 
	{
		commandList.SetViewPorts(&m_Viewport);
		commandList.SetScissorRects(&m_ScissorRect);
		commandList.SetPipelineState(m_PSO);
		commandList.SetGraphicsRootSignature(m_rootSignature);
		commandList.SetDescriptorHeap(*Engine::GetInUseHeap());
		m_RenderScreen->RecordCommand(commandList);
		m_RenderScreen->Draw(commandList);
	}
}