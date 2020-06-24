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

		Scene(const HWND m_appHwnd, const UINT width, const UINT height, ProjectiveCamera& camera);

		// Create Descriptor table for lights and put srv( for shadow map) into it
		virtual void Initialize();

		// TODO const camera, edit Camera class
		Camera& GetMainCamera() { return m_Camera; }

		//TODO
		const std::vector<GObject*> GetGameObjects(UINT index) { return m_ObjList; }

		// TODO accel structure
		virtual void AddGameObj(GObject* obj);
		virtual void DeleteGameObj(GObject* obj) {}

	protected:
		static const UINT LightTableSize = 4;

		ProjectiveCamera m_Camera;
		DirectionLight* m_DirectionLight;

		// Bind Light Stuff
		Graphic::ConstantBuffer* m_LightCBV; 
		Graphic::DescriptorTable* m_LightTable;

		std::vector<GObject*> m_ObjList;
	};

}