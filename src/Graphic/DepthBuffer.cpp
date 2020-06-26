#include "DepthBuffer.h"
#include "Texture.h"

namespace Graphic 
{
	DepthBuffer::DepthBuffer(const UINT width, const UINT height, UINT type)
		: m_width(width), m_height(height), m_depthTexture(width, height, type)
	{ 
		assert(type & TEXTURE_DSV && "Should include dsv when creating depth buffer");
	}

	void DepthBuffer::Initialize() 
	{
		//  TODO TEST
		m_depthTexture.CreateView(TEXTURE_DSV);
		m_dsvHandle = m_depthTexture.GetDepthStencilView()->GetDSVCPUHandle();
	}
}