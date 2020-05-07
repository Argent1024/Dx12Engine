#include "DepthBuffer.h"
#include "Texture.h"

namespace Graphic 
{
	DepthBuffer::DepthBuffer(const UINT width, const UINT height)
		: m_width(width), m_height(height)
	{ 
	}

	void DepthBuffer::Initialize(UINT type) 
	{
		assert(type & TEXTURE_DSV && "Should include dsv when creating depth buffer");
		m_depthTexture = new Texture2D(m_width, m_height, type);
		//  TODO TEST
		m_depthTexture->CreateView(TEXTURE_DSV);
		m_dsvHandle = m_depthTexture->GetDepthStencilView()->GetDSVCPUHandle();
	}
}