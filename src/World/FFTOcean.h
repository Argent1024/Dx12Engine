#pragma once

#include "stdafx.h"
#include "Transform.h"
#include "GObject.h"
#include "Camera.h"

#include <complex>

using namespace Game;
using namespace Math;

// TODO maybe don't use std complex
using Complex = std::complex<float>;


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
	FFTOcean(UINT res = 64) : m_ResX(res), m_ResY(res)
	{ 
		// TODO m_Random
	};

	void Update();

	ptrMesh GetMesh() { return m_Mesh; }

	ptrMaterial GetMaterial() { return m_Material; }


	void Initialize() {
		UINT size = (m_ResX + 1) * (m_ResY + 1);
		m_Displacement = std::vector<Vector3>(size);

		// Create Texture
		DXGI_FORMAT format = DXGI_FORMAT_R32G32B32_FLOAT;
		m_DisplacementTexture = new Graphic::Texture2D(m_ResX, m_ResY, Graphic::TEXTURE_SRV, format);
		m_NormalTexture = new Graphic::Texture2D(m_ResX, m_ResY, Graphic::TEXTURE_SRV, DXGI_FORMAT_R8G8B8A8_UNORM, TRUE);
		

		// Create Mesh
		//m_Mesh = TriangleMesh::GetXYPlane();
		InitOceanMesh();

		OceanMaterial::MatData materialData;
		materialData.TextureResolution = m_ResX;

		m_Material = std::make_shared<OceanMaterial>(materialData);
		m_Material->m_Displacement = m_DisplacementTexture;
		m_Material->m_Normal = m_NormalTexture;
		m_Material->UploadCBV();
	}
	
private:

	Scalar Spectrum(const Vector2 k) const 
	{
		Scalar k2 = Length2(k);
		Scalar kw2 = Pow(Dot(k, W_dir), Scalar(2.0));
		return A * Exp(-Scalar(1.0) / (k2 * L2)) * kw2 / (k2 * k2);
	}

	Complex AmplitedeBase(const Vector2 k) 
	{
		Scalar p = Sqrt(Spectrum(k));
		return m_Random * (float)p / sqrtf(2);
	}

	void InitOceanMesh();

	ptrMesh m_Mesh;
	std::shared_ptr<OceanMaterial> m_Material;
	Graphic::Texture* m_DisplacementTexture;
	Graphic::Texture* m_NormalTexture;
	std::vector<Vector3> m_Displacement;

	UINT m_ResX, m_ResY;
	Complex m_Random;

	// Constants used for calculating the height
	const Scalar g = 9.8;
	const Scalar A = 1.0;
	const Scalar V = 10.0;	// Wind speed 

	const Scalar L = V * V / g;
	const Scalar L2 = L * L;

	const Vector2 W_dir = Normalize(Vector2(1.0, 1.0));

};



