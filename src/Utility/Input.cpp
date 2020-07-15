#include "Input.h"

using namespace DirectX;
void InputManager::Initialize(HWND window) 
{
	m_Initialized = TRUE;
	m_keyboard = std::make_unique<Keyboard>();
	m_mouse = std::make_unique<Mouse>();
	m_mouse->SetWindow(window);
}

namespace Engine {
	InputManager MainInputManager;

}