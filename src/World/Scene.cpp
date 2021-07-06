#include "stdafx.h"
#include "Scene.h"

namespace Game {

	using namespace Math;

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
		m_SceneDTable = new Graphic::DescriptorTable(SceneTableSize);

		UINT cbvSize = CalculateConstantBufferByteSize(sizeof(SceneInfo));

		ptrGBuffer buffer = GPU::MemoryManager::CreateGBuffer();
		buffer->Initialize(cbvSize);

		// Create CBV at slot 0 of the lightTable
		m_SceneCBV.Initialze(buffer, cbvSize);
		m_SceneCBV.CreateView(m_SceneDTable, 0);
	}

	void Scene::SetEnvironmentMapping(ptrTexture envmap) 
	{
		m_SceneInfo.UseEnvMapping = TRUE;
		m_EnvMapping->CreateView(Graphic::TextureType::TEXTURE_SRV, m_SceneDTable, 1);
	}

	void Scene::AddLight(Light& light) 
	{
		assert(iDir <= m_SceneInfo.maxDir && 
			   iPoint <= m_SceneInfo.maxPoint &&
			   iSpot <=  m_SceneInfo.maxSpot &&
			   "Lights index out of ranged");
		LightState* pos = &(m_SceneInfo.Lights[iDir]);
		LightType type = light.Type();
		switch (type)
		{
		case Game::DIRECTION_LIGHT:
			pos = &(m_SceneInfo.Lights[iDir]);
			iDir ++;
			break;
		case Game::POINT_LIGHT:
			pos = &(m_SceneInfo.Lights[iPoint]);
			iPoint ++;
			break;
		case Game::SPOT_LIGHT:
			pos = &(m_SceneInfo.Lights[iSpot]);
			iSpot ++;
			break;
		}
		light.Initialize(pos);
	}

	void Scene::PrepareSceneCBV() 
	{
		// UINT size = sizeof(SceneLightsInfo);
		//TODO ERROR when only copying with size
		m_SceneCBV.copyData(&m_SceneInfo);// , sizeof(SceneLightsInfo));
	}

	void Scene::AddGameObj(GObject* obj, UINT renderType) 
	{
		m_GameObjectTable[renderType].push_back(obj);
	}
}