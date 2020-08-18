#pragma once

#include "DXHelper.h"

// TODO change name of this file

namespace Graphic 
{
	class DescriptorHeap;

	// TODO Rename this
	class GraphicCore {
	public:
		GraphicCore(UINT t_width, UINT t_height, LPCTSTR t_title):
			m_width(t_width), m_height(t_height), m_title(t_title) { }

		UINT GetWidth() const { return m_width; }
		UINT GetHeight() const {return m_height; }
		LPCTSTR GetTitle() const { return m_title; }

		virtual void Init(const HWND m_appHwnd)  = 0;
		virtual void Render() = 0;
		virtual void Update() { };
	protected:

		const UINT m_width;
		const UINT m_height;
		const LPCTSTR m_title;

		static const UINT FrameCount = 2;
		static const bool m_useWarpDevice = FALSE;
	};
}


namespace Engine 
{
	
	extern ComPtr<ID3D12Device> dxDevice;
	extern ComPtr<IDXGIFactory4> dxFactory;
	extern UINT dxgiFactoryFlags;
	extern UINT NumDescriptors;
	extern Graphic::DescriptorHeap InitHeap;
	extern Graphic::DescriptorHeap InUseHeap;
	extern Graphic::DescriptorHeap DSVHeap;
	extern Graphic::DescriptorHeap RTVHeap;

	inline ID3D12Device* GetDevice() { return dxDevice.Get(); }
	inline IDXGIFactory4* GetFactory() { return dxFactory.Get(); }
	void EnableDebug();
	void CreateHardwareAdapter(IDXGIFactory2* pFactory, IDXGIAdapter1** ppAdapter);
	void CreateDevice();
	void InitializeEngine();
}