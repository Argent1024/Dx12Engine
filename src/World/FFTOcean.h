#pragma once

#include "stdafx.h"
#include "Transform.h"
#include "GObject.h"
#include "Camera.h"

#include <complex>
#include <random>

using namespace Game;
using namespace Math;

// TODO maybe don't use std complex
using Complex = std::complex<double>;


const UINT OceanResolution = 256;

// How to reverse bits
// http://graphics.stanford.edu/~seander/bithacks.html#BitReverseTable
// https://stackoverflow.com/questions/2602823/in-c-c-whats-the-simplest-way-to-reverse-the-order-of-bits-in-a-byte
// wooooow! 
static const unsigned char BitReverseTable256[256] = 
{
#   define R2(n)     n,     n + 2*64,     n + 1*64,     n + 3*64
#   define R4(n) R2(n), R2(n + 2*16), R2(n + 1*16), R2(n + 3*16)
#   define R6(n) R4(n), R4(n + 2*4 ), R4(n + 1*4 ), R4(n + 3*4 )
	R6(0), R6(2), R6(1), R6(3)
};


inline UINT bitReverse256(UINT n) {
	return BitReverseTable256[n];
}

inline UINT bitReverse512(UINT n) {
	return BitReverseTable256[n & 0xff]<<8 | (n & 256);
}

// Reverse bits end


// Fast Fourire Transform 
std::vector<Complex> BitReverseCopy(const std::vector<Complex>& v);

// Calculate coeff[i] * w^i
// Where w^n = 1
std::vector<Complex> FFT(const std::vector<Complex>& coeff);



class OceanPSO : public Graphic::GraphicsPSO {
public:
	void Initialize() override;
};


class OceanMaterial : public Material {
public:
	enum SlotMaps 
	{
		ObjectCBV, // 0: useless, since Material class doesn't store this CBV
		MatCBV,	   // 1: useless

		DisplacementTex, // 2
		NormalTex // 3
	};

	struct MatData {
		UINT TextureResolution;
		BOOL padding0;
		BOOL padding1;
		BOOL padding2;
	};

	static const UINT MatCBVSize = CalculateConstantBufferByteSize(sizeof(OceanMaterial::MatData));


	OceanMaterial(MatData data) : m_MatData(data) 
	{ 
		assert(sizeof(MatData) % 16 == 0);
		ptrGBuffer buffer = GPU::MemoryManager::CreateGBuffer();
		buffer->Initialize(MatCBVSize);
		m_MatCBV.Initialze(buffer, MatCBVSize);
	}

	// not using
	void UploadCBV() { m_MatCBV.copyData(&m_MatData); }

	void BindMaterialAt(Graphic::DescriptorTable& table) 
	{
		assert(m_Displacement != nullptr && "Haven't set texture for diffuse color");
		m_Displacement->CreateView(Graphic::TEXTURE_SRV, &table, DisplacementTex);
		m_Normal->CreateView(Graphic::TEXTURE_SRV, &table, NormalTex);
	}

	Graphic::Texture* m_Displacement;
	Graphic::Texture* m_Normal;

	MatData m_MatData;
	Graphic::ConstantBuffer m_MatCBV;
};


class FFTOcean {

public:
	// res: how many pixel the texture will have, vertices number = (res+1)^2
	FFTOcean(UINT res = OceanResolution) : 
		m_ResX(res), m_ResY(res)
	{ };

	void Update();

	ptrMesh GetMesh() { return m_Mesh; }

	ptrMaterial GetMaterial() { return m_Material; }


	void Initialize() {
		

		UINT size = m_ResX * m_ResY;
		m_Displacement = std::vector<Vector3>(size);
		m_coeff = std::vector<std::vector<Complex>>(m_ResX, std::vector<Complex>(m_ResY));
		m_A =  std::vector<std::vector<Complex>>(m_ResX, std::vector<Complex>(m_ResY));
		
		// Create random for each point
		m_Random = std::vector<std::vector<Complex>>(m_ResX, std::vector<Complex>(m_ResY));
		std::default_random_engine generator;
		std::normal_distribution<double> distribution(0.0,1.0);
		for (int n = 0; n < m_ResX; ++n) {
			for (int m = 0; m < m_ResY; ++m) {
				double r = distribution(generator);
				double i = distribution(generator);
				m_Random[n][m] = Complex(r, i) / std::sqrt(2.0);
			}
		}


		// Create Texture
		// XMVECTOR 4 x 16 byte
		DXGI_FORMAT format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		m_DisplacementTexture = new Graphic::Texture2D(m_ResX, m_ResY, Graphic::TEXTURE_SRV, format);
		m_NormalTexture = new Graphic::Texture2D(m_ResX, m_ResY, Graphic::TEXTURE_SRV, DXGI_FORMAT_R8G8B8A8_UNORM, TRUE);
		

		// Create Mesh
		// m_Mesh = TriangleMesh::GetXYPlane();
		InitOceanMesh();

		OceanMaterial::MatData materialData;
		materialData.TextureResolution = m_ResX;

		m_Material = std::make_shared<OceanMaterial>(materialData);
		m_Material->m_Displacement = m_DisplacementTexture;
		m_Material->m_Normal = m_NormalTexture;
		m_Material->UploadCBV();

		// Test FFT
		
		/*std::vector<Complex> c(256, 1);
		std::vector<Complex> res = FFT(c);*/

	}
	
private:

	inline double Spectrum(const Vector2 k) const 
	{
		double k2 = Length2(k);
		double kw2 = std::pow((double)Dot(k, W_dir), 2.0);
		return A * std::exp(-1.0 / (k2 * L2)) * kw2 / (k2 * k2);
	}

	inline Complex H0(const Vector2 k, const Complex R) const {
		return R * Complex(std::sqrt(Spectrum(k)));
	}

	Complex Amplitede(UINT x, UINT y) 
	{
		if (x == m_ResX/2 && y == m_ResY/2) { return Complex(0.0, 0.0); }
		/*Vector2 k(1.0f * x / m_ResX - 0.5f, 1.0f * y / m_ResY - 0.5f);
		k *= 2 * PI * 530;*/

		Vector2 k(1.0f * (x - m_ResX/2) , 1.0f * (y - m_ResY/2));
		k *= 2 * PI / 500.f;

		Complex R = m_Random[x][y];

		Complex h1 = H0(k, R);
		Complex h2 = std::conj(H0(-k, R));
		
		Complex w = Complex(0.0, std::sqrt(g * (double)Length(k)));
		
		Complex h = h1 * std::exp(w) + h2 * std::exp(-w);
		return h;
	}

	void InitOceanMesh();

	ptrMesh m_Mesh;
	std::shared_ptr<OceanMaterial> m_Material;
	Graphic::Texture* m_DisplacementTexture;
	Graphic::Texture* m_NormalTexture;
	std::vector<Vector3> m_Displacement;

	std::vector<std::vector<Complex>> m_Random;
	std::vector<std::vector<Complex>> m_coeff;
	std::vector<std::vector<Complex>> m_A; // Store 1d fft

	UINT m_ResX, m_ResY;

	// Constants used for calculating the height
	const double g = 9.8;
	const double A = 2.0;
	const double V = 1.32;	// Wind speed 

	const double L = V * V / g;
	const double L2 = L * L;

	const Vector2 W_dir = Normalize(Vector2(1.0, 1.0));

};
