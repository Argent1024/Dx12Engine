#include "Texture.h"

namespace Graphic {

	TextureData::TextureData(const UINT size) 
		: m_data(size)
	{
		m_textureData.pData = &m_data[0];
		m_textureData.RowPitch = size;
		m_textureData.SlicePitch = size;
	}

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

	TextureBuffer::TextureBuffer(UINT size, UINT stride, TextureType type) 
		:Texture(type), m_size(size), m_stride(stride)
	{
		// TODO consider flags
		D3D12_RESOURCE_FLAGS flag;
		switch (type)
		{
		case Graphic::TEXTURE_SRV:
			flag = D3D12_RESOURCE_FLAG_NONE;
		case Graphic::TEXTURE_UAV:
			flag = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
		default:
			break;
		}
		m_textureDesc =  CD3DX12_RESOURCE_DESC::Buffer(size, flag);
		ptrGPUMem gpuMem = Engine::MemoryAllocator.CreateCommittedBuffer(m_textureDesc);

		switch (m_Type)
		{
		case Graphic::TEXTURE_SRV:
			CreateSRV();
			break;
		case Graphic::TEXTURE_UAV:
			CreateUAV();
			break;
		default:
			throw std::runtime_error("Wrong Texture type");
			break;
		}	

	}

	void TextureBuffer::CreateSRV() {}

	void TextureBuffer::CreateUAV() {
		ptrGPUMem gpuMem = Engine::MemoryAllocator.CreateCommittedBuffer(m_textureDesc);
		D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
		uavDesc.Format = DXGI_FORMAT_UNKNOWN;
		uavDesc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
		uavDesc.Buffer.FirstElement = 0;
		uavDesc.Buffer.NumElements = m_size;
		uavDesc.Buffer.StructureByteStride = m_stride;
		uavDesc.Buffer.CounterOffsetInBytes = 0;
		uavDesc.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_NONE;
		m_View = new UnorderedAccess(gpuMem, uavDesc);
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


		switch (m_Type)
		{
		case Graphic::TEXTURE_SRV:
			CreateSRV();
			break;
		case Graphic::TEXTURE_UAV:
			CreateUAV();
			break;
		default:
			throw std::runtime_error("Wrong Texture type");
			break;
		}	
		TextureData tdata(m_textureDesc.Width, m_textureDesc.Height);
		UploadTexture(tdata);
	}

	void Texture2D::CreateSRV() {
		ptrGPUMem gpuMem = Engine::MemoryAllocator.CreateCommittedBuffer(m_textureDesc);
		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.Format = m_textureDesc.Format;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = 1;
		m_View = new ShaderResource(gpuMem, srvDesc);
	}

	void Texture2D::CreateUAV() {
		assert(FALSE && "Not implemented!");
	}
}