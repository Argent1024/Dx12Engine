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
			std::wstring Title = L"helloword";
			// LPCTSTR Title = L"playground";
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

			m_RenderEngine = new RenderEngine(m_Setting.Width, m_Setting.Height);
			m_RenderEngine->Initialize(appHwnd);

			LoadAssert();

			m_Timer.Start();
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
			// Change Render Setting
			{
				// Press f1 to show depth texture
				if (input.IsKeyPressed(Keyboard::F1)) {
					RenderEngine::RenderConfiguration& renderSetting = m_RenderEngine->GetRenderSetting();
					renderSetting.mixpass = !renderSetting.mixpass;
				}

				// press f2 to show normal
				if (input.IsKeyPressed(Keyboard::F2)) {
					// Reset camera for FFT ocean
					ProjectiveCamera& camera = m_Scene->GetMainCamera();
					Vector3 position(0.0f, 0.0f, 10.f);
					Vector3 origin(0.0f, 0.0f, 0.0f);
					Vector3 worldUp(0.0f, 1.0f, 0.0f);
					camera.LookAt(position, origin, worldUp);

					RenderEngine::RenderConfiguration& renderSetting = m_RenderEngine->GetRenderSetting();

					Game::DefaultRenderPass& pass = m_RenderEngine->GetDefaultPass();
					Game::DefaultRenderPass::ConstBufferData& data = pass.GetCBVData();

					renderSetting.mixpass = false;
					data.debugnormal = true;
					data.debugpos = false;

				}

				// press f3 to show normal
				if (input.IsKeyPressed(Keyboard::F3)) {

					// Reset camera for FFT ocean
					ProjectiveCamera& camera = m_Scene->GetMainCamera();
					Vector3 position(0.0f, 0.0f, 10.f);
					Vector3 origin(0.0f, 0.0f, 0.0f);
					Vector3 worldUp(0.0f, 1.0f, 0.0f);
					camera.LookAt(position, origin, worldUp);

					RenderEngine::RenderConfiguration& renderSetting = m_RenderEngine->GetRenderSetting();

					DefaultRenderPass& pass = m_RenderEngine->GetDefaultPass();
					DefaultRenderPass::ConstBufferData& data = pass.GetCBVData();


					renderSetting.mixpass = false;
					data.debugnormal = false;
					data.debugpos = true;
				}
			}
			if (m_GameControl) {
				m_GameControl->UpdateScene(*m_Scene);
			}
			frame++;
		}

		// void LogicUpdate();
	};



}