#include "Scene.h"

namespace Game {

	Scene::Scene(const UINT width, const UINT height)
		:    m_Camera(width, height, 
			  Vector3(-3.0f, -1.0f, -12.f), 
			  Vector3(0.0f, 0.0f, 0.0f),
			  Vector3(0.0f, 1.0f, 0.0f))
	{ }

	Scene::Scene(ProjectiveCamera& camera)
		:  m_Camera(camera)
	{ }

	
	void Scene::Initialize() 
	{
		m_LightsTable = new Graphic::DescriptorTable(LightTableSize);
		m_LightsTable->Initialize(Engine::GetInitHeap());

		UINT cbvSize = CalculateConstantBufferByteSize(sizeof(SceneLightsInfo));
		ptrGPUMem gpumem = Engine::MemoryAllocator.CreateCommittedBuffer(cbvSize);
		// Create CBV at slot 0 of the lightTable
		m_LightsCBV = new Graphic::ConstantBuffer(gpumem, cbvSize);
		m_LightsCBV->Initialize();
		m_LightsCBV->CreateView(*m_LightsTable, 0);
		
		// Other Textures should be bind outside
	}

	void Scene::PrepareLights() 
	{
		
	}

	void Scene::AddGameObj(GObject* obj) 
	{
		m_ObjList.push_back(obj);
	}
}