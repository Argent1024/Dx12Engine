#pragma once
#include "Descriptor.h"

#define ptrTexture std::shared_ptr<Graphic::Texture>

namespace Graphic {

	enum TextureType 
	{
		TEXTURE_SRV = 1,	// Create a SRV texture
		TEXTURE_UAV = 2,	// Create a UAV texture
		TEXTURE_CBV = 4,		
		TEXTURE_DSV = 8,	// Create a DSV texture
		TEXTURE_RTV = 16
	};

	struct ImageMetadata
	{
		int width;
		int height;
		int channel;
		int pixelSize;	// In Byte
	};

	void LoadChessBoard(const UINT width, const UINT height, const UINT pixelSize, std::vector<UINT8>& data);

	// TODO when creating views, the control of format stuff need to be exposed outside
	// Texture
	// Texture class implement this base class should 
	//		1. Allocate memory from GPU mem manager
	//		2. initialize views (multiple views using the same gpu memory)
	//		3. Load data from file and copy(upload) it to the GPU memory
	class Texture {
	public:
		Texture(UINT type) : m_Type(type) {
			assert(type != 0 && " At least one type is needed Are you using && instead of || when creating type?");
			assert(!((type & TEXTURE_RTV) && (type & TEXTURE_DSV)) && "Creating RTV & DSV same time");
		}

		inline UINT GetType() { return m_Type; }
		/*// Copy (one)view to in use descriptor heap
		inline void BindTexture(UINT index, TextureType type=TEXTURE_SRV) const
		{
			assert(type & m_Type && "Binding view type not created");
			switch (type)
			{
			case Graphic::TEXTURE_SRV:
				m_SRV->BindDescriptor(index);
				break;
			case Graphic::TEXTURE_UAV:
				m_UAV->BindDescriptor(index);
				break;
			case Graphic::TEXTURE_CBV:
				
				break;
			case Graphic::TEXTURE_DSV:
				// I think we don't need to bind DSV / RTV
				break;
			case Graphic::TEXTURE_RTV:

				break;
			default:
				break;
			}
		} */

		// Giving a slot, create the view on that slot
		inline void CreateView(TextureType type, DescriptorTable* table=nullptr, UINT index=0) {
			assert(type & m_Type && "Creating type not mentioned in constructor");
			switch (type)
			{
			case Graphic::TEXTURE_SRV:
				CreateSRV(table, index);
				break;
			case Graphic::TEXTURE_UAV:
				CreateUAV(table, index);
				break;
			case Graphic::TEXTURE_CBV:
				CreateCBV(table, index);
				break;
			case Graphic::TEXTURE_DSV:
				CreateDSV(table, index);
				break;
			case Graphic::TEXTURE_RTV:
				CreateRTV(table, index);
				break;
			default:
				break;
			}
		}
		
		inline ShaderResource* GetShaderResourceView() const 
		{ 
			assert(m_Type & TEXTURE_SRV && "SRV not created for this texture");
			return m_SRV; 
		}
		
		inline UnorderedAccess* GetUnorderedAccessView() const 
		{ 
			assert(m_Type & TEXTURE_UAV && "DSV not created for this texture");
			return m_UAV; 
		}

		inline DepthStencil* GetDepthStencilView() const 
		{
			assert(m_Type & TEXTURE_DSV && "DSV not created for this texture");
			return m_DSV; 
		}

		inline RenderTarget* GetRenderTargetView() const
		{
			assert(m_Type & TEXTURE_RTV && "RTV not created for this texture");
			return m_RTV; 
		}

		// Write texture data to gpu memory
		// Only need to upload once since all views point to the same memory!
		inline void UploadTexture(D3D12_SUBRESOURCE_DATA* data) {
			assert(m_Type & TEXTURE_SRV  && "The Texture type doesn't contain SRV");

			// May not init SRV when calling upload
			// m_SRV->CopyTexture(&data); 
			Engine::MemoryAllocator.UploadTexure(*m_gpuMem, data);
		}

		// Create 1d texture data
		template <class T>
		static D3D12_SUBRESOURCE_DATA CreateTextureData(const std::vector<T>&  data) {
			D3D12_SUBRESOURCE_DATA textureData = {};
			textureData.pData = &data[0];
			textureData.RowPitch = data.size() * sizeof(T);
			textureData.SlicePitch = data.size() * sizeof(T);
			return textureData;
		}
		
		// Create 2d texture data
		static D3D12_SUBRESOURCE_DATA CreateTextureData(
			const ImageMetadata& metadata, 
			const std::vector<UINT8>& data) 
		{
			D3D12_SUBRESOURCE_DATA textureData = {};
			// Set infomation to m_textureData
			textureData.pData = &data[0];
			textureData.RowPitch = metadata.width * metadata.pixelSize;
			textureData.SlicePitch = textureData.RowPitch * metadata.height;
			return textureData;
		}

