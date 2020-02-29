#pragma once

#include "GraphicCore.h"
#include "CommandQueue.h"
#include "../World/Camera.h"
#include "../World/GObject.h"


namespace Samples {
	using namespace Graphic;
	using namespace Game;

	class MeshTest : public GraphicCore {
	public:
		void Init(const HWND m_appHwnd) override {}
		void Render() override {}

	private:
		std::shared_ptr<TriangleMesh> m_Mesh;
		ptrMaterial m_Material;

		GObject m_Bunny;

	};

}