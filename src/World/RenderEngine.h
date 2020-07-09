#pragma once
#include "RenderPass.h"


namespace Game {
	// The main renderer use(called) by the engine
	// TODO hard code passes for now, make it configable
	// Don't think a virtual class or any override is needed
	class RenderEngine {

	public:
		RenderEngine(const UINT width, const UINT height);

		void Initialize(const HWND appHwnd);

		void Render(Scene& scene);

	private:

		void BeginRender();
		void EndRender();
	
		DefaultRenderPass defalutpass;
		MixtureRenderPass mixPass;

		// Main Swap Chain for the Engine
		Graphic::SwapChain swapChain;
		Graphic::DepthBuffer depthBuffer;

	};
}