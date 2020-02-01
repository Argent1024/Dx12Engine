#pragma once
#include "DXHelper.h"
#include "CommandQueue.h"
#include "CommandList.h"
#include "GPUBuffer.h"
#include "Descriptor.h"
#include "PipelineState.h"
#include "SwapChain.h"
#include "../World/Camera.h"
#include "../World/Mesh.h"

namespace Graphic 
{
	using namespace DirectX;
	struct Vertex
    {
        XMFLOAT3 position;
        XMFLOAT4 color;
    };

	 struct SceneConstantBuffer
	{
		XMFLOAT4 offset;
	};

	class DXCore {
	public:
		// functions below
		DXCore(UINT t_width, UINT t_height):
			m_width(t_width), m_height(t_height),
			m_viewport(0.0f, 0.0f, static_cast<float>(t_width), static_cast<float>(t_height)),
			m_scissorRect(0, 0, static_cast<LONG>(t_width), static_cast<LONG>(t_height)),
			CopyCommandManager(1, D3D12_COMMAND_LIST_TYPE_COPY),
			GraphicsCommandManager(2)
		{
			// Init only after we have a window handle
			return;
		}

		virtual void Init(const HWND m_appHwnd);
		virtual void RecordCommandList();
		virtual void Render();

		UINT GetWidth() { return m_width; }
		UINT GetHeight() {return m_height; }

		// Data below
		static const UINT FrameCount = 2;
		static const bool m_useWarpDevice = FALSE;

		CD3DX12_VIEWPORT m_viewport;
		CD3DX12_RECT m_scissorRect;
		ComPtr<ID3D12Device> m_device;
		
		CommandList* m_commandList;
		GraphicsPSO* pso;
		ComPtr<ID3D12RootSignature> m_rootSignature;

		DescriptorHeap* cbvHeap;
		ConstantBuffer* m_ConstantBuffer;
		ptrGPUMem m_cbGPUmem;
		Game::Mesh* m_mesh;
		ptrGPUMem m_GPUmem;
		
		SwapChain* m_swapChain;
		
		const UINT m_width;
		const UINT m_height;

		CommandManager CopyCommandManager;
		CommandManager GraphicsCommandManager;

	protected:
		ComPtr<IDXGIFactory4> m_factory;

		UINT dxgiFactoryFlags;

		void CreateTriangle();
		void CreateParticle();

		void EnableDebug();
		void GetHardwareAdapter(IDXGIFactory2* pFactory, IDXGIAdapter1** ppAdapter);
		void CreateDevice();
		void CreateSwapChain(const HWND t_appHwnd);
	};
}