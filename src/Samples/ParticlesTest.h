#pragma once
#include "GraphicCore.h"
#include "CommandQueue.h"
#include "../World/Camera.h"
#include "../World/GObject.h"


namespace Samples {

	using namespace Graphic;
	using namespace Game;

	struct PointVertex
    {
        XMFLOAT4 color;
    };

	struct ParticleData
	{
		XMFLOAT4 position;
	};
	

	class ParticleTestPSO : public GraphicsPSO 
	{
	public:
		void Initialize() override
		{
			// Make Sure we did set a root signature
			assert(m_rootSignature != nullptr);
			m_psoDesc.pRootSignature = m_rootSignature;

			const std::wstring ShaderPath=L"D:\\work\\tEngine\\Shaders\\particletest.hlsl";
			/*const std::string vs = "VSParticleDraw";
			const std::string gs = "GSParticleDraw";
			const std::string ps = "PSParticleDraw";

			SetVertexShader(ShaderPath, vs);
			SetGeometryShader(ShaderPath, gs);
			SetPixelShader(ShaderPath, ps);*/


			ComPtr<ID3DBlob> VS;
			ComPtr<ID3DBlob> GS;
			ComPtr<ID3DBlob> PS;			

			ThrowIfFailed(D3DCompileFromFile(ShaderPath.c_str(), nullptr, nullptr, "VSParticleDraw", "vs_5_0", CompileFlags, 0, &VS, nullptr));
			ThrowIfFailed(D3DCompileFromFile(ShaderPath.c_str(), nullptr, nullptr, "GSParticleDraw", "gs_5_0", CompileFlags, 0, &GS, nullptr));
			ThrowIfFailed(D3DCompileFromFile(ShaderPath.c_str(), nullptr, nullptr, "PSParticleDraw", "ps_5_0", CompileFlags, 0, &PS, nullptr));
			this->SetVertexShader(CD3DX12_SHADER_BYTECODE(VS.Get()));
			this->SetGeometryShader(CD3DX12_SHADER_BYTECODE(GS.Get()));
			this->SetPixelShader(CD3DX12_SHADER_BYTECODE(PS.Get()));


			// Input layout for vertex 
			D3D12_INPUT_ELEMENT_DESC inputElementDescs[] =
			{
				{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
			};

			this->SetTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT);
			this->SetInoutLayout(_countof(inputElementDescs), inputElementDescs);

			CD3DX12_DEPTH_STENCIL_DESC depthStencilDesc(D3D12_DEFAULT);
			depthStencilDesc.DepthEnable = FALSE;
			depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
			this->SetDepthStencilState(depthStencilDesc);

			CD3DX12_BLEND_DESC blendDesc(D3D12_DEFAULT);
			blendDesc.RenderTarget[0].BlendEnable = TRUE;
			blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
			blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
			blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ZERO;
			blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
			this->SetBlendState(blendDesc);

			this->SetRasterState();
			
			//lol
			SetStuffThatIdontKnowYet();

			this->CreatePSO();
		}
	
	};


	class ParticleTest : public GraphicCore {
	public:
		const size_t NumRow = 15;//Particle in one row
		const size_t NumParticles = NumRow * NumRow * NumRow;
		const float RowLength = 0.5;
		const float Delta = 2.0f * RowLength / NumRow;
		const XMFLOAT4 LeftCorner = XMFLOAT4(-RowLength, -RowLength, -RowLength, 0.0);

		ParticleTest(UINT t_width, UINT t_height,  LPCTSTR t_title=L"playground") 
			: GraphicCore(t_width, t_height, t_title), 
			  m_Camera(t_width, t_height, Vector3(0.0, 0.0, 5.0), Vector3(0.0, 0.0, 0.0), Vector3(0.0, 1.0, 0.0)) {}

		void Init(const HWND m_appHwnd) override;
		void Render() override;

		inline XMFLOAT4 GetPosition(UINT index) 
		{
			UINT x = index / (NumRow * NumRow);
			UINT y = index / NumRow - x * NumRow;
			UINT z = index % NumRow;
			XMFLOAT4 Pos(Delta * x - RowLength, Delta * y - RowLength, Delta * z - RowLength, 0.0);
			return Pos;
		}

		void GenerateParticlesTexture(std::vector<ParticleData>& data) 
		{
			data.resize(NumParticles);
			for (UINT i = 0; i < NumParticles; i++) 
			{
				XMFLOAT4 pos = GetPosition(i);
				data[i] = { pos };
			}
		}

	private:
		void CreateGameObject();

		std::shared_ptr<VertexBuffer> m_vertexBuffer;
		std::shared_ptr<IndexBuffer> m_indexBuffer;

		DepthBuffer* m_depthBuffer;
		SwapChain* m_swapChain;

		ptrRootSigature m_GraphicRootSignature;
		ptrPSO m_GraphicPSO;

		ptrMesh m_Mesh;
		ptrTexture m_texture;
		ptrMaterial m_Material;
		GObject m_ParticleObject;

		ProjectiveCamera m_Camera;
	};
}

