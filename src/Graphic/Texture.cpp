#include "Texture.h"

namespace Graphic {

	Texture::Texture(UINT width, UINT height, D3D12_RESOURCE_DIMENSION dim=D3D12_RESOURCE_DIMENSION_TEXTURE2D)
	{
		m_textureDesc.MipLevels = 1;
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
		const UINT bufferSize = allocateInfo.SizeInBytes;
		ptrGPUMem gpuMem = EngineGPUMemory.CreateCommittedBuffer(bufferSize);
		m_srv = new ShaderResource(gpuMem, bufferSize, descriptorHeap);
	}

	void Texture::UploadTexture(const TextureData& textureData) 
	{
		
	}

}