#pragma once
#include "RenderPass.h"

// TODO why render engine in game namespace...?
namespace Game {

	// The main renderer use(called) by the engine
	// TODO hard code passes for now, make it configable
	// Don't think a virtual class or any override is needed
	class RenderEngine {

	friend Graphic::GraphicCore;

	public:
		struct Config 
		{
			bool mixpass = false;
		};

		RenderEngine(const UINT width, const UINT height);

		void Initialize(const HWND appHwnd);

		void Render(Scene& scene);

		Config& GetRenderSetting() { return Setting; }

		DefaultRenderPass& GetDefaultPass() { return defalutpass; }

	protected:
		Config Setting;

		DefaultRenderPass defalutpass;
		MixtureRenderPass mixPass;

	private:

		void BeginRender();
		void EndRender();

		// Main Swap Chain for the Engine
		Graphic::SwapChain swapChain;
		Graphic::DepthBuffer depthBuffer;

	};
}

namespace Engine {
	// TODO global render engine


}