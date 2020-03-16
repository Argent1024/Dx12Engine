#include"MeshTest.h"
#include"../Utility/MeshReader.h"

namespace Samples {

	void MeshTest::Init(const HWND m_appHwnd)
	{
		Engine::EnableDebug();
		Engine::CreateDevice();

		CopyHelper.Initialize();
		GraphicsCommandManager.Initialize();

		DescriptorHeap* initHeap = Engine::GetInitHeap();
		initHeap->Initialize();
		DescriptorHeap* useHeap = Engine::GetInUseHeap();
		useHeap->Initialize();

		// Swap Chain
		m_swapChain = new SwapChain(m_appHwnd, m_width, m_height);
		m_swapChain->Initialize(GraphicsCommandManager.GetCommadnQueue());

		m_rootSignature = std::make_shared<RootSignature>();

		m_GraphicPSO = std::make_shared<MeshTestPSO>();
		m_GraphicPSO->SetRootSigature(m_rootSignature->GetRootSignature());
		m_GraphicPSO->Initialize();

		m_Camera.CreateCBV();
		this->LoadAssert();

	}

	void MeshTest::LoadAssert() 
	{
		std::vector<DefaultVertex> vertex;
		std::vector<UINT> index;
		MeshReader::ReadOBJ("D:\\work\\tEngine\\airboat.obj", vertex, index);

		m_Mesh = std::make_shared<TriangleMesh>(vertex, index);
		m_Material = std::make_shared<NoMaterial>(m_GraphicPSO, m_rootSignature);
		m_Bunny.SetMesh(m_Mesh);
		m_Bunny.SetMaterial(m_Material);
	}

	void MeshTest::Render()
	{
		Vector3 Pos(0.0f, -2.0f, 10.f);
		//if (t >= 2.0) {t -= 0.05;}
		Vector3 Target(0.0f, 0.0f, 0.0f);
		Vector3 UP(0.0f, 1.0f, 0.0f);
		m_Camera.LookAt(Pos, Target, UP);
		angle += 3.14f / 300.0f;
		Transform modelT = Transform(Matrix4( XMMatrixRotationY(angle)));
		m_Bunny.SetTransform(modelT);

		GraphicsCommandManager.Start();
		DescriptorHeap* UseHeap = Engine::GetInUseHeap();
		UseHeap->Reset();

		CommandList mainCommandList;
		const float clearColor[] = { 0.0f, 0.2f, 0.4f, 1.0f };
		GraphicsCommandManager.InitCommandList(&mainCommandList);
		mainCommandList.SetSwapChain(*m_swapChain);
		mainCommandList.ResourceBarrier(*m_swapChain, D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
		mainCommandList.ClearSwapChain(*m_swapChain, clearColor);
		mainCommandList.ResourceBarrier(*m_swapChain, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
		GraphicsCommandManager.ExecuteCommandList(&mainCommandList);

		// Record commands for each object, only have one object here
		{
			
			CommandList ThreadCommandList;
			GraphicsCommandManager.InitCommandList(&ThreadCommandList);
			
			m_Bunny.RecordCommand(ThreadCommandList);

			
			m_Camera.UseCamera(ThreadCommandList, m_Bunny.GetTransform());
			

			// Barrier Draw
			ThreadCommandList.ResourceBarrier(*m_swapChain, D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
			
			ThreadCommandList.SetSwapChain(*m_swapChain);
			m_Bunny.Draw(ThreadCommandList);

			ThreadCommandList.ResourceBarrier(*m_swapChain, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);

			GraphicsCommandManager.ExecuteCommandList(&ThreadCommandList);
		}
		
		// Wait Finish
		GraphicsCommandManager.End();
		m_swapChain->Present();

	}
}