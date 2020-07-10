#pragma once

#include "MathCommon.h"
#include "GraphicCore.h"
#include "Texture.h"

namespace Game {
		
	using namespace Math;

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
			XMFLOAT4 color;
			bool UseTexture;
		};

		SimpleMaterial(Vector3 color) : m_Tex(nullptr) { SetColor(color); }

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
			if (m_Tex) 
			{
				m_Tex->CreateView(Graphic::TEXTURE_SRV, &table, texSlot);
			}
		}

		inline void UploadCBV() override { m_MatCBV->copyData(&m_Data); }

		inline void SetTexture(Graphic::Texture* texture) 
		{
			m_Tex = texture;
		}

		inline void SetColor(Vector3 color) 
		{
			XMStoreFloat4(&m_Data.color, color);
		}

	private:
		Data m_Data;
		Graphic::Texture* m_Tex;
	};


	class PrincipleMaterial : public Material 
	{
	public:
		struct Data
		{
			XMFLOAT4 diffuse;
		};

		void Initialize(); 

		void BindMaterialAt(Graphic::DescriptorTable& table) {};
		
		inline void UploadCBV() override { m_MatCBV->copyData(&m_MatData); }
	private:
		Data m_MatData;
	};
}