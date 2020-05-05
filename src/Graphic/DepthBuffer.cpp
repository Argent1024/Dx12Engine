#include "DepthBuffer.h"
#include "Texture.h"

namespace Graphic 
{
	DepthBuffer::DepthBuffer(const UINT width, const UINT height)
		: m_width(width), m_height(height)
	{ 
	}

	void DepthBuffer::Initialize() 
	{
		m_depthTexture = new Texture2D(m_width, m_height, TEXTURE_DSV);
		m_dsvHandle = m_depthTexture->GetDepthStencilView()->GetDSVCPUHandle();
	}
}