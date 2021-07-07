#pragma once

#include "stdafx.h"
#include "Math/Transform.h"
#include "GObject.h"
#include "Camera.h"

#include <complex>
#include <random>

using namespace Game;
using namespace Math;

// TODO maybe don't use std complex
using Complex = std::complex<double>;



const UINT logOceanResolution = 8;
const UINT OceanResolution = 1<<logOceanResolution;

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

// TODO not working
inline UINT bitReverse512(UINT n) {
	return BitReverseTable256[n & 0xff]<<1 | (n >> 8)&1;
}

// Reverse bits end


// Fast Fourire Transform 
std::vector<Complex> BitReverseCopy(const std::vector<Complex>& v);

// Calculate coeff[i] * w^i
// Where w^n = 1
std::vector<Complex> FFT(const std::vector<Complex>& coeff);

// Test if FFT is correct
std::vector<Complex> StupidFT(const std::vector<Complex>& coeff);


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
		NormalTex,// 3
		ReflectionTex, // 4
		FoamTex,	// 5
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
		m_Displacement->CreateSRV(&table, DisplacementTex);
		m_Normal->CreateSRV(&table, NormalTex);
		/*m_Reflection->CreateSRV(&table, ReflectionTex);
		m_FoamTex->CreateSRV(&table, FoamTex);*/
	}

	ptrTex2D m_Displacement;
	ptrTex2D m_Normal;
	/*ptrTex2D m_Reflection;
	ptrTex2D m_FoamTex;*/

	MatData m_MatData;
	Graphic::ConstantBuffer m_MatCBV;
};


class FFTOcean {

public:
	// res: how many pixel the texture will have, vertices number = (res+1)^2
	FFTOcean(UINT res = OceanResolution) : 
		m_ResX(res), m_ResY(res), m_Time(0.0)
	{ };

	void Update(double dt);

	ptrMesh GetMesh() { return m_Mesh; }

	ptrMaterial GetMaterial() { return m_Material; }


	void Initialize() {
		

		UINT size = m_ResX * m_ResY;
		m_Displacement = std::vector<Vector3>(size);
		m_Normals = std::vector<Vector3>(size);

		m_coeff = std::vector<std::vector<Complex>>(m_ResX, std::vector<Complex>(m_ResY));
		m_A =  std::vector<std::vector<Complex>>(m_ResX, std::vector<Complex>(m_ResY));
		
		// Create random for each point
		m_Random = std::vector<std::vector<Complex>>(m_ResX, std::vector<Complex>(m_ResY));
		std::default_random_engine generator;
		generator.seed(20210312);
		std::normal_distribution<double> distribution(0.0,1.0);
		for (UINT n = 0; n < m_ResX; ++n) {
			for (UINT m = 0; m < m_ResY; ++m) {
				double r = distribution(generator);
				double i = distribution(generator);
				m_Random[n][m] = Complex(r, i) / std::sqrt(2.0);
			}
		}


		// Create Texture
		// XMVECTOR 4 x 16 byte
		DXGI_FORMAT format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		m_DisplacementTexture = std::make_shared<Graphic::Texture2D>(m_ResX, m_ResY, Graphic::TEXTURE_SRV, format);
		m_NormalTexture = std::make_shared<Graphic::Texture2D>(m_ResX, m_ResY, Graphic::TEXTURE_SRV, format);
	

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
		std::vector<Complex> v(OceanResolution, 1);
		std::vector<Complex> res = FFT(v);
	}
	
	Graphic::Texture* m_EnvMapping;

private:

	inline Vector2 WaveK(UINT n, UINT m) const {
		Vector2 k(((float)n - (m_ResX - 1.0)/2), ((float)m - (m_ResY - 1.0)/2));
		k *= 2.0 * PI / PatchSize;
		return k;
	}

	inline double Spectrum(const Vector2 k) const 
	{
		double k2 = Length2(k);
		k2 = std::max(0.001, k2);

		double kw2 = std::pow((double)Dot(Normalize(k), W_dir), 2.0);
		return A * std::exp(-1.0 / (k2 * L2)) * kw2 / k2 / k2;
	}

	/*inline Complex H0(const Vector2 k, const Complex R) const {
		return R * Complex(std::sqrt(Spectrum(k)));
	}*/

	inline Complex H0(int n, int m) {
		Complex R = m_Random[n][m];
		Vector2 k = WaveK(n, m);
		return R * std::sqrt(Spectrum(k));
	}

	Complex Amplitede(int n, int m) 
	{
		
		// if (n == m_ResX/2 && m == m_ResY/2) { return Complex(0.0, 0.0); }
		Vector2 k = WaveK(n, m);
		// Vector2 k2 = WaveK(m_ResX - 1 - n, m_ResY - 1 - m);

		Complex h1 = H0(n, m);
		Complex h2 = std::conj(H0(m_ResX - 1 - n, m_ResY - 1 - m));
		
		Complex w = Complex(0.0, std::sqrt(g * (double)Length(k)));
		
		Complex h = h1 * std::exp(w * m_Time) + h2 * std::exp(-w * m_Time);
		return h;
	}

	void InitOceanMesh();

	void AmplitedeUpdate();
	void HeightUpdate();
	void ShiftUpdate();
	void NormalUpdate();

	ptrMesh m_Mesh;
	std::shared_ptr<OceanMaterial> m_Material;
	ptrTex2D m_DisplacementTexture;
	ptrTex2D m_NormalTexture;
	
	// ptrTex2D m_FoamTexture;

	std::vector<Vector3> m_Displacement;
	std::vector<Vector3> m_Normals;

	std::vector<std::vector<Complex>> m_Random;
	std::vector<std::vector<Complex>> m_coeff;
	std::vector<std::vector<Complex>> m_A; // Store 1d fft

	double m_Time;

	UINT m_ResX, m_ResY;

	// Constants used for calculating the height
	const double g = 9.8;
	const double A = 2*.00000000775f; //1.0;
	const double PatchSize = 100;
	const double L = 10.0;
	const double L2 = L * L;
	const double m_VerticalShift = 3.0;	


	Vector2 W_dir = Normalize(Vector2(1.0, 1.0));

};

