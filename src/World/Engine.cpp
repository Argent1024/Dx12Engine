#include "stdafx.h"
#include "Engine.h"
#include "Utility/logger.h"
#include "Utility/MeshReader.h"

namespace Engine {
	using namespace Math;
	using namespace Game;

	void GameEngine::Initialize(const HWND m_appHwnd)
	{
		Engine::InitializeEngine();
		
		// Control
		Engine::InitializeInputManager(m_appHwnd);
		m_GameControl = new SimpleCameraControl();

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
			m_Light->SetLightData({ {1.0, 1.0, 1.0}, {0.0 , 10.0, 0.0}, {0.0, 1.0, 1.0} });
			m_Material = std::make_shared<PrincipleMaterial>();

			// FFT Ocean Part
			m_Ocean = new FFTOcean();
			m_Ocean->Initialize();

			GObject* ocean_obj = new Game::GObject();
			const float scale = 5.f;
			ocean_obj->SetMesh(m_Ocean->GetMesh());
			ocean_obj->SetMaterial(m_Ocean->GetMaterial());
			ocean_obj->SetTransform(Transform({ scale, 0, 0 }, 
											  { 0, scale, 0 }, 
											  { 0, 0, scale }, {0.0, 0.0, 0.0}));
			m_Scene->AddGameObj(ocean_obj);

			ProjectiveCamera& camera = m_Scene->GetMainCamera();
			// Vector3 position(5.2, 0.0, 0.2);
			Vector3 position(10.2, 0.0, 2.2);
			Vector3 origin(0., 0., 0.);
			Vector3 worldUp(0., 0., 1.0);
			camera.LookAt(position, origin, worldUp);

			// Normal mesh stuff
			/*
			PrincipleMaterial::MatData mat;
			DirectX::XMStoreFloat3(&mat.BaseColor, Vector3(1.0, 0.0, 0.0));
			mat.Roughness = 0.15f;
			mat.Specular = 0.0f;
			m_Material->SetData(mat);
			m_Material->UploadCBV();
		
			MeshReader reader;
			reader.ReadOBJ("D://work/tEngine/airboat.obj");
			//reader.ReadOBJ("D:\\banana.obj");

			std::vector<DefaultVertex>& vertex = reader.m_vertex;
			std::vector<UINT>& index = reader.m_index;

			m_Mesh = std::make_shared<TriangleMesh>(vertex, index);
			*/

			//// Texture test
			//m_Mesh = TriangleMesh::GetXYPlane();
			//std::string texpath = "D://work/tEngine/snow.png";
			//Graphic::Texture2D* tex = new Graphic::Texture2D(texpath);

			//m_Material->SetTexture(PrincipleMaterial::DiffuseTex, tex);
			//m_Material->UploadCBV();
			//// Texture Test end


			// TODO error C2338 aligin
			/*GObject* obj0 = new Game::GObject();
			const float scale = 1.f;
			obj0->SetMesh(m_Mesh);
			obj0->SetMaterial(m_Material);
			obj0->SetTransform(Transform({ scale, 0, 0 }, { 0, scale, 0 }, { 0, 0, scale }, {0.0, 0.0, 0.0}));

			m_Scene->AddGameObj(obj0);
			*/
		}
		m_Timer.Start();
	}

	void GameEngine::Render() 
	{	
		double dt = m_Timer.GetDeltaTime()/ 1000000.0;
		m_Ocean->Update(dt / 5.0);
		m_RenderEngine->Render(*m_Scene);
	}

	void GameEngine::InputUpdate()
	{
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

				RenderEngine::RenderConfiguration& renderSetting = m_RenderEngine->GetRenderSetting();

				DefaultRenderPass& pass = m_RenderEngine->GetDefaultPass();
				DefaultRenderPass::ConstBufferData& data = pass.GetCBVData();


				renderSetting.mixpass = false;
				data.debugnormal = false;
				data.debugpos = true;
			}
		}
		m_GameControl->UpdateScene(*m_Scene);

		frame++;
	}



}