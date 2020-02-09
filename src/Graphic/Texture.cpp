#include "Texture.h"

namespace Graphic {

	TextureData::TextureData(const UINT width, const UINT height, const UINT pixelSize) 
		: m_data(width * height* pixelSize, 0)
	{	
		// Set infomation to m_textureData
		m_textureData.pData = &m_data[0];
		m_textureData.RowPitch = width * pixelSize;
		m_textureData.SlicePitch = m_textureData.RowPitch * height;
	}

	void TextureData::LoadTexture() { 
		// Do load texture to m_data
			

	}


	Texture::Texture(UINT width, UINT height, D3D12_RESOURCE_DIMENSION dim)
	{
		m_textureDesc = {};
		m_textureDesc.MipLevels = 1;
		m_textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		m_textureDesc.Width = width;
		m_textureDesc.Height = height;
		m_textureDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
		m_textureDesc.DepthOrArraySize = 1;
		m_textureDesc.SampleDesc.Count = 1;
		m_textureDesc.SampleDesc.Quality = 0;
		m_textureDesc.Dimension = dim;
	}


	void Texture::CreateSRV() 
	{	
		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
        srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
        srvDesc.Format = m_textureDesc.Format;
        srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D; //TODO emmmm
        srvDesc.Texture2D.MipLevels = 1;

		ptrGPUMem gpuMem = Engine::MemoryAllocator.CreateCommittedBuffer(m_textureDesc);

		m_srv = new ShaderResource(gpuMem);
		m_srv->SetSRVDesc(srvDesc);
		m_srv->Initialize();
	}


	void Texture::UploadTexture(const TextureData& textureData) 
	{
		m_srv->CopyTexture(&textureData.GetTextureData());
	}

	void Texture::CreateTexture() 
	{	
		CreateSRV();
		TextureData tdata(m_textureDesc.Width, m_textureDesc.Height);
		tdata.LoadTexture();
		UploadTexture(tdata);
	}


}