#pragma once

#include "GraphicCore.h"
#include "CommandQueue.h"
#include "../World/Camera.h"
#include "../World/GObject.h"
#include "../World/Scene.h"
#include "../World/RenderEngine.h"


namespace Samples {
	using namespace Graphic;
	using namespace Game;

	class MeshTest : public GraphicCore {
	public:
		MeshTest(UINT t_width, UINT t_height, LPCTSTR t_title = L"playground")
			: GraphicCore(t_width, t_height, t_title), m_Scene(t_width, t_height) {}
			// m_Camera(t_width, t_height, Vector3(0.0f, 0.0f, 10.0f), Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f, 1.0f, 0.0f)) {}

		void Init(const HWND m_appHwnd) override;
		void Render() override;

	private:
		void LoadAssert();

		float angle = 0.0;
		float t = 10.0;
		UINT frame = 0;
		Scene m_Scene;
		RenderEngine* m_Render;
		GObject* obj0;


		ptrRootSignature m_rootSignature;
		ptrPSO m_GraphicPSO;		// Main Render Pass

		std::shared_ptr<SimpleMaterial> m_Material;
		std::shared_ptr<TriangleMesh> m_Mesh;
		// ptrMaterial m_Material;
		DirectionLight* m_Light; // TODO 
	};

}