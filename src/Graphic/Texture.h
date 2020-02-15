#pragma once
#include "Descriptor.h"
#include "CommandList.h"

#define ptrTexture std::shared_ptr<Graphic::Texture>

namespace Graphic {
	enum TextureType 
	{
		TEXTURE_SRV,	// Create a SRV texture
		TEXTURE_UAV,	// Create a UAV texture
		TEXTURE_RW	// Create both SRV & UAV
	};


	void LoadChessBoard(const UINT width, const UINT height, const UINT pixelSize, std::vector<UINT8>& data);
	

	// Texture
	// Texture class implement this base class should 
	//		1. Allocate memory from GPU mem manager
	//		2. initialize the view 
	//		3. Load data from file and copy(upload) it to the GPU memory
	class Texture {
	public:
		Texture(TextureType type) : m_Type(type) {}

		// Copy (one)view to in use descriptor heap
		inline void BindTexture(UINT index, TextureType type=TEXTURE_SRV) const
		{
			switch (type)
			{
			case Graphic::TEXTURE_SRV:
				m_SRV->BindDescriptor(index);
				break;
			case Graphic::TEXTURE_UAV:
				m_UAV->BindDescriptor(index);
				break;
			case Graphic::TEXTURE_RW:
				throw std::runtime_error("Bind only one texture(view) at a time!");
				break;
			default:
				break;
			}
		}

		inline ShaderResource* GetShaderResourceView() const { return m_SRV; }
		inline UnorderedAccess* GetUnorderedAccessView() const { return m_UAV; }

		// Write texture data to gpu memory
		// Only need to upload once since SRV, UAV will point to the same memory!
		inline void UploadTexture(D3D12_SUBRESOURCE_DATA& data) { m_SRV->CopyTexture(&data); }

		// Create 1d texture data
		template <class T>
		static D3D12_SUBRESOURCE_DATA CreateTextureData(const std::vector<T>&  data) {
			D3D12_SUBRESOURCE_DATA textureData = {};
			textureData.pData = &data[0];
			textureData.RowPitch = data.size() * sizeof(T);
			textureData.SlicePitch = data.size() * sizeof(T);
			return textureData;
		}
		
		// TODO Template
		static D3D12_SUBRESOURCE_DATA CreateTextureData(const UINT width, const UINT height, const UINT pixelSize, 
												 const std::vector<UINT8>& data) 
		{
			D3D12_SUBRESOURCE_DATA textureData = {};
			// Set infomation to m_textureData
			textureData.pData = &data[0];
			textureData.RowPitch = width * pixelSize;
			textureData.SlicePitch = textureData.RowPitch * height;
			return textureData;
		}

	protected:
		virtual void CreateSRV() = 0;
		virtual void CreateUAV() = 0;
		inline  void CreateRW() {CreateSRV(); CreateUAV();}
		inline  void CreateView() 
		{
			switch (m_Type)
			{
			case Graphic::TEXTURE_SRV:
				CreateSRV();
				break;
			case Graphic::TEXTURE_UAV:
				CreateUAV();
				break;
			case Graphic::TEXTURE_RW:
				CreateRW();
				break;
			default:
				throw std::runtime_error("Wrong Texture type");
				break;
			}	
		}

		ptrGPUMem m_gpuMem;
		TextureType m_Type;
		ShaderResource* m_SRV;
		UnorderedAccess* m_UAV;
		D3D12_RESOURCE_DESC m_textureDesc;
	};


	// Texture 
	class TextureBuffer : public Texture {
	public:
		TextureBuffer(UINT elementSize, UINT stride, TextureType type=TEXTURE_SRV);
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