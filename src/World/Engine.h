#pragma once
#include "RenderEngine.h"
#include "GameInput.h"
#include "FFTOcean.h"
#include "Utility/Timer.h"

namespace Engine {

	// Do everything here
	// Render, Input, Update ...
	class GameEngine {
	public:
		struct GameConfiguration 
		{
			UINT Width = 1080;
			UINT Height = 960;
			LPCTSTR Title = L"playground";
		};

		void GameLoop() 
		{
			// LogicUpdate();
			Render();
			InputUpdate();
		}

		void Initialize(const HWND appHwnd);

		// (TODO) Load assert from file, create m_Scene, m_GameControl
		void LoadAssert();

		GameConfiguration& GetSetting() { return m_Setting; }

	protected:
		UINT frame = 0;
		Utility::Timer m_Timer;

		GameConfiguration m_Setting;

		RenderEngine* m_RenderEngine;
		
		// Data
		Game::Scene* m_Scene;
		Game::GameControl* m_GameControl;
		// Game::GameLogic* m_Logic;

		// TODO Resource Manager
		std::shared_ptr<Game::Mesh> m_Mesh;
		std::shared_ptr<Game::PrincipleMaterial> m_Material;

		FFTOcean* m_Ocean;


		void Render();

		void InputUpdate();

		// void LogicUpdate();
	};



}