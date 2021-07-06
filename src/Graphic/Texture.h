#pragma once
#include "Descriptor.h"
#include "GPUMemManager.h"
#include "PipelineState.h"
#include "RootSignature.h"

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
		UINT width;
		UINT height;
		UINT channel;
		UINT channelSize;	// In Byte
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
		// Use a compute shader to create mip levels of this texture
		static void CreateMipMap(ptrComputePSO pso, ptrTexture texture);
		static ptrRootSignature MipMapRootSignature;
		static ptrComputePSO MipMapPSO;

		Texture(UINT type) : m_Type(type) {
			assert(type != 0 && " At least one type is needed Are you using && instead of || when creating type?");
			assert(!((type & TEXTURE_RTV) && (type & TEXTURE_DSV)) && "Creating RTV & DSV same time");
		}


		inline UINT GetType() { return m_Type; }
			
		// Create 2d texture data (helper method for stb_image, so don't need to copy the image again)
		static D3D12_SUBRESOURCE_DATA CreateTextureData(
			const ImageMetadata& metadata, 
			const unsigned char* data) 
		{
			D3D12_SUBRESOURCE_DATA textureData = {};
			// Set infomation to m_textureData
			textureData.pData = data;
			textureData.RowPitch = metadata.width * metadata.channel * metadata.channelSize;
			textureData.SlicePitch = metadata.height;
			return textureData;
		}

		/*
		// TODO Need this when writing image back to disk
		static ImageMetadata CalculateMetaData() 
		{
			
		} */

		virtual void UploadTexture(D3D12_SUBRESOURCE_DATA* data) 
		{
			assert(FALSE && "Not implemented");
		}


		// Create the SRV & UAV at the table at tableIndex
		void CreateCBV(DescriptorTable* table=nullptr, UINT tableIndex=0) { assert(FALSE && "CBV not implemented!"); }
		void CreateSRV(DescriptorTable* table=nullptr, UINT tableIndex=0) { assert(FALSE && "SRV not implemented!"); }
		void CreateUAV(DescriptorTable* table=nullptr, UINT tableIndex=0) { assert(FALSE && "UAV not implemented!"); }
		void CreateDSV() { assert(FALSE && "DSV not implemented!"); }
		void CreateRTV() { assert(FALSE && "RTV not implemented!"); }

	protected:
		// Store or value for texture_type
		// EX: m_Type = TEXUTRE_SRV | TEXTURE_UAV | TEXTURE_DSV
		const UINT m_Type;

		CD3DX12_RESOURCE_DESC m_textureDesc;
	};

	class TextureSingle : public Texture {
	public:
		TextureSingle(UINT type) : Texture(type) { }

		const DepthStencilView& DSV() const { return m_DSV; }
		DepthStencilView& DSV() { return m_DSV; }
		

	protected:
		// ConstantBuffer  m_CBV;
		ShaderResourceView  m_SRV;
		UnorderedAccessView m_UAV;
		RenderTargetView    m_RTV;
		DepthStencilView    m_DSV;
	};

	/*template<UINT N>
	class TextureArray : public Texture 
	{
	public:
		TextureArray(UINT type) : Texture(type) { }
	private:

	};*/


	// ******************************************************************************************** //
	// ************************************ Implementation below ********************************** //
	// ******************************************************************************************** //
	
	/*
	class Texture1D : public TextureSingle {
	public:
		// TODO better way to express type?
		Texture1D(UINT elementNum, UINT stride, UINT type=TEXTURE_SRV);

		// Just Create a simple CBV
		Texture1D(UINT totalSize);

	private:
		void CreateCBV(DescriptorTable* table=nullptr, UINT tableIndex=0) override;
		void CreateSRV(DescriptorTable* table=nullptr, UINT tableIndex=0) override;
		void CreateUAV(DescriptorTable* table=nullptr, UINT tableIndex=0) override;

		ptrGBuffer m_buffer;

		UINT m_elementNum;
		UINT m_stride;
		UINT m_totalSize;
	};*/

	// TODO dont use stb_image, only support 8-bit's channel
	class Texture2D : public TextureSingle {
	public:
		static const UINT16 ArraySize = 1;

		// TODO better way to express type?
		Texture2D(UINT width, UINT height, UINT type=TEXTURE_SRV, 
			DXGI_FORMAT format=DXGI_FORMAT_R8G8B8A8_UNORM, bool loadChessBoard=false);

		Texture2D(std::string& filename, UINT type=TEXTURE_SRV);

		// Write texture data to gpu memory
		// Only need to upload once since all views point to the same memory!
		inline void UploadTexture(D3D12_SUBRESOURCE_DATA* data) override {
			GPU::MemoryManager::UploadTexure(*m_buffer, data);
		}

	private:
		ptrTBuffer m_buffer;

		// void TextureDescHelper(UINT width, UINT height, UINT16 miplevels, DXGI_FORMAT format);

		// After we have m_textureDesc and m_Type Allocate GPU memory and create texture
		void Initialize(CD3DX12_RESOURCE_DESC& texdesc);

		ImageMetadata LoadFromImage(std::string& filename, unsigned char*& data);

		//void CreateCBV(DescriptorTable* table=nullptr, UINT tableIndex=0) override;
		void CreateSRV(DescriptorTable* table=nullptr, UINT tableIndex=0) override;
		//void CreateUAV(DescriptorTable* table=nullptr, UINT tableIndex=0) override;
		void CreateDSV() override;
		void CreateRTV() override;
	};
	
	// Cube map helper
	class TextureCube : public Texture
	{
	public:
		TextureCube(UINT resolution, UINT16 miplevels, UINT type=TEXTURE_SRV);

	private:
		void TextureDescHelper(UINT resolution, UINT16 miplevels);

		// After we have m_textureDesc and m_Type Allocate GPU memory and create texture
		void Initialize();

		// ImageMetadata LoadFromImage(std::string& filename, unsigned char*& data);

		void CreateSRV(DescriptorTable* table=nullptr, UINT tableIndex=0) override;
		// void CreateUAV
		void CreateRTV() override;

		ptrTBuffer m_buffer;
		ShaderResourceView m_SRV;
		RenderTargetView m_RTV;
	};


}