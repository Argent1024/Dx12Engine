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
		m_Light->SetLightData({ {1.0, 1.0, 1.0}, {0.0 , 10.0, 0.0}, {0.0, 3.0, 0.0} });
		this->LoadAssert();

	}

	void MeshTest::LoadAssert() 
	{
		MeshReader reader;
		reader.ReadOBJ("D:\\work\\tEngine\\bunny.obj");

		std::vector<DefaultVertex>& vertex = reader.m_vertex;
		std::vector<UINT>& index = reader.m_index;

		m_Mesh = std::make_shared<TriangleMesh>(vertex, index);
		m_Material = std::make_shared<SimpleMaterial>(Vector3{0.75f, 0.75f, 0.9f});
		m_Material->Initialize();

		ptrMaterial mat1 = std::make_shared<SimpleMaterial>(Vector3{1.0, 0.0, 0.0});
		mat1->Initialize();

		ptrMaterial mat2 = std::make_shared<SimpleMaterial>(Vector3{0.0, 1.0, 0.0});
		mat2->Initialize();

		// TODO!! error C2338 aligin?
		obj0 = new Game::GObject();
		obj0->SetMesh(m_Mesh);
		obj0->SetMaterial(m_Material);
		obj0->Initialize();
		obj0->SetTransform(Transform({ 1.0, 0, 0 }, { 0, 1.0, 0 }, { 0, 0, 1.0 }, {0.0, 0, 0}));


		// Test Depth buffer
		ptrMesh plane = TriangleMesh::GetXYPlane();
		GObject* obj1 = new Game::GObject();

		obj1->SetMesh(plane);
		obj1->SetMaterial(mat1);
		obj1->Initialize();
		obj1->SetTransform(Transform({ 3, 0, 0 }, { 0, 4, 0 }, { 0, 0, 3 }, { 1, 0, 4.0}));
		
		GObject* obj2 = new Game::GObject();
		obj2->SetMesh(plane);
		obj2->SetMaterial(mat2);
		obj2->Initialize();
		obj2->SetTransform(Transform({ 3, 0, 0 }, { 0, 1, 0 }, { 0, 0, 1 }, {-2, 0, 5.0}));


		m_Scene.AddGameObj(obj0);
		//m_Scene.AddGameObj(obj1);
		//m_Scene.AddGameObj(obj2);
	}

	void MeshTest::Render()
	{
		// m_Scene->Render();
		m_Render->Render(m_Scene);
		frame ++;
		if (frame % 40 == 0) {
			Matrix4 t = obj0->GetTransform();
			Matrix4 r = Matrix4(XMMatrixRotationAxis({ 0, 1, 0 }, 0.1));
			t = t * r;
			obj0->SetTransform(Transform(t));
		}
	}
}