#pragma once
#include "stdafx.h"

#include "Mouse.h"
#include "Keyboard.h"

// Class that manage keyboard & mouse input
class InputManager {

public:
	void Initialize(HWND window);

	inline bool IsKeyPressed(DirectX::Keyboard::Keys key) const 
	{ return m_keyTraker.IsKeyPressed(key); }

	inline bool IsKeyPressed(DirectX::Mouse) const {}

	inline bool IsKeyReleased(DirectX::Keyboard::Keys key) const 
	{ return m_keyTraker.IsKeyReleased(key); }


	inline DirectX::Keyboard::State GetKeyboardState() const
	{
		assert(m_Initialized && "Input manager not initialized");
		return m_KeyboardState; 
	}
	
	inline DirectX::Mouse::State GetMouseState() const
	{ 
		assert(m_Initialized && "Input manager not initialized");
		return m_MouseState; 
	}

	inline void Update() { 
		m_KeyboardState = m_keyboard->GetState();
		m_MouseState = m_mouse->GetState();

		m_keyTraker.Update(m_KeyboardState); 
		m_mouseButtons.Update(m_MouseState);
	}

	inline void Reset() {
		m_keyTraker.Reset();
		m_mouseButtons.Reset();
	}

private:
	bool m_Initialized = FALSE;
	DirectX::Keyboard::KeyboardStateTracker m_keyTraker;
	DirectX::Mouse::ButtonStateTracker m_mouseButtons;

	DirectX::Keyboard::State m_KeyboardState;
	DirectX::Mouse::State m_MouseState;

	std::unique_ptr<DirectX::Keyboard> m_keyboard;
	std::unique_ptr<DirectX::Mouse> m_mouse;

};


namespace Engine 
{	
	extern InputManager MainInputManager;

	inline InputManager& GetInputManager() { return MainInputManager; }
	inline void InitializeInputManager(HWND window) { MainInputManager.Initialize(window); }
}