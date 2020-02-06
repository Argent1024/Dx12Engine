#include "Texture.h"

namespace Graphic {

	Texture::Texture(UINT width, UINT height, D3D12_RESOURCE_DIMENSION dim)
	{
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


	void Texture::CreateSRV(ComPtr<ID3D12Device> device, DescriptorHeap* descriptorHeap) 
	{
		// Whether this work...? Should rewrite GPU Memory?
		const D3D12_RESOURCE_ALLOCATION_INFO  allocateInfo = device->GetResourceAllocationInfo(0, 1, &m_textureDesc);
		const UINT64 bufferSize = allocateInfo.SizeInBytes;
		ptrGPUMem gpuMem = EngineGPUMemory.CreateCommittedBuffer(bufferSize);
		
		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
        srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
        srvDesc.Format = m_textureDesc.Format;
        srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D; //TODO emmmm
        srvDesc.Texture2D.MipLevels = 1;

		m_srv = new ShaderResource(gpuMem, bufferSize, descriptorHeap);
		m_srv->SetSRVDesc(srvDesc);
		m_srv->Initialize(device);
	}


	void Texture::UploadTexture(TextureData& textureData) 
	{
		m_srv->CopyTexture(&textureData.GetTextureData());
	}

}