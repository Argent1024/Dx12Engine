#include "WinApp.h"
#include "stdafx.h"

HWND WinApp::m_hwnd = nullptr;


void WinApp::InitWindow(HINSTANCE hInst, Graphic::GraphicCore* t_core)
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

	RECT windowRect = { 0, 0, static_cast<LONG>(t_core->GetWidth()), static_cast<LONG>(t_core->GetHeight()) };
	AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE);

	// Create the window and store a handle to it.
	m_hwnd = CreateWindow(
		windowClass.lpszClassName,
		t_core->GetTitle(),
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		windowRect.right - windowRect.left,
		windowRect.bottom - windowRect.top,
		nullptr,        // We have no parent window.
		nullptr,        // We aren't using menus.
		hInst,
		t_core);      // Passing data into call back function
	assert(m_hwnd != nullptr);

	// Init graphics core after get a handle
	t_core->Init(m_hwnd);
}

LRESULT CALLBACK WinApp::WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	Graphic::GraphicCore* t_core = reinterpret_cast<Graphic::GraphicCore*> (GetWindowLongPtr(hWnd, GWLP_USERDATA));
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
		//TODO render here
		t_core->Render();
		return 0;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

	}

	// Handle any messages switch statement didn't
	return DefWindowProc(hWnd, message, wParam, lParam);
}