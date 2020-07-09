#include "RenderEngine.h"

namespace Game {
	
	RenderEngine::RenderEngine(const UINT width, const UINT height) 
		: swapChain(width, height),
		  depthBuffer(width, height, Graphic::TEXTURE_DSV | Graphic::TEXTURE_SRV),
		  mixPass(4, width, height)
	{
	
	}


	void RenderEngine::Initialize(const HWND appHwnd) 
	{
		defalutpass.Initialize();

		swapChain.Initialize(GraphicsCommandManager.GetCommadnQueue(), appHwnd);
		depthBuffer.Initialize();
		
		// If mix pass
		mixPass.Initialize();
		Graphic::DescriptorTable* mixTable = mixPass.GetTable();
		Graphic::Texture& depthTex = depthBuffer.GetTexture();
		// TODO avoid magic number
		depthTex.CreateView(Graphic::TEXTURE_SRV, mixTable, 2);
	}


	void RenderEngine::Render(Scene& scene) 
	{
		BeginRender();

		scene.PrepareLights();

		// Set Scene Data for Default Render Pass
		defalutpass.PrepareData(scene);

		// TODO multi threading here

		Graphic::CommandList ThreadCommandList;
		GraphicsCommandManager.InitCommandList(&ThreadCommandList);
		
		// Main Render Pass
		ThreadCommandList.SetSwapChain(swapChain, depthBuffer);
		ThreadCommandList.ResourceBarrier(swapChain, D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
		defalutpass.Render(ThreadCommandList, scene);
		ThreadCommandList.ResourceBarrier(swapChain, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
		
		GraphicsCommandManager.ExecuteCommandList(&ThreadCommandList);
		
		// Mix Render Pass
		
		Graphic::CommandList MixCommandList;
		GraphicsCommandManager.InitCommandList(&MixCommandList);
		MixCommandList.SetSwapChain(swapChain);
		
		MixCommandList.ResourceBarrier(swapChain, D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
		const float ClearColor[] = {0.0, 0.0, 0.0, 0.0};
		MixCommandList.ClearSwapChain(swapChain, ClearColor);
		mixPass.Render(MixCommandList, scene);
		MixCommandList.ResourceBarrier(swapChain, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
		GraphicsCommandManager.ExecuteCommandList(&MixCommandList);
		
		
		// Multithreading join here

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