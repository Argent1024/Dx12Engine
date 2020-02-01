#include "Camera.h"

namespace Game {
	void Camera::UseCamera(Graphic::CommandList& commandList) {
		commandList.SetViewPorts(&m_Viewport);
		commandList.SetScissorRects(&m_ScissorRect);
	}


}