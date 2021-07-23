#pragma once

#include "Graphic/Texture.h"
#include "GObject.h"

#define ptrEnvMap std::shared_ptr<Game::EnvironmentMap>

namespace Game {

	class EnvironmentMap : public Graphic::TextureCube {
	public:
		static ptrComputePSO RoughnessMipmapPSO;

		EnvironmentMap(UINT resolution, UINT texType, DXGI_FORMAT format, UINT16 miplevels);

		void CreateRoughnessMipmap();
	
	};

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