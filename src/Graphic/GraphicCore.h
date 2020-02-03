#pragma once
#include <iostream>
#include "CommandQueue.h"


namespace Graphic 
{
	class GraphicCore {
	public:
		GraphicCore(UINT t_width, UINT t_height, LPCTSTR t_title):
			m_width(t_width), m_height(t_height), m_title(t_title),
			CopyCommandManager(1),
			GraphicsCommandManager(2) { }

		UINT GetWidth() const { return m_width; }
		UINT GetHeight() const {return m_height; }
		LPCTSTR GetTitle() const { return m_title; }

		virtual void Init(const HWND m_appHwnd)  = 0;
		virtual void Render() = 0;

	protected:
		void EnableDebug();
		void CreateHardwareAdapter(IDXGIFactory2* pFactory, IDXGIAdapter1** ppAdapter);
		void CreateDevice();

		ComPtr<ID3D12Device> m_device;
		ComPtr<IDXGIFactory4> m_factory;
		UINT dxgiFactoryFlags;


		// TODO maybe dont put here?
		CommandManager CopyCommandManager;
		CommandManager GraphicsCommandManager;

		const UINT m_width;
		const UINT m_height;
		const LPCTSTR m_title;

		static const UINT FrameCount = 2;
		static const bool m_useWarpDevice = FALSE;
	};
}