#include "Engine.h"
#include "Utility/logger.h"
#include "Utility/MeshReader.h"

namespace Engine {
	using namespace Math;
	using namespace Game;

	void GameEngine::Initialize(const HWND m_appHwnd)
	{
		Engine::InitializeEngine();
		Engine::InitializeInputManager(m_appHwnd);

		m_RenderEngine = new RenderEngine(m_Setting.Width, m_Setting.Height);
		m_RenderEngine->Initialize(m_appHwnd);

		// TODO should put into logic
		LoadAssert();
	}

	void GameEngine::LoadAssert()
	{
		m_Scene = new Scene(m_Setting.Width, m_Setting.Height);
		m_Scene->Initialize();

		// Load asserts
		{
			m_Scene->ConfigLight(1, 4, 4);
			// Load light
			DirectionLight* m_Light = new DirectionLight();
			m_Scene->AddLight(*m_Light);
			m_Light->SetLightData({ {1.0, 1.0, 1.0}, {0.0 , 10.0, 0.0}, {0.0, 1.0, 0.0} });
			m_Material = std::make_shared<PrincipleMaterial>();

			PrincipleMaterial::Data mat;
			DirectX::XMStoreFloat3(&mat.BaseColor, Vector3(1.0, 0.0, 0.0));
			mat.Roughness = 0.15f;
			mat.Specular = 0.0f;
			m_Material->SetData(mat);
			m_Material->UploadCBV();
		
			MeshReader reader;
			reader.ReadOBJ("D:\\work\\tEngine\\bunny.obj");

			std::vector<DefaultVertex>& vertex = reader.m_vertex;
			std::vector<UINT>& index = reader.m_index;

			m_Mesh = std::make_shared<TriangleMesh>(vertex, index);
		
			// TODO!! error C2338 aligin?
			GObject* obj0 = new Game::GObject();
			obj0->SetMesh(m_Mesh);
			obj0->SetMaterial(m_Material);
			obj0->SetTransform(Transform({ 1.0, 0, 0 }, { 0, 1.0, 0 }, { 0, 0, 1.0 }, {0.0, 0.0, 0.0}));

			m_Scene->AddGameObj(obj0);
		}


	}

	void GameEngine::Render() 
	{
		m_RenderEngine->Render(*m_Scene);
	}

	void GameEngine::InputUpdate()
	{
		InputManager& input = Engine::GetInputManager();


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

				RenderEngine::RenderConfiguration& renderSetting = m_RenderEngine->GetRenderSetting();

				Game::DefaultRenderPass& pass = m_RenderEngine->GetDefaultPass();
				Game::DefaultRenderPass::ConstBufferData& data = pass.GetCBVData();

				renderSetting.mixpass = false;
				data.debugnormal = true;
				data.debugpos = false;

			}

			// press f3 to show normal
			if (input.IsKeyPressed(Keyboard::F3)) {

				RenderEngine::RenderConfiguration& renderSetting = m_RenderEngine->GetRenderSetting();

				DefaultRenderPass& pass = m_RenderEngine->GetDefaultPass();
				DefaultRenderPass::ConstBufferData& data = pass.GetCBVData();


				renderSetting.mixpass = false;
				data.debugnormal = false;
				data.debugpos = true;
			}
		}
		m_GameControl->UpdateScene(*m_Scene);

		input.Update();
		frame++;
	}



}