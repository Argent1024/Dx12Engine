#include "TextureTest.h"

namespace Samples {

	void TextureTestRS::Initialize(ComPtr<ID3D12Device> device) 
	{
		D3D12_FEATURE_DATA_ROOT_SIGNATURE featureData = {};
		// This is the highest version the sample supports. If CheckFeatureSupport succeeds, the HighestVersion returned will not be greater than this.
		featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_1;
		if (FAILED(device->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE, &featureData, sizeof(featureData))))
		{
			featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_0;
		}

		// Define SRV
		CD3DX12_DESCRIPTOR_RANGE1 range[1];
		CD3DX12_ROOT_PARAMETER1 rootParameters[1];
		range[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);
		rootParameters[0].InitAsDescriptorTable(1, &range[0]);

		D3D12_ROOT_SIGNATURE_FLAGS rsFlag = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
		// Create RootSignature
		CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
		rootSignatureDesc.Init_1_1(_countof(rootParameters), rootParameters, 0, nullptr, rsFlag);
		ComPtr<ID3DBlob> signature;
		ComPtr<ID3DBlob> error;
		ThrowIfFailed(D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, featureData.HighestVersion, &signature, &error));
		ThrowIfFailed(device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&m_RootSignature)));
	}

	void TextureTestSample::Init(const HWND m_appHwnd) 
	{
		this->EnableDebug();
		this->CreateDevice();
		
		// Initialize Command Manager
		//CopyCommandManager.Initialize(m_device);
		EngineGPUMemory.Initialize(m_device);
		CopyHelper.Initialize(m_device);
		GraphicsCommandManager.Initialize(m_device);

		CreatSwapChain(m_appHwnd);
		{
			m_rootSignature = std::make_shared<TextureTestRS>();
			m_rootSignature->Initialize(m_device);
		}

		{
			UINT compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
			ComPtr<ID3DBlob> VS;
			ComPtr<ID3DBlob> PS;
			const std::wstring path = L"D:\\work\\tEngine\\Shaders\\ray.hlsl";
			ThrowIfFailed(D3DCompileFromFile(path.c_str(), nullptr, nullptr, "VSMain", "vs_5_0", compileFlags, 0, &VS, nullptr));
			ThrowIfFailed(D3DCompileFromFile(path.c_str(), nullptr, nullptr, "PSMain", "ps_5_0", compileFlags, 0, &PS, nullptr));

			// Input for vertex 
			D3D12_INPUT_ELEMENT_DESC inputElementDescs[] =
			{
					{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
					{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
			};

			m_GraphicPSO = std::make_shared<GraphicsPSO>();
			m_GraphicPSO->SetRootSigature(m_rootSignature->GetRootSignature());
			m_GraphicPSO->SetVertexShader(CD3DX12_SHADER_BYTECODE(VS.Get()));
			m_GraphicPSO->SetPixelShader(CD3DX12_SHADER_BYTECODE(PS.Get()));
			m_GraphicPSO->SetTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
			m_GraphicPSO->SetInoutLayout(_countof(inputElementDescs), inputElementDescs);
			m_GraphicPSO->Initialize(m_device);
		}
		
		// Create Assert
		{
			// Define the geometry for a triangle.
			Vertex triangleVertices[] =
			{
				{ { 0.5f, 0.5f, 0.0f }, { 1.0f, 1.0f } },
				{ { 0.5f, -0.5f, 0.0f }, { 1.0f, 0.0f } },
				{ { -0.5f, 0.5f, 0.0f }, { 0.0f, 1.0f } },
				{ { -0.5f, -0.5f, 0.0f }, { 0.0f, 0.0f } }
			};
			UINT index_list[] = { 0, 1, 2, 3, 2, 1 };

			const UINT vertexBufferSize = sizeof(triangleVertices);
			const UINT indexBufferSize = sizeof(index_list);
			
			m_GPUmem = EngineGPUMemory.CreateCommittedBuffer(vertexBufferSize + indexBufferSize);

			m_vertexBuffer = std::make_shared<VertexBuffer>(m_GPUmem, vertexBufferSize, sizeof(Vertex));
			m_vertexBuffer->Initialize();
			m_vertexBuffer->copyData(triangleVertices);

			m_indexBuffer = std::make_shared<IndexBuffer>(m_GPUmem, indexBufferSize);
			m_indexBuffer->Initialize();
			m_indexBuffer->copyData((void*)index_list);
			
			m_Mesh =std::make_shared<TriangleMesh>(m_vertexBuffer, m_indexBuffer);
		}

		m_Material = std::make_shared<NoMaterial>(m_GraphicPSO, m_rootSignature);

		m_textureObject.SetMesh(m_Mesh);
		m_textureObject.SetMaterial(m_Material);

	}

}