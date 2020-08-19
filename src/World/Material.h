#pragma once

#include "Math/MathLib.h"
#include "Graphic/Texture.h"

namespace Game {
		
	class Material 
	{
	public:

		// Bind views, textures to the descriptor table
		virtual void BindMaterialAt(Graphic::DescriptorTable& table) = 0;

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
			ObjectCBV, // useless, since Material class doesn't store this CBV
			
			MatCBV,    // Slot store material information. E.X. Diffuse Color, Use Normal map?, ... 

			DiffuseColor,	
			Normal,
			Roughness
		};
		

		struct Data
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

		static const UINT MatCBVSize = CalculateConstantBufferByteSize(sizeof(PrincipleMaterial::Data));


		PrincipleMaterial();

		void BindMaterialAt(Graphic::DescriptorTable& table);
		
		inline Data& GetData() { return m_MatData; }
		inline void SetData(const Data& data) { m_MatData = data; }

		inline void UploadCBV() override { m_MatCBV.copyData(&m_MatData); }

	private:
		Data m_MatData;
		Graphic::ConstantBuffer m_MatCBV;
	};
}