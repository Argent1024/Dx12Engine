#include "Camera.h"

namespace Game {
	void ProjectiveCamera::UseCamera(Graphic::CommandList& commandList) {
		commandList.SetViewPorts(&m_Viewport);
		commandList.SetScissorRects(&m_ScissorRect);
		//TODO Camera space transform
		// Root Signature is used for projective camera should use b0 as the view matrix
		commandList.SetGraphicsRootConstants(0, sizeof(DirectX::XMMATRIX) / 4,  &(DirectX::XMMATRIX)m_ViewProjective);
	}
}