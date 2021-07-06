#include "stdafx.h"
#include "RenderEngine.h"
#include "FFTOcean.h"


namespace Engine {
	
	RenderEngine::RenderEngine(const UINT width, const UINT height) 
		: swapChain(width, height),
		  depthBuffer(width, height, Graphic::TEXTURE_DSV | Graphic::TEXTURE_SRV)
	{
		m_UiEngine = std::make_shared<SimpleUIEngine>();
	}


	void RenderEngine::Initialize(const HWND appHwnd, RenderPassesTable& passes) 
	{	
		if(m_UiEngine) m_UiEngine->Initialize(appHwnd);

		swapChain.Initialize(GraphicsCommandManager.GetCommadnQueue(), appHwnd);
		depthBuffer.Initialize();

		for (const auto& pass_i : passes) {
			ptrRenderPass pass = pass_i.first;
			ptrPSO pso = pass_i.second;
			m_RenderPasses.push_back(pass);
			m_EnabledPasses.push_back(TRUE);

			pass->Initialize(pso);
		}

		PostInitialize();
	}


	void RenderEngine::Render(Game::Scene& scene) 
	{
		BeginRender();
		scene.PrepareSceneCBV();
	
		// Multi Threading maybe
		Graphic::CommandList commandlist;
		GraphicsCommandManager.InitCommandList(&commandlist);

		// Scene
		for (ptrRenderPass& pass : m_RenderPasses) {
			pass->PrepareData(scene);
			commandlist.SetSwapChain(swapChain, depthBuffer);
			commandlist.ResourceBarrier(swapChain, D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);

			pass->Render(commandlist, scene);
			
			commandlist.ResourceBarrier(swapChain, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
		}
		// Multithreading join here
		GraphicsCommandManager.ExecuteCommandList(&commandlist);

		// UI
		if(m_UiEngine)
		{
			Graphic::DescriptorHeap* uiheap = Engine::GetUIHeap();
			Graphic::CommandList uiCtx;
			GraphicsCommandManager.InitCommandList(&uiCtx);
			uiCtx.SetSwapChain(swapChain);
			uiCtx.SetDescriptorHeap(*uiheap);
			uiCtx.ResourceBarrier(swapChain, D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
			m_UiEngine->Render(uiCtx);
			uiCtx.ResourceBarrier(swapChain, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
			GraphicsCommandManager.ExecuteCommandList(&uiCtx);
		}

		EndRender();
	}
	
	void RenderEngine::BeginRender() 
	{
		GraphicsCommandManager.Start();
		Graphic::DescriptorHeap* UseHeap = Engine::GetInUseHeap();
		UseHeap->Reset();

		Graphic::CommandList mainCommandList;
		const float clearColor[] = { 0.0f, 0.2f, 0.4f, 1.0f };
		GraphicsCommandManager.InitCommandList(&mainCommandList);

		// Record main command list
		mainCommandList.SetSwapChain(swapChain, depthBuffer);
		mainCommandList.ClearDepthBuffer(depthBuffer);
		mainCommandList.ResourceBarrier(swapChain, D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
		mainCommandList.ClearSwapChain(swapChain, clearColor);
		mainCommandList.ResourceBarrier(swapChain, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
		// Main Commanlist will go before other commandlist
		GraphicsCommandManager.ExecuteCommandList(&mainCommandList);
	}

	void RenderEngine::EndRender()
	{
		GraphicsCommandManager.End();
		swapChain.Present();
	}




}