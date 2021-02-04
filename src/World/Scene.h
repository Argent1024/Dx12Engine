#pragma once

#include "Graphic/GraphicCore.h"
#include "Graphic/CommandList.h"
#include "Graphic/CommandQueue.h"
#include "GObject.h"
#include "Camera.h"
#include "Light.h"


namespace Game
{

	struct SceneLightsInfo
	{
		// TODO check memory?
		static const UINT maxLights = 16;

		LightState Lights[maxLights];

		// Store num of each light
		UINT maxDir;
		UINT maxPoint;
		UINT maxSpot;
	};

	class Scene
	{
	public:

		Scene(const UINT width, const UINT height);

		// Weird to create camera before scene = =
		Scene(ProjectiveCamera& camera);

		// Create Descriptor table for lights and put srv( for shadow map) into it
		virtual void Initialize();

		// TODO const camera, edit Camera class
		inline ProjectiveCamera& GetMainCamera() { return m_Camera; }
		// inline Graphic::ConstantBuffer* GetMainCameraCBV() { return m_MainCameraCBV; }

		//TODO
		const std::vector<GObject*> GetGameObjects(UINT index) { return m_ObjList; }

		// TODO accel structure
		virtual void AddGameObj(GObject* obj);
		virtual void DeleteGameObj(GObject* obj) {}

		// Light Stuff
		
		inline Graphic::DescriptorTable* GetLightsTable() { return m_LightsTable; }

		virtual void AddLight(Light& light);

		inline void ConfigLight(UINT nDir, UINT nPoint, UINT nSpot)
		{
			
			assert(nDir + nPoint + nSpot <= SceneLightsInfo::maxLights);
			iDir = 0;
			iPoint = nDir;
			iSpot =  nDir + nPoint;

			m_LightInfo.maxDir = iDir + nDir;
			m_LightInfo.maxPoint = iPoint + nPoint;
			m_LightInfo.maxSpot = iSpot + nSpot;
			
		}

		// Store Lights' data into CBV
		// Assume all lights have put there data in the light state
		void PrepareLights();

	protected:
		static const UINT LightTableSize = 4;

		// Main Camera Stuff
		ProjectiveCamera m_Camera;
		Graphic::ConstantBuffer m_MainCameraCBV;

		// Bind Light Stuff
		SceneLightsInfo m_LightInfo;
		Graphic::ConstantBuffer m_LightsCBV;	  // Store normal data of lights

		Graphic::DescriptorTable* m_LightsTable;  // Store Shadow map

		std::vector<int> m_ShadowLightsIndex;   // Render Shadow Map for these lights			
		// std::vector<Light*> m_Lights;			// Store the lights (maybe)

		// Counter of lights
		UINT iDir;
		UINT iPoint;
		UINT iSpot;

		

		// Game Objects
		std::vector<GObject*> m_ObjList;
	};
}
