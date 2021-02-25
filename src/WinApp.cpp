#include "stdafx.h"
#include "WinApp.h"


#include "Mouse.h"
#include "Keyboard.h"

HWND WinApp::m_hwnd = nullptr;

using namespace DirectX;
using namespace Engine;

void WinApp::InitWindow(HINSTANCE hInst, Engine::GameEngine* t_engine)
{
	//TODO Parse the command line parameters
	//int argc;
	//LPWSTR* argv = CommandLineToArgvW(GetCommandLineW(), &argc);
	//LocalFree(argv);

	// Initialize the window class.
	WNDCLASSEX windowClass = { 0 };
	windowClass.cbSize = sizeof(WNDCLASSEX);
	windowClass.style = CS_HREDRAW | CS_VREDRAW;
	windowClass.lpfnWndProc = WindowProc;
	windowClass.hInstance = hInst;
	windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	windowClass.lpszClassName = L"WinAppClass";
	RegisterClassEx(&windowClass);

	const GameEngine::GameConfiguration GameSetting = t_engine->GetSetting();
	RECT windowRect = { 0, 0, static_cast<LONG>(GameSetting.Width), static_cast<LONG>(GameSetting.Height) };
	AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE);

	// Create the window and store a handle to it.
	m_hwnd = CreateWindow(
		windowClass.lpszClassName,
		GameSetting.Title,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		windowRect.right - windowRect.left,
		windowRect.bottom - windowRect.top,
		nullptr,        // We have no parent window.
		nullptr,        // We aren't using menus.
		hInst,
		t_engine);      // Passing data into call back function
	assert(m_hwnd != nullptr);

	// Init graphics core after get a handle
	t_engine->Initialize(m_hwnd);
}

LRESULT CALLBACK WinApp::WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	 GameEngine* t_engine = reinterpret_cast<GameEngine*> (GetWindowLongPtr(hWnd, GWLP_USERDATA));
	switch (message) 
	{
	case WM_CREATE: 
	   {
		// Save the shared_ptr<t_game> passed in to CreateWindow
		LPCREATESTRUCT pCreateStruct = reinterpret_cast<LPCREATESTRUCT>(lParam);
		SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pCreateStruct->lpCreateParams));
	   }
	   return 0;

	case WM_PAINT:
		t_engine->GameLoop();
		return 0;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

	//************************** INPUT BELOW ***************************//
	case WM_ACTIVATEAPP:

		Keyboard::ProcessMessage(message, wParam, lParam);
		//Mouse::ProcessMessage(message, wParam, lParam);
		break;

	case WM_MOUSEHOVER:
		Mouse::ProcessMessage(message, wParam, lParam);
		break;

	case WM_KEYDOWN:
    case WM_KEYUP:
	case WM_SYSKEYUP:	// Alt
		Keyboard::ProcessMessage(message, wParam, lParam);
		break;

	case WM_SYSKEYDOWN:
		Keyboard::ProcessMessage(message, wParam, lParam);
    /*if (wParam == VK_RETURN && (lParam & 0x60000000) == 0x20000000)
    {
        ...
    }*/
		break;
	// ************************* INPUT  END  ****************************//
	}
	
	Mouse::ProcessMessage(message, wParam, lParam);
	// Handle any messages switch statement didn't
	return DefWindowProc(hWnd, message, wParam, lParam);
}