		// Create 2d texture data (helper method for stb_image, so don't need to copy the image again)
		static D3D12_SUBRESOURCE_DATA CreateTextureData(
			const ImageMetadata& metadata, 
			const unsigned char* data) 
		{
			D3D12_SUBRESOURCE_DATA textureData = {};
			// Set infomation to m_textureData
			textureData.pData = data;
			textureData.RowPitch = metadata.width * metadata.pixelSize;
			textureData.SlicePitch = textureData.RowPitch * metadata.height;
			return textureData;
		}

		/*
		// TODO Need this when writing image back to disk
		static ImageMetadata CalculateMetaData() 
		{
			
		} */

	protected:
		// Create the SRV & UAV at the table at tableIndex
		virtual void CreateCBV(DescriptorTable* table=nullptr, UINT tableIndex=0) = 0;
		virtual void CreateSRV(DescriptorTable* table=nullptr, UINT tableIndex=0) = 0;
		virtual void CreateUAV(DescriptorTable* table=nullptr, UINT tableIndex=0) = 0;
		virtual void CreateDSV(DescriptorTable* table=nullptr, UINT tableIndex=0) = 0;
		virtual void CreateRTV(DescriptorTable* table=nullptr, UINT tableIndex=0) = 0;

		ptrGPUMem m_gpuMem;

		// Store or value for texture_type
		// EX: m_Type = TEXUTRE_SRV | TEXTURE_UAV | TEXTURE_DSV
		const UINT m_Type;

		// TODO Use clever way
		ConstantBuffer* m_CBV;
		ShaderResource* m_SRV;
		UnorderedAccess* m_UAV;
		DepthStencil* m_DSV;
		RenderTarget* m_RTV;

		D3D12_RESOURCE_DESC m_textureDesc;
	};


	// Texture 
	class TextureBuffer : public Texture {
	public:
		// TODO better way to express type?
		TextureBuffer(UINT elementNum, UINT stride, UINT type=TEXTURE_SRV);

		// Just Create a simple CBV
		TextureBuffer(UINT totalSize);

	private:
		void CreateCBV(DescriptorTable* table=nullptr, UINT tableIndex=0) override;
		void CreateSRV(DescriptorTable* table=nullptr, UINT tableIndex=0) override;
		void CreateUAV(DescriptorTable* table=nullptr, UINT tableIndex=0) override;
		void CreateDSV(DescriptorTable* table=nullptr, UINT tableIndex=0) override;
		void CreateRTV(DescriptorTable* table=nullptr, UINT tableIndex=0) override;
		UINT m_elementNum;
		UINT m_stride;
		UINT m_totalSize;
	};

	// TODO dont use stb_image, only support 8-bit's channel
	class Texture2D : public Texture {
	public:
		// TODO better way to express type?
		Texture2D(UINT width, UINT height, UINT type=TEXTURE_SRV);

		Texture2D(std::string& filename, UINT type=TEXTURE_SRV);

	private:
		void TextureDescHelper(UINT width, UINT height);

		// After we have m_textureDesc and m_Type Allocate GPU memory and create texture
		void Initialize();

		ImageMetadata LoadFromImage(std::string& filename, unsigned char*& data);

		void CreateCBV(DescriptorTable* table=nullptr, UINT tableIndex=0) override;
		void CreateSRV(DescriptorTable* table=nullptr, UINT tableIndex=0) override;
		void CreateUAV(DescriptorTable* table=nullptr, UINT tableIndex=0) override;
		void CreateDSV(DescriptorTable* table=nullptr, UINT tableIndex=0) override;
		void CreateRTV(DescriptorTable* table=nullptr, UINT tableIndex=0) override;
	};

	/*
	class Texture3D : public Texture 
	{
	public:
		Texture3D(UINT type=TEXTURE_SRV);

	private:
		void TextureDescHelper();

		// After we have m_textureDesc and m_Type Allocate GPU memory and create texture
		void Initialize();

		ImageMetadata LoadFromImage(std::string& filename, unsigned char*& data);

		void CreateCBV(DescriptorTable* table=nullptr, UINT tableIndex=0) override;
		void CreateSRV(DescriptorTable* table=nullptr, UINT tableIndex=0) override;
		void CreateUAV(DescriptorTable* table=nullptr, UINT tableIndex=0) override;
		void CreateDSV(DescriptorTable* table=nullptr, UINT tableIndex=0) override;
		void CreateRTV(DescriptorTable* table=nullptr, UINT tableIndex=0) override;

	};
	*/

}