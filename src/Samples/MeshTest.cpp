#include"MeshTest.h"
#include"../Utility/MeshReader.h"

namespace Samples {

	void MeshTest::Init(const HWND m_appHwnd)
	{
		Engine::InitializeEngine();

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
		m_Light->SetLightData({ {1.0, 1.0, 1.0}, {1.0 , 1.0, 1.0}, {1.0, 1.0, 1.0} });
		this->LoadAssert();

	}

	void MeshTest::LoadAssert() 
	{
		MeshReader reader;
		reader.ReadOBJ("D:\\work\\tEngine\\bunny.obj");

		std::vector<DefaultVertex>& vertex = reader.m_vertex;
		std::vector<UINT>& index = reader.m_index;

		m_Mesh = std::make_shared<TriangleMesh>(vertex, index);
		//m_Material = std::make_shared<Material>();
		
		// TODO!! error C2338 aligin?
		GObject* obj0 = new Game::GObject();
		obj0->SetMesh(m_Mesh);
		// obj0->SetMaterial(m_Material);
		obj0->Initialize();
		obj0->SetTransform(Transform({ 2, 0, 0 }, { 0, 2, 0 }, { 0, 0, 2 }, {0, 0, 0}));
		m_Scene.AddGameObj(obj0);
	}

	void MeshTest::Render()
	{
		// m_Scene->Render();
		m_Render->Render(m_Scene);
	}
}