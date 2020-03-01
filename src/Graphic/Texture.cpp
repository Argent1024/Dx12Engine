#include "Texture.h"

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


	TextureBuffer::TextureBuffer(UINT elementSize, UINT stride, TextureType type) 
		:Texture(type), m_size(elementSize), m_stride(stride)
	{
		// TODO consider flags
		D3D12_RESOURCE_FLAGS flag;;
		switch (type)
		{
		case Graphic::TEXTURE_SRV:
			flag = D3D12_RESOURCE_FLAG_NONE;
		case Graphic::TEXTURE_UAV:
			flag = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
		case Graphic::TEXTURE_RW:
			flag = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
		default:
			break;
		}

		m_textureDesc =  CD3DX12_RESOURCE_DESC::Buffer(elementSize * m_stride, flag);
		m_gpuMem = Engine::MemoryAllocator.CreateCommittedBuffer(m_textureDesc);
		CreateView();
	}

	void TextureBuffer::CreateSRV() 
	{
		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
        srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
        srvDesc.Format = DXGI_FORMAT_UNKNOWN;
        srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
        srvDesc.Buffer.FirstElement = 0;
        srvDesc.Buffer.NumElements = m_size;
        srvDesc.Buffer.StructureByteStride = m_stride;
        srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
		m_SRV = new ShaderResource(m_gpuMem, srvDesc);
	}

	void TextureBuffer::CreateUAV() 
	{
		D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
		uavDesc.Format = DXGI_FORMAT_UNKNOWN;
		uavDesc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
		uavDesc.Buffer.FirstElement = 0;
		uavDesc.Buffer.NumElements = m_size;
		uavDesc.Buffer.StructureByteStride = m_stride;
		uavDesc.Buffer.CounterOffsetInBytes = 0;
		uavDesc.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_NONE;
		m_UAV = new UnorderedAccess(m_gpuMem, uavDesc);
	}

	Texture2D::Texture2D(UINT width, UINT height, TextureType type, const std::wstring& textureFile)
		: Texture(type)
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

		m_gpuMem = Engine::MemoryAllocator.CreateCommittedBuffer(m_textureDesc);
		CreateView();

		std::vector<UINT8> data;
		LoadChessBoard(width, height, 4, data);

		D3D12_SUBRESOURCE_DATA textureData = CreateTextureData(m_textureDesc.Width, m_textureDesc.Height, 4, data);
		UploadTexture(textureData);
	}

	void Texture2D::CreateSRV() {
		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.Format = m_textureDesc.Format;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = 1;
		m_SRV = new ShaderResource(m_gpuMem, srvDesc);
	}

	void Texture2D::CreateUAV() {
		assert(FALSE && "Not implemented!");
	}
}