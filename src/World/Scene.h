#pragma once

#include "GraphicCore.h"
#include "CommandList.h"
#include "CommandQueue.h"
#include "GObject.h"
#include "Camera.h"
#include "Light.h"


namespace Game
{

	struct SceneLightsInfo
	{
		// TODO check memory?
		static const UINT maxLights = 16;

		// Store num of each light
		UINT numDir;
		UINT numPoint;
		UINT numSpot;

		LightState Lights[maxLights];
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
		inline Camera& GetMainCamera() { return m_Camera; }
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
			m_LightInfo.numDir = nDir;
			m_LightInfo.numPoint = nPoint;
			m_LightInfo.numSpot = nSpot;
			iDir = 0;
			iPoint = nDir;
			iSpot =  nDir + nPoint;
		}

		// Store Lights' data into CBV
		// Assume all lights have put there data in the light state
		void PrepareLights();

	protected:
		static const UINT LightTableSize = 4;

		// Main Camera Stuff
		ProjectiveCamera m_Camera;
		// Graphic::ConstantBuffer* m_MainCameraCBV;

		// Bind Light Stuff
		Graphic::ConstantBuffer* m_LightsCBV;
		Graphic::DescriptorTable* m_LightsTable;  // Store Shadow map

		SceneLightsInfo m_LightInfo;

		UINT iDir;
		UINT iPoint;
		UINT iSpot;

		std::vector<GObject*> m_ObjList;
	};
}
