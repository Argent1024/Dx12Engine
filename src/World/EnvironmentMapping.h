#pragma once

#include "Graphic/Texture.h"
#include "GObject.h"

namespace Game {

	class SkyBox : public GObject {
	public:
		// TODO need to set material outside = =
		SkyBox() {
			ptrMesh mesh = TriangleMesh::GetSkyBox();
			SetMesh(mesh);
		}
	};

	std::shared_ptr<SkyBox> CreateSkyBoxFromFile(std::string path, UINT resolution, UINT16 miplevels);
}