#include "Scene.h"

namespace Game {

	Scene::Scene(const HWND AppHwnd, const UINT width, const UINT height)
		:
		  m_Camera(width, height, 
			  Vector3(-3.0f, -1.0f, -12.f), 
			  Vector3(0.0f, 0.0f, 0.0f),
			  Vector3(0.0f, 1.0f, 0.0f))
	{ }

	Scene::Scene(const HWND m_appHwnd, const UINT width, const UINT height, ProjectiveCamera& camera)
		:  m_Camera(camera)
	{ }

	
	void Scene::Initialize() 
	{
		// m_Camera.
		m_LightTable = new Graphic::DescriptorTable(LightTableSize);
		m_LightTable->Initialize(Engine::GetInitHeap());
	}

	
	void Scene::AddGameObj(GObject* obj) 
	{
		m_ObjList.push_back(obj);
	}
}