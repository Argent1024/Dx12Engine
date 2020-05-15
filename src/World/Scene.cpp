#include "Scene.h"

namespace Game {
	
	Scene::Scene(const HWND AppHwnd, const UINT width, const UINT height)
		: m_SwapChain(AppHwnd, width, height), m_depthBuffer(width, height),
		  m_Camera(width, height, Vector3(0.0f, 1.0f, -5.0f), Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f, 1.0f, 0.0f)),
		  m_MixturePass(1, width, height) // one Texture to mix in the mixture stage
	{

		m_SwapChain.Initialize(GraphicsCommandManager.GetCommadnQueue());
		m_Renderpass.Initialize();
		m_MixturePass.Initialize();

		// Create depth buffer & srv
		m_depthBuffer.Initialize(Graphic::TEXTURE_DSV | Graphic::TEXTURE_SRV);
		Graphic::Texture* depthTexture = m_depthBuffer.GetTexture();
		Graphic::DescriptorTable* table = m_MixturePass.GetDescriptorTable();
		depthTexture->CreateView(Graphic::TEXTURE_SRV, table, 0);

		// Init camera
		m_Camera.Initialize();
	}

	Scene::Scene(const HWND m_appHwnd, const UINT width, const UINT height, ProjectiveCamera& camera)
		: m_SwapChain(m_appHwnd, width, height), m_depthBuffer(width, height), m_Camera(camera), 
		  m_MixturePass(1, width, height) // one Texture to mix in the mixture stage
	{}

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
		GraphicsCommandManager.ExecuteCommandList(&ThreadCommandList);
		
		// Mix Render Pass
		
		Graphic::CommandList MixCommandList;
		GraphicsCommandManager.InitCommandList(&MixCommandList);
		MixCommandList.SetSwapChain(m_SwapChain);
		const float ClearColor[] = {0.0, 0.0, 0.0, 0.0};
		
		MixCommandList.ResourceBarrier(m_SwapChain, D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
		//MixCommandList.ClearSwapChain(m_SwapChain, ClearColor);
		m_MixturePass.Render(MixCommandList);
		MixCommandList.ResourceBarrier(m_SwapChain, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
		GraphicsCommandManager.ExecuteCommandList(&MixCommandList);
		
		
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