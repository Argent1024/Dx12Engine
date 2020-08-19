#include "MeshTest.h"
#include "Utility/MeshReader.h"
#include "Utility/input.h"
#include "Utility/logger.h"

namespace Samples {

	using namespace Math;


	void MeshTest::Init(const HWND m_appHwnd)
	{
		Engine::InitializeEngine();
		Engine::InitializeInputManager(m_appHwnd);

		/*m_rootSignature = std::make_shared<RootSignature>();
		m_rootSignature->Initialize();

		m_GraphicPSO = std::make_shared<MeshTestPSO>();
		m_GraphicPSO->SetRootSigature(m_rootSignature->GetRootSignature());
		m_GraphicPSO->Initialize();*/

		// m_Camera.CreateCBV();
		// Create Scene
		
		m_Scene.Initialize();

		m_Render = new RenderEngine(m_width, m_height);
		m_Render->Initialize(m_appHwnd);

		m_Scene.ConfigLight(1, 4, 4);
		// Load light
		m_Light = new DirectionLight();
		m_Scene.AddLight(*m_Light);
		m_Light->SetLightData({ {1.0, 1.0, 1.0}, {0.0 , 10.0, 0.0}, {0.0, 1.0, 0.0} });
		this->LoadAssert();

	}

	void MeshTest::LoadAssert() 
	{
		m_Material = std::make_shared<PrincipleMaterial>();

		PrincipleMaterial::Data mat;
		DirectX::XMStoreFloat3(&mat.BaseColor, Vector3(1.0, 0.0, 0.0));
		mat.Roughness = 0.15f;
		mat.Specular = 0.0f;
		m_Material->SetData(mat);
		m_Material->UploadCBV();
		
		//m_Mesh = TriangleMesh::GetSphere(64, 32);
		
		MeshReader reader;
		reader.ReadOBJ("D:\\work\\tEngine\\bunny.obj");

		std::vector<DefaultVertex>& vertex = reader.m_vertex;
		std::vector<UINT>& index = reader.m_index;

		m_Mesh = std::make_shared<TriangleMesh>(vertex, index);
		

		// TODO!! error C2338 aligin?
		obj0 = new Game::GObject();
		obj0->SetMesh(m_Mesh);
		 //obj0->SetMesh(plane);
		obj0->SetMaterial(m_Material);
		
		obj0->SetTransform(Transform({ 1.0, 0, 0 }, { 0, 1.0, 0 }, { 0, 0, 1.0 }, {0.0, 0.0, 0.0}));

		m_Scene.AddGameObj(obj0);
		
	}

	void MeshTest::Render()
	{
		// m_Scene->Render();
		m_Render->Render(m_Scene);
	}

	void MeshTest::Update() {
		InputManager& input = Engine::GetInputManager();

		auto kb = input.GetKeyboardState();
		auto mouse = input.GetMouseState();
		
		// TODO move debug view into render engine later 
		// Press f1 to show depth texture
		if (input.IsKeyPressed(Keyboard::F1)) {
			RenderEngine::Config& renderSetting = m_Render->GetRenderSetting();
			renderSetting.mixpass = !renderSetting.mixpass;
		}

		// press f2 to show normal
		if (input.IsKeyPressed(Keyboard::F2)) {

			RenderEngine::Config& renderSetting = m_Render->GetRenderSetting();

			DefaultRenderPass& pass = m_Render->GetDefaultPass();
			DefaultRenderPass::ConstBufferData& data = pass.GetCBVData();
			
			renderSetting.mixpass = false;
			data.debugnormal = true;
			data.debugpos = false;
			
		}

		// press f3 to show normal
		if (input.IsKeyPressed(Keyboard::F3)) {

			RenderEngine::Config& renderSetting = m_Render->GetRenderSetting();

			DefaultRenderPass& pass = m_Render->GetDefaultPass();
			DefaultRenderPass::ConstBufferData& data = pass.GetCBVData();
			
			
			renderSetting.mixpass = false;
			data.debugnormal = false;
			data.debugpos = true;
		}

		frame ++;
		if (kb.E) {
			Matrix4 t = obj0->GetTransform();
			Matrix4 r = Matrix4(XMMatrixRotationAxis({ 0, 1, 0 }, 0.01f));
			t = t * r;
			obj0->SetTransform(Transform(t));
		}

		if (kb.Q) {
			Matrix4 t = obj0->GetTransform();
			Matrix4 r = Matrix4(XMMatrixRotationAxis({ 0, 1, 0 }, -0.01f));
			t = t * r;
			obj0->SetTransform(Transform(t));
		}

		if (kb.W) {
			Matrix4 t = obj0->GetTransform();
			Matrix4 r = Matrix4(XMMatrixRotationAxis({ 1, 0, 0 }, 0.01f));
			t = t * r;
			obj0->SetTransform(Transform(t));
		}

		if (kb.S) {
			Matrix4 t = obj0->GetTransform();
			Matrix4 r = Matrix4(XMMatrixRotationAxis({ 1, 0, 0 }, -0.01f));
			t = t * r;
			obj0->SetTransform(Transform(t));
		}
		/*
		Camera& camera = m_Scene.GetMainCamera();
		float cameraSpeed = 0.01;
		if (kb.Up) {
			camera.RotatePitch(cameraSpeed);
		} 
		if (kb.Down) {
			camera.RotatePitch(-cameraSpeed);
		}
		if (kb.Left) {
			camera.RotateYaw(cameraSpeed);
		}
		if (kb.Right) {
			camera.RotateYaw(-cameraSpeed);
		}
		camera.Look();
		*/
		
		PrincipleMaterial::Data& matData = m_Material->GetData();
		
		if (kb.Up) {
			matData.Specular += 0.01f;
			Logger::Log(matData.Specular, "Set Specular :");
			m_Material->UploadCBV();
		}
		
		if (kb.Down) {
			matData.Specular -= 0.01f;
			Logger::Log(matData.Specular, "Set Specular :");
			m_Material->UploadCBV();
		}

		if (kb.Left) {
			matData.Roughness += 0.01f;
			Logger::Log(matData.Roughness, "Set Roughness :");
			m_Material->UploadCBV();
		}

		if (kb.Right) {
			matData.Roughness -= 0.01f;
			Logger::Log(matData.Roughness, "Set Roughness :");
			m_Material->UploadCBV();
		}
		input.Update();
	}

}