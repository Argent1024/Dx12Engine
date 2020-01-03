#pragma once

#include "DXHelper.h"
#include <DirectXMath.h>
#include "DXCore.h"

namespace Graphic {

	using namespace DirectX;

	struct Vertex
    {
        XMFLOAT3 position;
        XMFLOAT4 color;
    };

	class Shader {
		/* Store Shader used in render process */
	public:
		Shader(const DXCore& pDXCore, const std::wstring& pName,
			const std::wstring& pVertexPath, const std::wstring& pPixelPath)
			:m_DXCore(pDXCore), m_Name(pName), 
			m_VertexPath(pVertexPath), m_PixelPath(pPixelPath)
		{
			Init();
		}

		std::wstring GetName() const { return m_Name; }

		void Render() {
			 // Execute the command list.
			ID3D12CommandList* ppCommandLists[] = { m_commandList.Get() };
			m_DXCore.m_commandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

			 // Present the frame.
			ThrowIfFailed(m_DXCore.m_swapChain->Present(0, 0));
		}

		void Init();
		void InitCommandList();

		ID3D12CommandList* GetCommandList() { return m_commandList.Get(); }
		ID3DBlob* GetVertex() { return m_VertexShader.Get(); }
		ID3DBlob* GetPixel() { return m_PixelShader.Get(); }
		
	private:

		void virtual CreatePipeLineState();
		void virtual CreateCommandAllocator();
		void virtual CreateCommandList();

		const std::wstring m_Name;
		const std::wstring m_VertexPath;
		const std::wstring m_PixelPath;

		const DXCore& m_DXCore;
		ComPtr<ID3D12RootSignature> m_rootSignature;
		ComPtr<ID3DBlob> m_VertexShader;
		ComPtr<ID3DBlob> m_PixelShader;
		ComPtr<ID3D12PipelineState> m_pipelineState;
		ComPtr<ID3D12GraphicsCommandList> m_commandList;
		ComPtr<ID3D12CommandAllocator> m_commandAllocator;

		 // App resources.
		ComPtr<ID3D12Resource> m_vertexBuffer;
		D3D12_VERTEX_BUFFER_VIEW m_vertexBufferView;
	};

}