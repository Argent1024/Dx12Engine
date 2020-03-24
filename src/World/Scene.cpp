#include "Scene.h"

namespace Game {
	
	Scene::Scene(const HWND AppHwnd, const UINT width, const UINT height)
		: m_SwapChain(AppHwnd, width, height), m_Camera(width, height)
	{
		m_SwapChain.Initialize(GraphicsCommandManager.GetCommadnQueue());
		m_Camera.CreateCBV();
	}

	void Scene::Render()
	{
		// TODO multi threading here
		Graphic::CommandList ThreadCommandList;
		GraphicsCommandManager.InitCommandList(&ThreadCommandList);
		ThreadCommandList.SetSwapChain(m_SwapChain);

		for (auto const& g_obj : m_ObjList)
		{
			g_obj->RecordCommand(ThreadCommandList);

			m_Camera.UseCamera(ThreadCommandList, g_obj->GetTransform());
			ThreadCommandList.ResourceBarrier(m_SwapChain, D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
			g_obj->Draw(ThreadCommandList);
			ThreadCommandList.ResourceBarrier(m_SwapChain, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
		}

		GraphicsCommandManager.ExecuteCommandList(&ThreadCommandList);
	}

	void Scene::BeginRender() 
	{
		Graphic::CommandList mainCommandList;
		const float clearColor[] = { 0.0f, 0.2f, 0.4f, 1.0f };
		GraphicsCommandManager.InitCommandList(&mainCommandList);

		mainCommandList.SetSwapChain(m_SwapChain);
		mainCommandList.ResourceBarrier(m_SwapChain, D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
		mainCommandList.ClearSwapChain(m_SwapChain, clearColor);
		mainCommandList.ResourceBarrier(m_SwapChain, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
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
}