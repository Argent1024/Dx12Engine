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

			// Barrier here since we are going to modify SwapChain
			//commandList.ResourceBarrier(m_SwapChain, D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
			g_obj->Draw(commandList);
			// commandList.ResourceBarrier(m_SwapChain, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
		}
		
	}

	MixtureRenderPass::MixtureRenderPass(std::shared_ptr<SimpleMaterial> textureMaterial)
	{
		m_MixtureTextures = textureMaterial; 
		m_PSO = std::make_shared<Graphic::MixturePSO>();
		m_rootSignature = std::make_shared<Graphic::MixRootSignature>(textureMaterial->size());
	}

	void MixtureRenderPass::Initialize() 
	{
		m_rootSignature->Initialize();

		m_PSO->SetRootSigature(m_rootSignature->GetRootSignature());
		m_PSO->Initialize();

		// Create Mesh
		std::vector<DefaultVertex> triangleVertices =
		{
			{ { 1.0f,  1.0f, 0.0f }, { 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f  } },
			{ { 1.0f, -1.0f, 0.0f }, { 1.0f, 1.0f, 1.0f }, { 1.0f, -1.0f } },
			{ { -1.0f, 1.0f, 0.0f }, { 1.0f, 1.0f, 1.0f }, { -1.0f, 1.0f } },
			{ { -1.0f, -1.0f, 0.0f }, { 1.0f, 1.0f, 1.0f}, { -1.0f, -1.0f } }
		};
		std::vector<UINT> index_list = { 0, 1, 2, 3, 2, 1 };
		ptrMesh screenMesh = std::make_shared<TriangleMesh>(triangleVertices, index_list);

		m_RenderScreen = new GObject();
		m_RenderScreen->SetMaterial(m_MixtureTextures);
		m_RenderScreen->SetMesh(screenMesh);
	}

	void MixtureRenderPass::Render(Graphic::CommandList& commandList) 
	{
		commandList.SetPipelineState(m_PSO);
		commandList.SetGraphicsRootSignature(m_rootSignature);

		m_RenderScreen->Draw(commandList);
	}
}