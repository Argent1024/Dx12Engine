#pragma once
#include "RenderPass.h"


namespace Game {
	// The main renderer use(called) by the engine
	// TODO hard code passes for now, make it configable
	class RenderEngine {

	public:
		virtual void Initialize();

		virtual void Render(Scene& scene);

	private:

		void BeginRender();
		void EndRender();
		

		Graphic::ConstantBuffer* sceneData;

		DefaultRenderPass defalutpass;


		// Main Swap Chain for the Engine
		Graphic::SwapChain swapChain;
		Graphic::DepthBuffer depthBuffer;

	};
}