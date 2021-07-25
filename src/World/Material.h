#pragma once

#include "Math/MathLib.h"
#include "Graphic/Texture.h"
#include "UiEngine.h"

namespace Game {
		
	class Material 
	{
	public:

		// Bind views, textures to the descriptor table
		virtual Graphic::DescriptorTable& BindMaterialAt() = 0;

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
			MatCBV = 0,
			DiffuseTex = 1,
			NormalTex = 2, 
			RoughnessTex = 3,

			// TODO reconsider this
			BrdfTex = 4
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

		Graphic::DescriptorTable& BindMaterialAt() override;

		inline MatData& GetData() { return m_MatCBV.GetData(); }
		
		inline void SetData(const MatData& data) { m_MatCBV.SetData(data); }

		// Set texture to xxx, the default will remove the texture of one slot (pass a nullptr in Texture)
		void SetTexture(SlotMaps texType, ptrTex2D tex=nullptr);

		inline void UploadCBV() override { m_MatCBV.UpdateData(); }

	private:
		static constexpr UINT BRDF_TEX_Width = 1024;
		static constexpr UINT BRDF_TEX_Height = 1024;
		static void InitBRDFTexture();

		void BindTexture(SlotMaps slot, Graphic::DescriptorTable& table);

		Graphic::ConstantBuffer<MatData> m_MatCBV;
		Graphic::DescriptorTable m_DTable;

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
		
		PureColorCube() : m_DTable(4)
		{
			m_MatCBV.Initialize();
		}

		Graphic::DescriptorTable& BindMaterialAt() override 
		{
			m_MatCBV.CreateView(&m_DTable, 0);
			m_Textures->CreateSRV(&m_DTable, 1);
			return m_DTable;
		};


		void SetTexture(UINT slot, ptrTexCube tex) 
		{
			// assert(slot < MaxTextures);
			m_Textures = tex;
		}

		inline void UploadCBV() override { m_MatCBV.UpdateData(); }

	private:
		Graphic::ConstantBuffer<MatData> m_MatCBV;
		Graphic::DescriptorTable m_DTable;

		ptrTexCube m_Textures;
	};
}

namespace UI {
	void PrincipleMaterialUIWindow(std::shared_ptr<Game::PrincipleMaterial> mat, const std::string window_name);
}