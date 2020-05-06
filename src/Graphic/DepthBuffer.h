#pragma once

#include "GraphicCore.h"
#include "Texture.h"

namespace Graphic 
{
	class DepthBuffer 
	{
	public:
		DepthBuffer(const UINT width, const UINT height);

		void Initialize(UINT type=TEXTURE_DSV);
		// DSV don't use the same structure as SRV&UAV&CBV 
		// Looks like don't need to bind
		//inline void BindDepthBuffer() {}

		inline CD3DX12_CPU_DESCRIPTOR_HANDLE GetDepthBufferCPUHandle() const 
		{	
			return m_dsvHandle; 
		}

		
	private:
		const UINT m_width;
		const UINT m_height;
		CD3DX12_CPU_DESCRIPTOR_HANDLE m_dsvHandle;
		Texture* m_depthTexture;
	};


}