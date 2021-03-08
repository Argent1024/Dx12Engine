#include "stdafx.h"
#include "GameInput.h"

using namespace Game;
using namespace Math;
using namespace Graphic;

void SimpleCameraControl::UpdateScene(Scene& scene) 
{	
	ProjectiveCamera& camera = scene.GetMainCamera();
	InputManager& input = Engine::GetInputManager();
	
	auto kb = input.GetKeyboardState();
	auto mouse = input.GetMouseState();

	bool ALT = kb.LeftAlt || kb.RightAlt;

	// Mouse Stuff
	// TODO: Why?
	/*if (mouse.positionMode == DirectX::Mouse::MODE_RELATIVE)
	{*/
		Vector3 delta = Vector3(float(mouse.x), float(mouse.y), 0.f) * m_MouseSpeed;

		// Zoom
		if (mouse.scrollWheelValue != 0) 
		{
			// TODO Be better to write these min,max setting in camera class
			float& fov = camera.FovY();
			float dfov = std::max(m_ZoomSpeed, fov/2.0f);

			if (mouse.scrollWheelValue > 0) {
				fov = std::max(Math::PI / 20.0f, fov - dfov);
			}
			else {
				fov = std::min(Math::PI, fov + dfov);
			}
		}

		// Rotation
		if (mouse.leftButton && ALT)
		{


		}

	//}
	// Update Camera data
	camera.Look();
}