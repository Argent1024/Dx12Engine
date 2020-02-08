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

		inline UINT GetWidth() const { return m_Width; }
		inline UINT GetHeight() const { return m_Heighet; }

	private:
		D3D12_SUBRESOURCE_DATA m_textureData;
		std::vector<UINT8> m_data;
		const UINT m_Width;
		const UINT m_Heighet;
		const UINT m_PixelSize;
	};

	// Texture
	class Texture {
	public:
		Texture(UINT width, UINT height, D3D12_RESOURCE_DIMENSION dim=D3D12_RESOURCE_DIMENSION_TEXTURE2D);

		void CreateSRV(ComPtr<ID3D12Device> device);

		void UploadTexture(const TextureData& textureData);

		// Simple method to create the SRV, load texture from file and upload it to the SRV
		void CreateTexture(ComPtr<ID3D12Device> device);

		// Copy texture to in use descriptor heap
		inline void CopyTexture(ComPtr<ID3D12Device> device, UINT index) const {
			m_srv->CopyDescriptor(index);
		}

	private:
		ShaderResource* m_srv;
		D3D12_RESOURCE_DESC m_textureDesc;
	};


}