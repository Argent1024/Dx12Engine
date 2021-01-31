#pragma once
#include "GPUBuffer.h"

class Graphic::CommandList;

namespace GPU 
{

	// TODO need to change all these somehow
	class MemoryManager 
	{
	public:
		// TODO need to check memory usage / page(...?)
		static ptrGBuffer CreateGBuffer() 
		{
			ptrGBuffer buffer = std::make_shared<GPU::GPUBuffer>();
			return buffer;
		}

		static ptrTBuffer CreateTBuffer()
		{
			ptrTBuffer buffer = std::make_shared<GPU::TextureBuffer>();
			return buffer;
		}

		static void UploadData(GPUBuffer& dest, const void* data, UINT dataSize, UINT offset);

		// TODO still need to change this later
		static void UploadTexure(TextureBuffer& dest, D3D12_SUBRESOURCE_DATA* textureData);

	private:

		static void _CopyBuffer(GPUBuffer& dest, GPUBuffer& src);
	};



}
