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
	struct SceneConstantBuffer 
	{
		XMFLOAT4X4 projection;
		XMFLOAT4X4 view;
	};

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
		
		void SetCameraTransformation(Camera& camera);

		DefaultRenderPass m_Renderpass;
		MixtureRenderPass m_MixturePass;

		ProjectiveCamera m_Camera;
		DirectionLight* m_DirectionLight;

		// Bind Stuff
		SceneConstantBuffer m_SceneData;
		Graphic::ConstantBuffer* m_SceneCBV; 
		Graphic::DescriptorTable* m_SceneTable;

		std::vector<GObject*> m_ObjList;

		Graphic::DepthBuffer m_depthBuffer;
		Graphic::SwapChain m_SwapChain;
	};

}