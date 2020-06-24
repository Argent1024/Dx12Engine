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

		Graphic::DescriptorTable* GetLightsTable() { return m_LightsTable; }

		// TODO const camera, edit Camera class
		Camera& GetMainCamera() { return m_Camera; }

		//TODO
		const std::vector<GObject*> GetGameObjects(UINT index) { return m_ObjList; }

		// TODO accel structure
		virtual void AddGameObj(GObject* obj);
		virtual void DeleteGameObj(GObject* obj) {}

		// TODO kind of stupid to creat light outside with this ptr
		inline LightState* GetLightState(UINT index)
		{
			assert(index < SceneLightsInfo::maxLights);
			return &m_LightInfo.Lights[index];
		}


		inline void ConfigLight(UINT nDir, UINT nPoint, UINT nSpot)
		{
			assert(nDir + nPoint + nSpot <= SceneLightsInfo::maxLights);
			m_LightInfo.numDir = nDir;
			m_LightInfo.numPoint = nPoint;
			m_LightInfo.numSpot = nSpot;
		}

		// Store Lights' data into CBV
		// Assume all lights have put there data in the light state
		void PrepareLights();

	protected:
		static const UINT LightTableSize = 4;

		ProjectiveCamera m_Camera;

		// Bind Light Stuff
		Graphic::ConstantBuffer* m_LightsCBV;
		Graphic::DescriptorTable* m_LightsTable;

		SceneLightsInfo m_LightInfo;

		std::vector<GObject*> m_ObjList;
	};
}
