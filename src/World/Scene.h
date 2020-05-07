#pragma once

#include "GraphicCore.h"
#include "CommandList.h"
#include "CommandQueue.h"
#include "GObject.h"
#include "Camera.h"
#include "Light.h"
#include "RenderPass.h"

namespace Game 
{

	class Scene
	{
	public:
		Scene(const HWND m_appHwnd, const UINT width, const UINT height);

		Scene(const HWND m_appHwnd, const UINT width, const UINT height, ProjectiveCamera& camera);

		virtual void Render();

		virtual void Update();

		// TODO accel structure
		virtual void AddGameObj(GObject* obj);

		// TODO
		virtual void DeleteGameObj(GObject* obj) {}

	protected:

		virtual void BeginRender();
		virtual void EndRender();
		
		DefaultRenderPass m_Renderpass;

		// TODO need change
		ProjectiveCamera m_Camera;

		std::vector<ptrLight> m_LightList;
		std::vector<GObject*> m_ObjList;

		Graphic::DepthBuffer m_depthBuffer;
		Graphic::SwapChain m_SwapChain;
	};

}