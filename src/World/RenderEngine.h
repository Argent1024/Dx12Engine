#pragma once
#include "RenderPass.h"

// TODO why render engine in game namespace...?
namespace Engine {

	// The main renderer use(called) by the engine
	// TODO hard code passes for now, make it configable
	// Don't think a virtual class or any override is needed
	class RenderEngine {

	public:
		// TODO move all the setting in the render pass to here
		struct RenderConfiguration 
		{
			bool mixpass = false;
		};

		RenderEngine(const UINT width, const UINT height);

		void Initialize(const HWND appHwnd);

		void Render(Game::Scene& scene);

		RenderConfiguration& GetRenderSetting() { return Setting; }

		Game::DefaultRenderPass& GetDefaultPass() { return defalutpass; }

	protected:
		RenderConfiguration Setting;

		// Different pass
		Game::DefaultRenderPass defalutpass;
		Game::MixtureRenderPass mixPass;

	private:

		void BeginRender();
		void EndRender();

		// Main Swap Chain for the Engine
		Graphic::SwapChain swapChain;
		Graphic::DepthBuffer depthBuffer;

	};
}