#pragma once

#include "Math/MathLib.h"
#include "Graphic/Texture.h"

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

			DiffuseTex,	
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


		PrincipleMaterial();

		void BindMaterialAt(Graphic::DescriptorTable& table);

		inline MatData& GetData() { return m_MatData; }
		
		inline void SetData(const MatData& data) { m_MatData = data; }

		// Set texture to xxx, the default will remove the texture of one slot (pass a nullptr in Texture)
		void SetTexture(SlotMaps texType, Graphic::Texture* tex=nullptr);

		inline void UploadCBV() override { m_MatCBV.copyData(&m_MatData); }

	private:
		void BindTexture(SlotMaps slot, Graphic::DescriptorTable& table);

		MatData m_MatData;
		Graphic::ConstantBuffer m_MatCBV;
		Graphic::Texture* m_DiffuseTex;
	};
}