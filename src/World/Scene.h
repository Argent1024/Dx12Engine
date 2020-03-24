#pragma once

#include "GraphicCore.h"
#include "CommandList.h"
#include "CommandQueue.h"
#include "GObject.h"
#include "Camera.h"
#include "Light.h"

namespace Game 
{

	class Scene
	{
	public:
		Scene(const HWND m_appHwnd, const UINT width, const UINT height);

		virtual void Render();

		virtual void Update();

	protected:

		void BeginRender();
		void EndRender();

		// TODO need change
		ProjectiveCamera m_Camera;
		std::vector<ptrLight> m_LightList;
		std::vector<ptrGobject> m_ObjList;

		Graphic::SwapChain m_SwapChain;
	};

}