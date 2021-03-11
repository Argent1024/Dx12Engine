#include "stdafx.h"
#include "FFTOcean.h"


void OceanPSO::Initialize() 
{
	assert(m_rootSignature != nullptr);
	m_psoDesc.pRootSignature = m_rootSignature;

	ComPtr<ID3DBlob> VS;
	ComPtr<ID3DBlob> PS;

	const std::wstring path = L"D:\\work\\tEngine\\Shaders\\Ocean.hlsl";

	ThrowIfFailed(D3DCompileFromFile(path.c_str(), nullptr, nullptr, "VSMain", "vs_5_0", CompileFlags, 0, &VS, nullptr));
	ThrowIfFailed(D3DCompileFromFile(path.c_str(), nullptr, nullptr, "PSMain", "ps_5_0", CompileFlags, 0, &PS, nullptr));

	D3D12_INPUT_ELEMENT_DESC  inputElementDescs[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,  0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_UINT,     0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 1, DXGI_FORMAT_R32G32_FLOAT,    0, 20, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
	};

	this->SetVertexShader(CD3DX12_SHADER_BYTECODE(VS.Get()));
	this->SetPixelShader(CD3DX12_SHADER_BYTECODE(PS.Get()));
	this->SetTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
	this->SetInoutLayout(_countof(inputElementDescs), inputElementDescs);

	// configrations
	CD3DX12_DEPTH_STENCIL_DESC depthStencilDesc(D3D12_DEFAULT);
	depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
	this->SetDepthStencilState(depthStencilDesc);

	this->SetBlendState();

	CD3DX12_RASTERIZER_DESC raseterDesc(D3D12_DEFAULT);
	raseterDesc.CullMode = D3D12_CULL_MODE_NONE;
	this->SetRasterState(raseterDesc);

	this->SetStuffThatIdontKnowYet();
	this->CreatePSO();
}

void FFTOcean::InitOceanMesh() 
{
	struct VertexData 
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMUINT2 texcoor;
		DirectX::XMFLOAT2 fuv;
	};
	
	// Test texture
	/*std::vector<VertexData> triangleVertices =
	{
		{ { 1.0f,  1.0f, 0.0f }, { 1, 0 }, { 1.0f, 0.0f  } },
		{ { 1.0f, -1.0f, 0.0f }, { 1, 1 }, { 1.0f, 1.0f } },
		{ { -1.0f, 1.0f, 0.0f }, { 0, 0 }, { 0.0f, 0.0f } },
		{ { -1.0f, -1.0f, 0.0f }, { 0, 1}, { 0.0f, 1.0f } }
	};
	std::vector<UINT> index_list = { 0, 1, 2, 3, 2, 1 };
	m_Mesh = std::make_shared<TriangleMesh>(triangleVertices, index_list);*/



	UINT vertexSize = (m_ResX + 1) * (m_ResY + 1);
	std::vector<VertexData> vertices(vertexSize);

	UINT pixelSize = m_ResX * m_ResY;
	std::vector<UINT> index;

	const float dx = 2.0f / m_ResX;

	for (int x = 0; x < m_ResX + 1; ++x) {
		for (int y = 0; y < m_ResY + 1; ++y) {
			int i = x * (m_ResY + 1) + y;
			VertexData& v = vertices[i];
			v.position = DirectX::XMFLOAT3(dx*x - 1.0f, dx*y - 1.0f, 0.0f);
			v.texcoor = DirectX::XMUINT2(x, y);
			v.fuv = DirectX::XMFLOAT2(float(x) / m_ResX, float(y) / m_ResY);
		}
	}

	for (int x = 0; x < m_ResX; ++x) {
		for (int y = 0; y < m_ResY; ++y) {
			UINT a = x * (m_ResY + 1) + y;
			UINT b = (x+1) * (m_ResY + 1) + y;
			UINT c = x * (m_ResY + 1) + (y+1);
			UINT d = (x+1) * (m_ResY + 1) + (y+1);
			index.push_back(a);
			index.push_back(b);
			index.push_back(c);

			index.push_back(c);
			index.push_back(d);
			index.push_back(b);
		}
	}

	m_Mesh = std::make_shared<TriangleMesh>(vertices, index);
}


void FFTOcean::Update() {
	// calculate coeff
	for (int n = 0; n < m_ResX; ++n) {
		for (int m = 0; m < m_ResY; ++m) {
			Complex h = Amplitede(n, m);
			m_coeff[n][m] = h;

			float s = 2.0 * h.real() + 0.5;
			int index = n * m_ResY + m;
			m_Displacement[index] = Vector3(s, s, 0);
		}
	}


	// FFT
	for (int n = 0; n < m_ResX; ++n) {
		std::vector<Complex> An = FFT(m_coeff[n]);
		for (int i = 0; i < An.size(); ++i) {
			m_A[i][n] = An[i];
		}
	}

	for (int m = 0; m < m_ResY; ++m) {
		std::vector<Complex> hm = FFT(m_A[m]);
		for (int n = 0; n < hm.size(); ++n) {
			float h = hm[n].real();
			int index = n * m_ResY + m;
			m_Displacement[index] = Vector3(h, h, h);
		}
	}
	
	D3D12_SUBRESOURCE_DATA textureData = 
	Graphic::Texture::CreateTextureData({ m_ResX, m_ResY, 4, 4}, (const unsigned char*)&m_Displacement[0]);
	m_DisplacementTexture->UploadTexture(&textureData);
}


std::vector<Complex> BitReverseCopy(const std::vector<Complex>& v) {
	int n = v.size();
	std::vector<Complex> A(n);
	for (int i = 0; i < n; i++) {
		A[bitReverse256(i)] = v[i];
	}
	return A;
}


std::vector<Complex> FFT(const std::vector<Complex>& coeff) {
	int n = coeff.size();
	int lgN = 8; // Assume 256 = 2^8 for simplicity

	// Bit reverse copy
	std::vector<Complex> A = BitReverseCopy(coeff);

	for (int i = 1; i <= lgN; ++i) {
		int m = 1<<i;
		Complex Wm = std::exp(Complex{0, 2 * PI / m});
		for (int k = 0; k < n; k += m) {
			Complex W{1.0, 0.0};
			for (int j = 0; j < m / 2; ++j) {
				Complex t = W * A[k + j + m/2];
				Complex u = A[k + j];
				A[k + j] = u + t;
				A[k + j + m/2] = u - t;
				W = W * Wm;
			}
		}
	}
	return A;
}