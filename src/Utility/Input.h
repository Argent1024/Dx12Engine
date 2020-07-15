#pragma once
#include "stdafx.h"

#include "Mouse.h"
#include "Keyboard.h"

// Class that manage keyboard & mouse input
class InputManager {

public:
	void Initialize(HWND window);

	inline DirectX::Keyboard::State GetKeyboardState() const
	{
		assert(m_Initialized && "Input manager not initialized");
		return m_keyboard->GetState(); 
	}
	
	inline DirectX::Mouse::State GetMouseState() const
	{ 
		assert(m_Initialized && "Input manager not initialized");
		return m_mouse->GetState(); 
	}

private:
	bool m_Initialized = FALSE;
	std::unique_ptr<DirectX::Keyboard> m_keyboard;
	std::unique_ptr<DirectX::Mouse> m_mouse;

};


namespace Engine 
{	
	extern InputManager MainInputManager;

	inline InputManager& GetInputManager() { return MainInputManager; }
	inline void InitializeInputManager(HWND window) { MainInputManager.Initialize(window); }
}