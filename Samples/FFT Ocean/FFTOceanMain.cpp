#pragma once

#include "stdafx.h"
#include "WinApp.h"

#include "World/Engine.h"
#include <iostream>

namespace Engine {
	class FFTOceanGE : public GameEngine {
	public:
		void LoadAssert() override;

		void GameUpdate() override { double dt = m_Timer.GetDeltaTime()/ 1000000.0; m_Ocean->Update(dt / 5.0);}

		void InitRenderEngine(const HWND appHwnd) override;

	private:
		std::shared_ptr<Game::PrincipleMaterial> m_Material;
		FFTOcean* m_Ocean;
	};


	void FFTOceanGE::InitRenderEngine(const HWND appHwnd) 
	{
		ptrRenderPass ocean_pass = std::make_shared<Game::DefaultRenderPass>();
		ptrGraphicsPSO ocean_pso = std::make_shared<OceanPSO>();

		RenderPassesTable render_passes = { {ocean_pass, ocean_pso} };
		m_RenderEngine = new RenderEngine(m_Setting.Width, m_Setting.Height);
		m_RenderEngine->Initialize(appHwnd, render_passes);
	}


	void FFTOceanGE::LoadAssert() {
		m_Scene = new Scene(m_Setting.Width, m_Setting.Height);
		m_Scene->Initialize();

		// Load asserts
		{
			m_Scene->ConfigLight(1, 4, 4);
			// Load light
			DirectionLight* m_Light = new DirectionLight();
			m_Scene->AddLight(*m_Light);
			m_Light->SetLightData({ {1.0, 1.0, 1.0}, {0.0 , 10.0, 0.0}, {1.0, 1.0, 2.5} });
			m_Material = std::make_shared<PrincipleMaterial>();

			// FFT Ocean Part
			m_Ocean = new FFTOcean();
			m_Ocean->Initialize();

			int n = 3;
			float gridSize = 2.0 * (1.0 - 4.0 / OceanResolution);
			for (int i = 0; i < n; ++i) {
				for (int j = 0; j < n; ++j) {
					GObject* ocean_obj = new Game::GObject();

					const float scale = 5.f;
					Vector3 offset = scale * Vector3(gridSize * (i - n / 2), gridSize * (j - n / 2), 0.0);
					ocean_obj->SetMesh(m_Ocean->GetMesh());
					ocean_obj->SetMaterial(m_Ocean->GetMaterial());
					ocean_obj->SetTransform(Transform({ scale, 0, 0 },
						{ 0, scale, 0 },
						{ 0, 0, scale }, offset));
					m_Scene->AddGameObj(ocean_obj);


				}
			}


			ProjectiveCamera& camera = m_Scene->GetMainCamera();
			// Vector3 position(5.2, 0.0, 0.2);
			Vector3 position(7.2, 7.2, 1.2);

			Vector3 origin(-3.0, 0., 0.);
			Vector3 worldUp(0., 0., 1.0);
			camera.LookAt(position, origin, worldUp);

		}
	}

}
_Use_decl_annotations_
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow)
{

	Engine::FFTOceanGE engine = Engine::FFTOceanGE(); // new Engine::GameEngine();
	Engine::GameEngine* pEngine = &engine;
	return WinApp::RunApp(pEngine, hInstance, nCmdShow);
}