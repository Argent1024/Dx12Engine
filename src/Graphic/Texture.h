#pragma once
#include "Descriptor.h"
#include "GPUMemManager.h"
#include "PipelineState.h"
#include "RootSignature.h"

#define ptrTexture std::shared_ptr<Graphic::Texture>
#define ptrTex2D std::shared_ptr<Graphic::Texture2D>

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

		// TODO write back texture to disk


		void UploadTexture(D3D12_SUBRESOURCE_DATA* data) 
		{
			GPU::MemoryManager::UploadTexure(*m_buffer, data);
		}

		const DepthStencilView& DSV() const { return m_DSV; }

	protected:
		// Create the SRV & UAV at the table at tableIndex
		void _CreateCBV(DescriptorTable* table, UINT tableIndex) 
		{ assert(FALSE && "CBV not implemented!"); }

		void _CreateSRV(const D3D12_SHADER_RESOURCE_VIEW_DESC* srvDesc, DescriptorTable* table, UINT tableIndex) 
		{ 
			assert(m_Type & TEXTURE_SRV && "Texture type doesn't have srv"); 
			m_SRV.CreateView(table, tableIndex, m_buffer->GetResource(), srvDesc);
		}

		void _CreateUAV(const D3D12_UNORDERED_ACCESS_VIEW_DESC* uavDesc, DescriptorTable* table, UINT tableIndex) 
		{ 
			assert(m_Type & TEXTURE_UAV && "Texture type doesn't have uav"); 
			m_UAV.CreateView(table, tableIndex, m_buffer->GetResource(), uavDesc);
		}

		void _CreateDSV(const D3D12_DEPTH_STENCIL_VIEW_DESC* dsvDesc) 
		{
			assert(m_Type & TEXTURE_DSV && "Texture type doesn't have dsv"); 
			m_DSV.CreateView(m_buffer->GetResource(), dsvDesc);
		}

		void _CreateRTV() { assert(FALSE && "RTV not implemented!"); }

		// Store or value for texture_type
		// EX: m_Type = TEXUTRE_SRV | TEXTURE_UAV | TEXTURE_DSV
		const UINT m_Type;

		CD3DX12_RESOURCE_DESC m_textureDesc;

		ptrTBuffer m_buffer;
		ShaderResourceView  m_SRV;
		UnorderedAccessView m_UAV;
		RenderTargetView    m_RTV;
		DepthStencilView    m_DSV;
	};


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
		ptrGBuffer m_buffer;

		UINT m_elementNum;
		UINT m_stride;
		UINT m_totalSize;
	};*/

	// TODO dont use stb_image, only support 8-bit's channel
	class Texture2D : public Texture {
	public:
		// Use a compute shader to create mip levels of this texture
		static void CreateMipMap(ptrComputePSO pso, ptrTex2D texture);


		static const UINT16 ArraySize = 1;

		// TODO better way to express type?
		Texture2D(UINT width, UINT height, UINT type=TEXTURE_SRV, 
			DXGI_FORMAT format=DXGI_FORMAT_R8G8B8A8_UNORM, bool loadChessBoard=false);

		Texture2D(std::string& filename, UINT type=TEXTURE_SRV);


		void CreateSRV(DescriptorTable* table=nullptr, UINT tableIndex=0, UINT MostDetailedMip=0, UINT MipLevels=1);

		void CreateUAV(DescriptorTable* table=nullptr, UINT tableIndex=0, UINT MipLevels=0);

		void CreateDSV();

	private:
		// After we have m_textureDesc and m_Type Allocate GPU memory and create texture
		void Initialize(CD3DX12_RESOURCE_DESC& texdesc);

		ImageMetadata LoadFromImage(std::string& filename, unsigned char*& data);
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

		ptrTBuffer m_buffer;
	};


}