#include "Texture.h"

namespace Graphic {

	TextureData::TextureData(const UINT width, const UINT height, const UINT pixelSize) 
		: m_data(width * height* pixelSize)
	{
		// Set infomation to m_textureData
		m_textureData.pData = &m_data[0];
		m_textureData.RowPitch = width * pixelSize;
		m_textureData.SlicePitch = m_textureData.RowPitch * height;

		const UINT textureSize = m_textureData.SlicePitch;
		const UINT rowPitch = m_textureData.RowPitch;
		const UINT cellPitch = rowPitch >> 3;        // The width of a cell in the checkboard texture.
		const UINT cellHeight = width >> 3;    // The height of a cell in the checkerboard texture.
		// Load Chess Board
		for (UINT n = 0; n < textureSize; n += pixelSize)
		{
			UINT x = n % rowPitch;
			UINT y = n / rowPitch;
			UINT i = x / cellPitch;
			UINT j = y / cellHeight;

			if (i % 2 == j % 2)
			{
				m_data[n] = 0x00;        // R
				m_data[n + 1] = 0x00;    // G
				m_data[n + 2] = 0x00;    // B
				m_data[n + 3] = 0xff;    // A
			}
			else
			{
				m_data[n] = 0xff;        // R
				m_data[n + 1] = 0xff;    // G
				m_data[n + 2] = 0xff;    // B
				m_data[n + 3] = 0xff;    // A
			}
			
		}
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

		m_srv = new ShaderResource(gpuMem, srvDesc);
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