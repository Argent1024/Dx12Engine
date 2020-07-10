#pragma once

#include "GraphicCore.h"
#include "Texture.h"

namespace Graphic 
{
	class DepthBuffer 
	{
	public:
		DepthBuffer(const UINT width, const UINT height, UINT type=TEXTURE_DSV);

		void Initialize();

		// DSV don't use the same structure as SRV&UAV&CBV 
		// Looks like don't need to bind
		//inline void BindDepthBuffer() {}

		inline CD3DX12_CPU_DESCRIPTOR_HANDLE GetDepthBufferCPUHandle() const 
		{	
			return m_dsvHandle; 
		}
		
		
		// If we are going to create SRV in some place
		inline Texture& GetTexture() { return m_depthTexture; }
		
	private:
		const UINT m_width;
		const UINT m_height;
		CD3DX12_CPU_DESCRIPTOR_HANDLE m_dsvHandle;
		Texture2D m_depthTexture;
	};


}