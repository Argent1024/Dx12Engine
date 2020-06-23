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

		// TODO const camera, edit Camera class
		Camera& GetMainCamera() { return m_Camera; }

		//TODO
		const std::vector<GObject*> GetGameObjects(UINT index) { return m_ObjList; }

		// TODO accel structure
		virtual void AddGameObj(GObject* obj);
		virtual void DeleteGameObj(GObject* obj) {}

	protected:

		void SetCameraTransformation(Camera& camera);

		ProjectiveCamera m_Camera;
		DirectionLight* m_DirectionLight;

		// Bind Stuff
		SceneConstantBuffer m_SceneData;
		Graphic::ConstantBuffer* m_SceneCBV; 
		Graphic::DescriptorTable* m_SceneTable;

		std::vector<GObject*> m_ObjList;
	};

}