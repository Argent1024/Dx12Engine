#include "stdafx.h"
#include "Input.h"
#include "Logger.h"

using namespace DirectX;
void InputManager::Initialize(HWND window) 
{
	m_Initialized = TRUE;
	m_keyboard = std::make_unique<Keyboard>();
	m_mouse = std::make_unique<Mouse>();
	m_mouse->SetWindow(window);
	Logger::Log("Input Manager initialized");
}

namespace Engine {
	InputManager MainInputManager;

}