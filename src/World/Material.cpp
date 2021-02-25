#include "stdafx.h"
#include "Material.h"


namespace Game {

	PrincipleMaterial::PrincipleMaterial() 
	{
		assert(sizeof(PrincipleMaterial::MatData) % 16 == 0);

		ptrGBuffer buffer = GPU::MemoryManager::CreateGBuffer();
		buffer->Initialize(MatCBVSize);
		m_MatCBV.Initialze(buffer, MatCBVSize);
	}

	void PrincipleMaterial::BindMaterialAt(Graphic::DescriptorTable& table) 
	{
		m_MatCBV.CreateView(&table, MatCBV);

		if (m_MatData.CTexture) { BindTexture(DiffuseTex, table); }
		if (m_MatData.NTexture) { BindTexture(NormalTex, table); }
		
		// TODO other textures
	}

	void PrincipleMaterial::BindTexture(SlotMaps slot, Graphic::DescriptorTable& table) 
	{
		switch (slot)
		{
		case Game::PrincipleMaterial::ObjectCBV:
			break;
		case Game::PrincipleMaterial::MatCBV:
			break;
		case Game::PrincipleMaterial::DiffuseTex:
			assert(m_DiffuseTex != nullptr && "Haven't set texture for diffuse color");
			m_DiffuseTex->CreateView(Graphic::TEXTURE_SRV, &table, DiffuseTex);
			break;
		case Game::PrincipleMaterial::NormalTex:
			break;
		case Game::PrincipleMaterial::RoughnessTex:
			break;
		default:
			break;
		}
	}

	void PrincipleMaterial::SetTexture(SlotMaps texType, Graphic::Texture* tex)
	{
		switch (texType)
		{
		case Game::PrincipleMaterial::ObjectCBV:
			break;
		case Game::PrincipleMaterial::MatCBV:
			break;
		case Game::PrincipleMaterial::DiffuseTex:
			m_MatData.CTexture = tex != nullptr;
			m_DiffuseTex = tex;
			break;
		case Game::PrincipleMaterial::NormalTex:
			break;
		case Game::PrincipleMaterial::RoughnessTex:
			break;
		default:
			break;
		}
	}
}