#include "Texture.h"
#include "Utility/Logger.h"

#include <WICTextureLoader.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb/stb_image_write.h"    

namespace 
{
	// Copy from Dartmouth CS 73/273 Computational Aspects of Digital Photography C++ basecode.
	std::string getExtension(const std::string &filename)
	{
		if (filename.find_last_of(".") != std::string::npos)
			return filename.substr(filename.find_last_of(".") + 1);
		return "";
	}


}

namespace Graphic {


	void LoadChessBoard(const UINT width, const UINT height, const UINT pixelSize, std::vector<UINT8>& data)
	{
		const UINT rowPitch = width * pixelSize;
		const UINT textureSize = rowPitch * height;
		data.resize(textureSize);
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
				data[n] = 0x00;        // R
				data[n + 1] = 0x00;    // G
				data[n + 2] = 0x00;    // B
				data[n + 3] = 0xff;    // A
			}
			else
			{
				data[n] = 0xff;        // R
				data[n + 1] = 0xff;    // G
				data[n + 2] = 0xff;    // B
				data[n + 3] = 0xff;    // A
			}
			
		}
	}


	TextureBuffer::TextureBuffer(UINT elementNum, UINT stride, UINT type) 
		:Texture(type), m_elementNum(elementNum), m_stride(stride), m_totalSize(m_elementNum * m_stride)
	{
		// TODO consider flags
		D3D12_RESOURCE_FLAGS flag;
		switch (type)
		{
		case Graphic::TEXTURE_UAV:
			flag = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
			break;
		default:
			flag = D3D12_RESOURCE_FLAG_NONE;
			break;
		}

		m_buffer = GPU::MemoryManager::CreateGBuffer();
		m_buffer->Initialize(m_totalSize);
	}

	TextureBuffer::TextureBuffer(UINT totalSize) : 
		Texture(TEXTURE_CBV), m_totalSize(totalSize)
	{
		D3D12_RESOURCE_FLAGS flag = D3D12_RESOURCE_FLAG_NONE;

		m_buffer = GPU::MemoryManager::CreateGBuffer();
		m_buffer->Initialize(m_totalSize);
	}

	void TextureBuffer::CreateCBV(DescriptorTable* table, UINT tableIndex) 
	{
		/*if (!m_CBV) {
			m_CBV = new ConstantBuffer(m_buffer, m_totalSize);
		}
		if (table) {
			m_CBV->CreateView(*table, tableIndex);
		} else {
			m_CBV->CreateRootView();
		}*/
	}

	void TextureBuffer::CreateSRV(DescriptorTable* table, UINT tableIndex) 
	{
	
		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.Format = DXGI_FORMAT_UNKNOWN;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
		srvDesc.Buffer.FirstElement = 0;
		srvDesc.Buffer.NumElements = m_elementNum;
		srvDesc.Buffer.StructureByteStride = m_stride;
		srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
		
		m_SRV.CreateView(table, tableIndex, m_buffer->GetResource(), &srvDesc);
	}

	void TextureBuffer::CreateUAV(DescriptorTable* table, UINT tableIndex) 
	{
		D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
		uavDesc.Format = DXGI_FORMAT_UNKNOWN;
		uavDesc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
		uavDesc.Buffer.FirstElement = 0;
		uavDesc.Buffer.NumElements = m_elementNum;
		uavDesc.Buffer.StructureByteStride = m_stride;
		uavDesc.Buffer.CounterOffsetInBytes = 0;
		uavDesc.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_NONE;

		m_UAV.CreateView(table, tableIndex, m_buffer->GetResource(), &uavDesc);
	}

	Texture2D::Texture2D(UINT width, UINT height, UINT type, bool loadChessBoard)
		: Texture(type)
	{
		TextureDescHelper(width, height);
		Initialize();

		if (loadChessBoard) {
			std::vector<UINT8> data;
			LoadChessBoard(width, height, 4, data);
			D3D12_SUBRESOURCE_DATA textureData = 
			CreateTextureData({ (int)m_textureDesc.Width, (int)m_textureDesc.Height, 4, 4}, &data[0]);
			UploadTexture(&textureData);
		}
	}

	Texture2D::Texture2D(std::string& filename, UINT type) 
		: Texture(type)
	{	
		// Load Chess Board

		unsigned char* data = nullptr;
		
		ImageMetadata metadata = LoadFromImage(filename, data);
		metadata.channel = 4;
		metadata.pixelSize = 4;

		D3D12_SUBRESOURCE_DATA texData = CreateTextureData(metadata, data);
		TextureDescHelper(metadata.width, metadata.height);
		Initialize();
		UploadTexture(&texData);

		// Using stb_image so need to free data here
		// TODO Thinking sometime we need store the data in memory = =
		stbi_image_free(data);
		
	}
	
	void Texture2D::TextureDescHelper(UINT width, UINT height) 
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
		m_textureDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	}

	void Texture2D::Initialize() 
	{
		m_buffer = GPU::MemoryManager::CreateTBuffer();
		if (m_Type & TEXTURE_DSV) {
			// Change Texture format to D32
			m_textureDesc.Format = DXGI_FORMAT_D32_FLOAT;
			m_textureDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

			D3D12_RESOURCE_STATES initState = D3D12_RESOURCE_STATE_DEPTH_WRITE;

			// provide a clear value if we are using texture as a depth buffer
			D3D12_CLEAR_VALUE clearValue = {};
			clearValue.Format = DXGI_FORMAT_D32_FLOAT;
			clearValue.DepthStencil.Depth = 1.0f;
			clearValue.DepthStencil.Stencil = 0;

			m_buffer->Initialize(&m_textureDesc, &clearValue);
			// Change back to R32 from D32
			m_textureDesc.Format = DXGI_FORMAT_R32_FLOAT;

		} else {
			m_buffer->Initialize(&m_textureDesc);
		}
	}

	ImageMetadata Texture2D::LoadFromImage(std::string& filename, unsigned char*& data) 
	{
		// stb image only support 8 bit
		ImageMetadata metadata = {};
		data = stbi_load(filename.c_str(), &metadata.width, &metadata.height, &metadata.channel, 4);
		if (data == nullptr) {
			std::string errormsg = "ERROR when reading image " + filename;
			Logger::Log(errormsg);
		}
		metadata.pixelSize = metadata.channel; // Store pixel size in Byte
		return metadata;
		// LoadWICTextureFromFile()
	}

	void Texture2D::CreateSRV(DescriptorTable* table, UINT tableIndex)  
	{
		// Describe SRV
		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.Format = m_textureDesc.Format;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = 1;

		m_SRV.CreateView(table, tableIndex, m_buffer->GetResource(), &srvDesc);
	}

	/*void Texture2D::CreateUAV(DescriptorTable* table, UINT tableIndex)  {
		assert(FALSE && "Not implemented!");
	}*/

	void Texture2D::CreateDSV() 
	{
		// Describe SRV
		D3D12_DEPTH_STENCIL_VIEW_DESC  dsvDesc = {};
		dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
		dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
		dsvDesc.Flags = D3D12_DSV_FLAG_NONE;

		m_DSV.CreateView(m_buffer->GetResource(), &dsvDesc);
	}

	void Texture2D::CreateRTV()  
	{
		//D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = {};
		//rtvDesc.Format = m_textureDesc.Format;
		//rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
		throw std::runtime_error("Not Implemented!");
		//m_RTV = new RenderTarget(m_gpuMem, rtvDesc, Engine::GetRTVHeap())		
	}

	TextureCube::TextureCube(UINT resolution, UINT type) 
		: Texture(type)
	{
		TextureDescHelper(resolution);
		Initialize();
	}
	
	void TextureCube::TextureDescHelper(UINT resolution) 
	{
		m_textureDesc = {};
		m_textureDesc.MipLevels = 1;
		m_textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		m_textureDesc.Width = resolution;
		m_textureDesc.Height = resolution;
		m_textureDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
		m_textureDesc.DepthOrArraySize = 6; // TODO
		m_textureDesc.SampleDesc.Count = 1;
		m_textureDesc.SampleDesc.Quality = 0;
		m_textureDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE3D; // TODO is this correct?
	}

	void TextureCube::Initialize()
	{
		// m_gpuMem = Engine::MemoryAllocator.CreateCommittedBuffer(m_textureDesc);
	}

	void TextureCube::CreateSRV(DescriptorTable* table, UINT tableIndex)
	{
		
		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.Format = m_textureDesc.Format;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
		srvDesc.Texture2D.MipLevels = 1;
		
	}
	
}