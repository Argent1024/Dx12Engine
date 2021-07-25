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

		m_SceneCBV.Initialize();
		m_SceneCBV.CreateView(m_SceneDTable, 0);
		
		m_MainCameraCB.Initialize();
		m_MainCameraCB.CreateRootView();
		
	}

	void Scene::AddLight(Light& light) 
	{
		SceneInfo& info = m_SceneCBV.GetData();
		assert(iDir <= info.maxDir && 
			   iPoint <= info.maxPoint &&
			   iSpot <=  info.maxSpot &&
			   "Lights index out of ranged");
		LightState* pos = &(info.Lights[iDir]);
		LightType type = light.Type();
		switch (type)
		{
		case Game::DIRECTION_LIGHT:
			pos = &(info.Lights[iDir]);
			iDir ++;
			break;
		case Game::POINT_LIGHT:
			pos = &(info.Lights[iPoint]);
			iPoint ++;
			break;
		case Game::SPOT_LIGHT:
			pos = &(info.Lights[iSpot]);
			iSpot ++;
			break;
		}
		light.Initialize(pos);
	}

	void Scene::PrepareSceneCBV() 
	{
		m_SceneCBV.UpdateData();

		{	// Update Camera CB
			auto& cameraData = m_MainCameraCB.GetData();
			const Transform& view = m_Camera.GetView();
			const Transform& proj = m_Camera.GetToScreen();
			const Vector3& cameraPos = m_Camera.Position();

			// Need to transpose
			XMStoreFloat4x4(&cameraData.projection, XMMatrixTranspose((XMMATRIX)proj));
			XMStoreFloat4x4(&cameraData.view, XMMatrixTranspose((XMMATRIX)view));
			XMStoreFloat3(&cameraData.CameraPos, cameraPos);

			m_MainCameraCB.UpdateData();
		}
	}

	void Scene::AddGameObj(GObject* obj, RenderType renderType) 
	{
		assert(renderType != RenderTypeSkyBox);
		m_GameObjectTable[renderType].push_back(obj);
	}
}