#include "Scene.h"

namespace Game {

	Scene::Scene(const UINT width, const UINT height)
		:    m_Camera(width, height, 
			  Vector3(0.0f, 0.0f, 10.f), 
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

	void Scene::AddLight(Light& light) 
	{
		assert(iDir <= m_LightInfo.maxDir && 
			   iPoint <= m_LightInfo.maxPoint &&
			   iSpot <=  m_LightInfo.maxSpot &&
			   "Lights index out of ranged");
		LightState* pos = &(m_LightInfo.Lights[iDir]);
		LightType type = light.Type();
		switch (type)
		{
		case Game::DIRECTION_LIGHT:
			pos = &(m_LightInfo.Lights[iDir]);
			iDir ++;
			break;
		case Game::POINT_LIGHT:
			pos = &(m_LightInfo.Lights[iPoint]);
			iPoint ++;
			break;
		case Game::SPOT_LIGHT:
			pos = &(m_LightInfo.Lights[iSpot]);
			iSpot ++;
			break;
		}
		light.Initialize(pos);
	}

	void Scene::PrepareLights() 
	{
		// UINT size = sizeof(SceneLightsInfo);
		//TODO ERROR when only copying with size
		m_LightsCBV->copyData(&m_LightInfo);// , sizeof(SceneLightsInfo));
	}

	void Scene::AddGameObj(GObject* obj) 
	{
		m_ObjList.push_back(obj);
	}
}