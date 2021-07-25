#pragma once

#include "Graphic/GraphicCore.h"
#include "Graphic/CommandList.h"
#include "Graphic/CommandQueue.h"
#include "GObject.h"
#include "Camera.h"
#include "Light.h"
#include "EnvironmentMapping.h"

// TODO = = how to make a good key
using RenderType = UINT;
constexpr RenderType RenderTypeSkyBox=-1;

namespace Game
{
	struct SceneInfo
	{
		static const UINT maxLights = 16;
		
		BOOL UseEnvMapping = FALSE;
		// Store num of each light
		UINT maxDir;
		UINT maxPoint;
		UINT maxSpot;

		LightState Lights[maxLights];	
	};


	struct CameraData 
	{
		DirectX::XMFLOAT4X4 projection;
		DirectX::XMFLOAT4X4 view;
		DirectX::XMFLOAT3 CameraPos;
		BOOL padding[1];
	};


	class Scene
	{
	using GameObjectTable = std::map<RenderType, std::vector<GObject*>>;

	public:
		Scene(const UINT width, const UINT height);

		// Weird to create camera before scene = =
		Scene(ProjectiveCamera& camera);

		// Create Descriptor table for lights and put srv( for shadow map) into it
		virtual void Initialize();

		const ProjectiveCamera& GetMainCamera() const { return m_Camera; }
		ProjectiveCamera& GetMainCamera() { return m_Camera; }
		
		void SetSkyBox(std::shared_ptr<SkyBox> box) { m_SkyBox = box; }
		std::shared_ptr<SkyBox> GetSkyBox() const { return m_SkyBox; }

		void SetEnvMapping(UINT slot, ptrEnvMap env) 
		{
			assert(0 < slot && slot < 4 && "Slot of Env map out of range");
			env->CreateSRV(m_SceneDTable, slot);
		}

		const std::vector<GObject*>& GetGameObjects(RenderType renderType) const {
			const auto it = m_GameObjectTable.find(renderType);
			if (it == m_GameObjectTable.end()) {
				Logger::Log("Querying render type not exist");
				return {};
			}
			return it->second;
		}

		// TODO accel structure
		virtual void AddGameObj(GObject* obj, RenderType renderType=0);
		virtual void DeleteGameObj(GObject* obj) {}

		const Graphic::ConstantBuffer<CameraData>& GetCameraCB() const { return m_MainCameraCB; }

		const Graphic::DescriptorTable* GetSceneTable() const { return m_SceneDTable; }
		Graphic::DescriptorTable* GetSceneTable()  { return m_SceneDTable; }

		// Lights Stuff
		virtual void AddLight(Light& light);
		inline void ConfigLight(UINT nDir, UINT nPoint, UINT nSpot)
		{
			
			assert(nDir + nPoint + nSpot <= SceneInfo::maxLights);
			iDir = 0;
			iPoint = nDir;
			iSpot =  nDir + nPoint;
			SceneInfo& info = m_SceneCBV.GetData();
			info.maxDir = iDir + nDir;
			info.maxPoint = iPoint + nPoint;
			info.maxSpot = iSpot + nSpot;
			
		}

		// Store Lights' data into CBV
		// Assume all lights have put there data in the light state
		void PrepareSceneCBV();

	protected:
		static const UINT SceneTableSize = 16;

		// Main Camera Stuff
		ProjectiveCamera m_Camera;
		Graphic::ConstantBuffer<CameraData> m_MainCameraCB;


		Graphic::DescriptorTable* m_SceneDTable;  // Store textures unique to scene(E.X. light's shadow map, env mapping)

		Graphic::ConstantBuffer<SceneInfo> m_SceneCBV;	  // Store normal data of lights
		

		// TODO
		// Lights stuff below
		// std::vector<int> m_ShadowLightsIndex;   // Render Shadow Map for these lights			
		// std::vector<Light*> m_Lights;			// Store the lights (maybe)

		// Counter of lights
		UINT iDir;
		UINT iPoint;
		UINT iSpot;

		// Game Objects
		GameObjectTable m_GameObjectTable;
		std::shared_ptr<SkyBox> m_SkyBox;
	};
}
