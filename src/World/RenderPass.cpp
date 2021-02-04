#include "RenderPass.h"
#include "../Utility/MeshReader.h"

namespace Game {
	using namespace Math;

	void DefaultRenderPass::Initialize() 
	{
		// TODO Use only one root signature
		m_rootSignature = std::make_shared<Graphic::RootSignature>();
		m_rootSignature->Initialize();

		m_PSO = std::make_shared<Graphic::DefaultPSO>();
		m_PSO->SetRootSigature(m_rootSignature->GetRootSignature());
		m_PSO->Initialize();

		
		// Create Descriptor Table
		// TODO determine how large the table should be
		const UINT tableSize = 4;
		m_DescriptorTable = new Graphic::DescriptorTable(tableSize);
	

		// Creata CBV 
		
		const UINT cbvSize = CalculateConstantBufferByteSize(sizeof(DefaultRenderPass::ConstBufferData));
		
		ptrGBuffer buffer = GPU::MemoryManager::CreateGBuffer();
		buffer->Initialize(cbvSize);	// TODO need upload? or default is fine?

		m_CBV.Initialze(buffer, cbvSize);
		m_CBV.CreateView(m_DescriptorTable, 0);
		

		// The other textures needed is binded outside by render engine
	}

	void DefaultRenderPass::PrepareData(Scene& scene)
	{
		Camera& camera = scene.GetMainCamera();
		const Transform& view = camera.GetView();
		const Transform& proj = camera.GetToScreen();
		Vector3& cameraPos = camera.Position();

		// Need to transpose
		XMStoreFloat4x4(&m_CBVData.projection, XMMatrixTranspose((XMMATRIX)proj));
		XMStoreFloat4x4(&m_CBVData.view, XMMatrixTranspose((XMMATRIX)view));

		XMStoreFloat3(&m_CBVData.CameraPos, cameraPos);
		
		// Copy to CBV
		m_CBV.copyData(&m_CBVData);//, sizeof(CameraBufferData));
	}

	void DefaultRenderPass::Render(Graphic::CommandList& commandList, Scene& scene) {
		
		commandList.SetDescriptorHeap(*Engine::GetInUseHeap());
		commandList.SetPipelineState(m_PSO);
		commandList.SetGraphicsRootSignature(m_rootSignature);
		

		CD3DX12_GPU_DESCRIPTOR_HANDLE cameraTableHandle = m_DescriptorTable->BindDescriptorTable();
		// Slot 0 in root signature is for camera
		commandList.SetGraphicsRootDescriptorTable(0, cameraTableHandle);
		
		Graphic::DescriptorTable* lightTable = scene.GetLightsTable();
		CD3DX12_GPU_DESCRIPTOR_HANDLE lightTableHandle = lightTable->BindDescriptorTable();
		commandList.SetGraphicsRootDescriptorTable(1, lightTableHandle);

		Camera& camera = scene.GetMainCamera();
		camera.UseCamera(commandList);

		const std::vector<GObject*> objList = scene.GetGameObjects(m_ObjRenderType);
		for (auto const& g_obj : objList)
		{
			g_obj->RecordCommand(commandList);
			g_obj->Draw(commandList);
		}
		
	}

	MixtureRenderPass::MixtureRenderPass(UINT num_texture, const UINT width, const UINT height)
		: m_Viewport(0.0f, 0.0f, static_cast<float>(width), static_cast<float>(height)),
		  m_ScissorRect(0, 0, static_cast<LONG>(width), static_cast<LONG>(height))
		 { m_DescriptorTable = new Graphic::DescriptorTable(num_texture); }

	void MixtureRenderPass::Initialize() 
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

	void MixtureRenderPass::Render(Graphic::CommandList& commandList, Scene& scene) 
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