#include "Scene.h"

namespace Game {
	
	Scene::Scene(const HWND AppHwnd, const UINT width, const UINT height)
		: m_SwapChain(AppHwnd, width, height), m_depthBuffer(width, height),
		  m_Camera(width, height, Vector3(-1.0f, -1.0f, 10.0f), Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f, 1.0f, 0.0f))
	{

		m_depthBuffer.Initialize(Graphic::TEXTURE_DSV | Graphic::TEXTURE_SRV);
		m_SwapChain.Initialize(GraphicsCommandManager.GetCommadnQueue());
		m_Renderpass.Initialize();
		// TODO fix camera
		m_Camera.CreateCBV();
	}

	Scene::Scene(const HWND m_appHwnd, const UINT width, const UINT height, ProjectiveCamera& camera)
		: m_SwapChain(m_appHwnd, width, height), m_depthBuffer(width, height), m_Camera(camera) {}

	void Scene::Render()
	{
		BeginRender();

		// TODO multi threading here
		Graphic::CommandList ThreadCommandList;
		GraphicsCommandManager.InitCommandList(&ThreadCommandList);

		
		// Main Render Pass
		ThreadCommandList.SetSwapChain(m_SwapChain, m_depthBuffer);
		m_Renderpass.SetCamera(&m_Camera);
		ThreadCommandList.ResourceBarrier(m_SwapChain, D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
		m_Renderpass.Render(ThreadCommandList, m_ObjList);
		ThreadCommandList.ResourceBarrier(m_SwapChain, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
		/*// ThreadCommandList.SetSwapChain(m_SwapChain);
		for (auto const& g_obj : m_ObjList)
		{
			g_obj->RecordCommand(ThreadCommandList);

			m_Camera.UseCamera(ThreadCommandList, g_obj->GetTransform());

			// Barrier here since we are going to modify SwapChain
			ThreadCommandList.ResourceBarrier(m_SwapChain, D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
			g_obj->Draw(ThreadCommandList);
			ThreadCommandList.ResourceBarrier(m_SwapChain, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
		}*/
		GraphicsCommandManager.ExecuteCommandList(&ThreadCommandList);
		

		// Mix Render Pass
		Graphic::CommandList MixCommandList;
		GraphicsCommandManager.InitCommandList(&MixCommandList);



		// Multithreading join here

		EndRender();
	}

	void Scene::BeginRender() 
	{
		GraphicsCommandManager.Start();
		Graphic::DescriptorHeap* UseHeap = Engine::GetInUseHeap();
		UseHeap->Reset();

		Graphic::CommandList mainCommandList;
		const float clearColor[] = { 0.0f, 0.2f, 0.4f, 1.0f };
		GraphicsCommandManager.InitCommandList(&mainCommandList);

		// Record main command list
		mainCommandList.SetSwapChain(m_SwapChain, m_depthBuffer);
		mainCommandList.ClearDepthBuffer(m_depthBuffer);
		mainCommandList.ResourceBarrier(m_SwapChain, D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
		mainCommandList.ClearSwapChain(m_SwapChain, clearColor);
		mainCommandList.ResourceBarrier(m_SwapChain, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
		
		GraphicsCommandManager.ExecuteCommandList(&mainCommandList);
	}

	void Scene::EndRender()
	{
		GraphicsCommandManager.End();
		m_SwapChain.Present();
	}

	void Scene::Update() 
	{
		// TODO DO NOTHING FOR NOW

	}

	void Scene::AddGameObj(GObject* obj) 
	{
		m_ObjList.push_back(obj);
	}
}