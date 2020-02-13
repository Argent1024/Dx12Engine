#pragma once
#include "Descriptor.h"
#include "CommandList.h"

#define ptrTexture std::shared_ptr<Graphic::Texture>

namespace Graphic {
	enum TextureType 
	{
		TEXTURE_SRV,
		TEXTURE_UAV
	};

	// Helper class to upload texture data
	// Usage:
	//	In Texture Class
	//	{	
	//		TextureData data(...);	<-- load data
	//		UploadTexture(data);	<-- do upload
	//		return;
	//	}
	class TextureData {
	public:
		TextureData(const UINT size);

		// Load chess board texture data
		TextureData(const UINT width, const UINT height, const UINT piexlSize=4);

		inline D3D12_SUBRESOURCE_DATA GetTextureData() const {return m_textureData; }

	private:
		D3D12_SUBRESOURCE_DATA m_textureData;
		std::vector<UINT8> m_data;	// Does UINT8 ok?
	};

	// Texture
	// Texture class implement this base class should 
	//		1. Allocate memory from GPU mem manager
	//		2. initialize the view 
	//		3. Load data from file and copy(upload) it to the GPU memory
	class Texture {
	public:
		Texture(TextureType type) : m_Type(type) {}
		// Copy texture to in use descriptor heap
		inline void BindTexture(UINT index) const { m_View->BindDescriptor(index); }

		inline HeapDescriptor* GetShaderResourceView() { return m_View; }

	protected:
		virtual void CreateSRV() = 0;
		virtual void CreateUAV() = 0;

		// Write texture data to gpu memory
		inline void UploadTexture(const TextureData& textureData) { m_View->CopyTexture(&textureData.GetTextureData()); }

		TextureType m_Type;
		HeapDescriptor* m_View;
		D3D12_RESOURCE_DESC m_textureDesc;
	};


	// Texture 
	class TextureBuffer : public Texture {
	public:
		TextureBuffer(UINT size, UINT stride, TextureType type=TEXTURE_SRV);
	private:
		void CreateSRV() override;
		void CreateUAV() override;
		UINT m_size;
		UINT m_stride;
	};

	class Texture2D : public Texture{
	public:
		Texture2D(UINT width, UINT height, TextureType type=TEXTURE_SRV, const std::wstring& textureFile=L"");
	private:
		void CreateSRV() override;
		void CreateUAV() override;
	};


}