#pragma once

#include "Math/MathCommon.h"
#include "Graphic/Texture.h"

namespace Game {
		
	class Material 
	{
	public:
		
		// Create Data from template
		virtual void Initialize() = 0;

		// Bind views, textures to the descriptor table
		virtual void BindMaterialAt(Graphic::DescriptorTable& table) = 0;

		// Copy data to cbv
		virtual void UploadCBV() = 0;

	protected:
		// Helper function for Materials.
		// SetNormalMap(texture) -> SetTexture(texture, normalSlot)
		inline virtual void SetTexture(Graphic::Texture* texture, UINT slot) 
		{
			assert(slot < m_Textures.size() && "Texture slot out of ranged");
			m_Textures[slot] = texture;
		}

		inline virtual Graphic::Texture* GetTexture(UINT slot) 
		{
			assert(slot < m_Textures.size() && "Texture slot out of ranged");
			return m_Textures[slot];
		}

		inline virtual void BindTexture(UINT slot, 
										Graphic::DescriptorTable& table, 
										Graphic::TextureType type = Graphic::TEXTURE_SRV)
		{
			Graphic::Texture* T = m_Textures[slot];
			assert(T != nullptr && "Binding Texture not setted yet");
			T->CreateView(type, &table, slot);
		}


		Graphic::ConstantBuffer* m_MatCBV;
		std::vector<Graphic::Texture*> m_Textures;
	};

	/********************************************************************************** 
	************************  Material implementation below ***************************
	***********************************************************************************/

	// Store only color
	class SimpleMaterial : public Material
	{
	public:
		static const UINT cbvSlot = 1; // Store in slot 1 in Descriptor table
		static const UINT texSlot = 2;
		struct Data
		{
			DirectX::XMFLOAT4 color;
			bool UseTexture = FALSE;
		};

		SimpleMaterial(Math::Vector3 color) { SetColor(color); }

		void Initialize() override
		{
			UINT cbvSize = CalculateConstantBufferByteSize(sizeof(SimpleMaterial::Data));
			ptrGPUMem gpumem = Engine::MemoryAllocator.CreateCommittedBuffer(cbvSize, D3D12_HEAP_TYPE_UPLOAD);
			m_MatCBV = new Graphic::ConstantBuffer(gpumem, cbvSize);
			m_MatCBV->Initialize();
		}

		inline void BindMaterialAt(Graphic::DescriptorTable& table) override
		{
			m_MatCBV->CreateView(table, cbvSlot);
			if (m_Textures.size() > 0) {
				m_Textures[0]->CreateView(Graphic::TEXTURE_SRV, &table, texSlot);
			}
		}

		inline void UploadCBV() override { m_MatCBV->copyData(&m_Data); }

		inline void SetColor(Math::Vector3 color) 
		{
			DirectX::XMStoreFloat4(&m_Data.color, color);
		}

	private:
		Data m_Data;
	};


	class PrincipleMaterial : public Material 
	{
	public:
		// Should compatatible with root signature

		
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

		void Initialize(); 

		void BindMaterialAt(Graphic::DescriptorTable& table);
		
		inline Data& GetData() { return m_MatData; }
		inline void SetData(const Data& data) { m_MatData = data; }

		inline void UploadCBV() override { m_MatCBV->copyData(&m_MatData); }
	private:
		Data m_MatData;
	};
}