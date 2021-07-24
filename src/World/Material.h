#pragma once

#include "Math/MathLib.h"
#include "Graphic/Texture.h"
#include "UiEngine.h"

namespace Game {
		
	class Material 
	{
	public:

		// Bind views, textures to the descriptor table
		virtual void BindMaterialAt(Graphic::DescriptorTable& table) = 0;

		// TODO not every material need one ?
		// Copy data to cbv
		virtual void UploadCBV() = 0;
	};

	/********************************************************************************** 
	************************  Material implementation below ***************************
	***********************************************************************************/

	class PrincipleMaterial : public Material 
	{
	public:
		// Should compatatible with root signature
		// Mapping for Textures
		enum SlotMaps
		{
			ObjectCBV, // 0: useless, since Material class doesn't store this CBV
			
			MatCBV,    // 1: Slot store material information. E.X. Diffuse Color, Use Normal map?, ... 
			DiffuseTex,	// 2: 
			PbrBrdf, // 3 TODO change the order of textures later

			NormalTex,
			RoughnessTex
		};
		

		struct MatData
		{
			DirectX::XMFLOAT3 BaseColor;
			FLOAT Roughness;
			FLOAT Metallic;
			FLOAT Specular;

			BOOL CTexture = FALSE;  // Diffuse Color Texture
			BOOL NTexture = FALSE;  // Normal Texture

			BOOL RTexture = FALSE;  // Roughness Texture
			BOOL MTexture = FALSE;  // Metallic Texture
			BOOL STexture = FALSE;  // Specular Texture
			BOOL padding;
 		};

		static const UINT MatCBVSize = CalculateConstantBufferByteSize(sizeof(PrincipleMaterial::MatData));

		static ptrTex2D brdfTex;

		PrincipleMaterial();

		void BindMaterialAt(Graphic::DescriptorTable& table) override;

		inline MatData& GetData() { return m_MatData; }
		
		inline void SetData(const MatData& data) { m_MatData = data; }

		// Set texture to xxx, the default will remove the texture of one slot (pass a nullptr in Texture)
		void SetTexture(SlotMaps texType, ptrTex2D tex=nullptr);

		inline void UploadCBV() override { m_MatCBV.copyData(&m_MatData); }

	private:
		static constexpr UINT BRDF_TEX_Width = 1024;
		static constexpr UINT BRDF_TEX_Height = 1024;
		static void InitBRDFTexture();

		void BindTexture(SlotMaps slot, Graphic::DescriptorTable& table);

		MatData m_MatData;
		Graphic::ConstantBuffer m_MatCBV;
		ptrTex2D m_DiffuseTex;
		ptrTex2D m_NormalTex;
	};


	class PureColorCube : public Material  {
	public:
		// static constexpr UINT MaxTextures = 4;
		struct MatData
		{ 
			BOOL Padding[4];
		};
		static constexpr UINT MatCBVSize = CalculateConstantBufferByteSize(sizeof(PureColorCube::MatData));
		
		PureColorCube() 
		{
			assert(sizeof(PureColorCube::MatData) % 16 == 0);

			ptrGBuffer buffer = GPU::MemoryManager::CreateGBuffer();
			buffer->Initialize(MatCBVSize);
			m_MatCBV.Initialze(buffer, MatCBVSize);
		}

		void BindMaterialAt(Graphic::DescriptorTable& table) override 
		{
			m_MatCBV.CreateView(&table, 1);
			m_Textures->CreateSRV(&table, 2);
		};


		void SetTexture(UINT slot, ptrTexCube tex) 
		{
			// assert(slot < MaxTextures);
			m_Textures = tex;
		}

		inline void UploadCBV() override { m_MatCBV.copyData(&m_MatData); }

	private:
		MatData m_MatData;
		Graphic::ConstantBuffer m_MatCBV;
		ptrTexCube m_Textures;
	};
}

namespace UI {
	void PrincipleMaterialUIWindow(std::shared_ptr<Game::PrincipleMaterial> mat, const std::string window_name);
}