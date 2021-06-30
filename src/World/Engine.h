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
			UINT Width = 1920;
			UINT Height = 1080;
			std::wstring Title = L"playground";
		};

		virtual void GameLoop() 
		{
			Render();
			InputUpdate();
			GameUpdate();
			frame++;
		}

		void Initialize(const HWND appHwnd) {
			Engine::InitializeEngine();

			Engine::InitializeInputManager(appHwnd);

			InitRenderEngine(appHwnd);
			LoadAssert();

			m_Timer.Start();
		}

		
		// Create a default RenderEngine
		virtual void InitRenderEngine(const HWND appHwnd) {  	
			m_RenderEngine = new RenderEngine(m_Setting.Width, m_Setting.Height);
			m_RenderEngine->Initialize(appHwnd);
		}


		// (TODO) Load assert from file, create m_Scene, m_GameControl
		virtual void LoadAssert() = 0;

		GameConfiguration& GetSetting() { return m_Setting; }

	protected:
		UINT frame = 0;
		Utility::Timer m_Timer;

		GameConfiguration m_Setting;

		RenderEngine* m_RenderEngine;
		
		// Data
		Game::Scene* m_Scene;
		Game::GameControl* m_GameControl;

		virtual void Render() {
			m_RenderEngine->Render(*m_Scene);
		};

		virtual void GameUpdate() {
			return;
		}

		virtual void InputUpdate() {
			InputManager& input = Engine::GetInputManager();
			input.Update();

			auto kb = input.GetKeyboardState();
			auto mouse = input.GetMouseState();
			
			if (m_GameControl) {
				m_GameControl->UpdateScene(*m_Scene);
			}
			frame++;
		}

		// void LogicUpdate();
	};



}