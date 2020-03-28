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
		D3D12_RESOURCE_FLAGS flag;
		switch (type)
		{
		case Graphic::TEXTURE_SRV:
			flag = D3D12_RESOURCE_FLAG_NONE;
			break;
		case Graphic::TEXTURE_UAV:
			flag = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
			break;
		case Graphic::TEXTURE_CBV:
			flag = D3D12_RESOURCE_FLAG_NONE;
			break;
		case Graphic::TEXTURE_DSV:
			flag = D3D12_RESOURCE_FLAG_NONE; //?
			break;
		case Graphic::TEXTURE_RTV:
			flag = D3D12_RESOURCE_FLAG_NONE;
			break;
		default:
			break;
		}

		m_textureDesc =  CD3DX12_RESOURCE_DESC::Buffer(elementSize * m_stride, flag);
		m_gpuMem = Engine::MemoryAllocator.CreateCommittedBuffer(m_textureDesc);
		CreateViews();
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

	void TextureBuffer::CreateDSV()
	{
		assert(false && "Not implementend!");
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

		if (m_Type & TEXTURE_DSV) {
			// if we are using texture as a depth buffer
			m_textureDesc.Format = DXGI_FORMAT_D32_FLOAT;

			D3D12_RESOURCE_STATES initState = D3D12_RESOURCE_STATE_DEPTH_WRITE;
			D3D12_CLEAR_VALUE clearValue = {};
			clearValue.Format = DXGI_FORMAT_D32_FLOAT;
			clearValue.DepthStencil.Depth = 1.0f;
			clearValue.DepthStencil.Stencil = 0;
			m_gpuMem = Engine::MemoryAllocator.CreateCommittedBuffer(m_textureDesc, clearValue, D3D12_HEAP_TYPE_DEFAULT, initState);
		}
		else {
			m_gpuMem = Engine::MemoryAllocator.CreateCommittedBuffer(m_textureDesc);		
		}
		

		CreateViews();
		if (m_Type & TEXTURE_DSV || m_Type & TEXTURE_RTV) {
			// I think we are not going to write these kind of views
			return;
		}
		
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

	void Texture2D::CreateDSV() {
		D3D12_DEPTH_STENCIL_VIEW_DESC  dsvDesc = {};
		// TODO format?
		dsvDesc.Format = m_textureDesc.Format;
		dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
		dsvDesc.Flags = D3D12_DSV_FLAG_NONE;

		m_DSV = new DepthStencil(m_gpuMem, dsvDesc, Engine::GetDSVHeap());
	}
}