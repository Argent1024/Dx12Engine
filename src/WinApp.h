#pragma once

#include "stdafx.h"
#include "World/Engine.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

class WinApp {
public:
	WinApp() {}

	static HWND GetHwnd() { return m_hwnd; }

	static int RunApp(Engine::GameEngine* t_engine,HINSTANCE hInst, int nCmdShow) {
		// Init Window
		InitWindow(hInst, t_engine);
		// Init GameApp

		ShowWindow(m_hwnd, nCmdShow);


		// Main loop.
		MSG msg = {};
		while (msg.message != WM_QUIT)
		{
			// Process any messages in the queue.
			if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			t_engine->GameLoop();
			// ImGui::EndFrame();
		}

		// Return this part of the WM_QUIT message to Windows.
		return static_cast<char>(msg.wParam);
		
	}

protected:

	static void InitWindow(HINSTANCE hInst,Engine::GameEngine* t_engine);

	static LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	static HWND m_hwnd;

};