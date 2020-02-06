#pragma once
#include "Descriptor.h"
#include "CommandList.h"

namespace Graphic {

	struct TextureData {
		std::vector<UINT8> data;
		const UINT TextureWidth;
		const UINT TextureHeighet;
		const UINT TexturePixelSize;
	};

	// Texture
	class Texture {
	public:
		Texture(UINT width, UINT height, D3D12_RESOURCE_DIMENSION dim=D3D12_RESOURCE_DIMENSION_TEXTURE2D);

		void CreateSRV(ComPtr<ID3D12Device> device, DescriptorHeap* descriptorHeap);

		void UploadTexture(const TextureData& textureData);

	private:
		ShaderResource* m_srv;
		D3D12_RESOURCE_DESC m_textureDesc;
	};


}