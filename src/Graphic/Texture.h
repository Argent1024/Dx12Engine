#pragma once
#include "Descriptor.h"
#include "CommandList.h"

#define ptrTexture std::shared_ptr<Graphic::Texture>

namespace Graphic {

	class TextureData {
	public:
		TextureData(const UINT width, const UINT height, const UINT piexlSize=4);

		inline D3D12_SUBRESOURCE_DATA GetTextureData() const {return m_textureData; }

		void LoadTexture();

	private:
		D3D12_SUBRESOURCE_DATA m_textureData;
		std::vector<UINT8> m_data;	// Does UINT8 ok?
	};

	// Texture
	class Texture {
	public:
		Texture(UINT width, UINT height, D3D12_RESOURCE_DIMENSION dim=D3D12_RESOURCE_DIMENSION_TEXTURE2D);

		// Simple method to create the SRV, load texture from file and upload it to the SRV
		void CreateTexture();

		// Copy texture to in use descriptor heap
		inline void BindTexture(UINT index) const {
			m_srv->BindDescriptor(index);
		}

	private:
		void CreateSRV();
		void UploadTexture(const TextureData& textureData);

		ShaderResource* m_srv;
		D3D12_RESOURCE_DESC m_textureDesc;
	};


}