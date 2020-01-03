#pragma once
#include "DXHelper.h"

namespace Graphic 
{

	class DXCore {
	public:
		// functions below
		DXCore(UINT t_width, UINT t_height):
			m_width(t_width), m_height(t_height),
			m_viewport(0.0f, 0.0f, static_cast<float>(t_width), static_cast<float>(t_height)),
			m_scissorRect(0, 0, static_cast<LONG>(t_width), static_cast<LONG>(t_height))
		{
			// Init only after we have a window handle
			return;
		}

		void InitPipeline(const HWND t_appHwnd);
		void Init(const HWND m_appHwnd);

		void WaitFrame();
		void WaitGPU();

		UINT GetWidth() { return m_width; }
		UINT GetHeight() {return m_height; }

		// Data below
		static const UINT FrameCount = 2;
		static const bool m_useWarpDevice = FALSE;

		CD3DX12_VIEWPORT m_viewport;
		CD3DX12_RECT m_scissorRect;
		ComPtr<ID3D12Device> m_device;
		ComPtr<ID3D12CommandQueue> m_commandQueue;
		ComPtr<IDXGISwapChain3> m_swapChain;
		ComPtr<ID3D12DescriptorHeap> m_rtvHeap;
		ComPtr<ID3D12Resource> m_renderTargets[FrameCount];
		ComPtr<ID3D12CommandAllocator> m_commandAllocator;
		UINT m_rtvDescriptorSize;


		// Synchronization objects.
		UINT m_frameIndex;
		HANDLE m_fenceEvent;
		ComPtr<ID3D12Fence> m_fence;
		UINT64 m_fenceValues[FrameCount];


		const UINT m_width;
		const UINT m_height;
		

	protected:
		ComPtr<IDXGIFactory4> m_factory;

		UINT dxgiFactoryFlags;

		

		void EnableDebug();
		void GetHardwareAdapter(IDXGIFactory2* pFactory, IDXGIAdapter1** ppAdapter);
		void CreateDevice();
		void CreateQueue();
		void CreateSwapChain(const HWND t_appHwnd);
		void CreateDescriptorHeap();
		void CreateRTV();
	};
}