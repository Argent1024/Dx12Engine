#pragma once
#include "DXHelper.h"
#include "CommandQueue.h"
#include "CommandList.h"
#include "GPUBuffer.h"
#include "PipelineState.h"

namespace Graphic 
{
	using namespace DirectX;
	struct Vertex
    {
        XMFLOAT3 position;
        XMFLOAT4 color;
    };

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

		void Init(const HWND m_appHwnd);
		void RecordCommandList();
		void Render();

		UINT GetWidth() { return m_width; }
		UINT GetHeight() {return m_height; }

		// Data below
		static const UINT FrameCount = 2;
		static const bool m_useWarpDevice = FALSE;

		CD3DX12_VIEWPORT m_viewport;
		CD3DX12_RECT m_scissorRect;
		ComPtr<ID3D12Device> m_device;
		
		// Temp
		CommandQueue* m_commandQueue;
		CommandList* m_commandList;
		GraphicsPSO* pso;
		ComPtr<ID3D12RootSignature> m_rootSignature;

		/*ComPtr<ID3D12Resource> m_vertexBuffer;
		D3D12_VERTEX_BUFFER_VIEW m_vertexBufferView;*/
		VertexGPUBuffer* m_vertexBuffer;

		ComPtr<IDXGISwapChain3> m_swapChain;
		ComPtr<ID3D12DescriptorHeap> m_rtvHeap;
		ComPtr<ID3D12Resource> m_renderTargets[FrameCount];
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
		void CreateSwapChain(const HWND t_appHwnd);
		void CreateDescriptorHeap();
		void CreateRTV();
	};
}