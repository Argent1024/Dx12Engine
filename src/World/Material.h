#pragma once

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

	class SimpleMaterial : public Material 
	{
	public: 
		static const UINT cbvSlot = 2; // Store in slot 2 in Descriptor table
		
		struct Data 
		{
			XMFLOAT4 diffuse;
		};

		void Initialize() 
		{
			UINT cbvSize = CalculateConstantBufferByteSize(sizeof(SimpleMaterial::Data));
			ptrGPUMem gpumem = Engine::MemoryAllocator.CreateCommittedBuffer(cbvSize);
			m_MatCBV = new Graphic::ConstantBuffer(gpumem, cbvSize);
			m_MatCBV->Initialize();
		}

		void BindMaterialAt(Graphic::DescriptorTable& table) 
		{
			m_MatCBV->CreateView(table, cbvSlot);
		}

	private:
		Data m_Data;
	};

	class PrincipleMaterial : public Material 
	{
	public:
		struct Data
		{
			XMFLOAT4 diffuse;
		};

		void Initialize(); 

		void BindMaterialAt(Graphic::DescriptorTable& table);

	private:
		Data m_MatData;
	};
}