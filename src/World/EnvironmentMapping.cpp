#include "stdafx.h"
#include "EnvironmentMapping.h"
using namespace Graphic;

namespace Game {


	//void EnvMapping::CreateCubeMap(ptrTex2D texture) {
	//	assert(m_Texture);
	//	assert((texture->GetType() & TEXTURE_SRV) && (m_Texture->GetType() & TEXTURE_UAV));



	//	

	//}

	std::shared_ptr<SkyBox> CreateSkyBoxFromFile(std::string path, UINT resolution, UINT16 miplevels) {
		UINT texType = TEXTURE_SRV | TEXTURE_UAV;
		DXGI_FORMAT format=DXGI_FORMAT_R8G8B8A8_UNORM;
		ptrTexCube texCube = std::make_shared<Graphic::TextureCube>(resolution, texType, format, miplevels);
		texCube->LoadFromImage(path);

		// TODO roughness mipmap


		auto Mat = std::shared_ptr<PureColorCube>();
		Mat->SetTexture(0, texCube);
		Mat->UploadCBV();

		auto skybox = std::shared_ptr<SkyBox>();
		const float defaultScale = 1000.f;
		skybox->SetTransform(
			Math::Transform({ defaultScale, 0, 0 }, { 0, defaultScale, 0 }, { 0, 0, defaultScale }, Math::Vector3(Math::kZero))
		);

		skybox->SetMaterial(Mat);
	}
}