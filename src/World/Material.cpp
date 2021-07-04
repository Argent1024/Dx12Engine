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
			assert(FALSE && "Why ObjectCBV in set texture?");
			break;
		case Game::PrincipleMaterial::MatCBV:
			assert(FALSE && "Why MatCBV in set texture?");
			break;
		case Game::PrincipleMaterial::DiffuseTex:
			m_MatData.CTexture = tex != nullptr;
			m_DiffuseTex = tex;
			break;
		case Game::PrincipleMaterial::NormalTex:
			m_MatData.NTexture = tex != nullptr;
			m_NormalTex = tex;
			break;
		case Game::PrincipleMaterial::RoughnessTex:
			break;
		default:
			break;
		}
	}
}


void UI::PrincipleMaterialUIWindow(std::shared_ptr<Game::PrincipleMaterial> mat, const std::string window_name) {
		Game::PrincipleMaterial::MatData& data = mat->GetData();
		ImGui::Begin(window_name.c_str());
		bool update = false;

		update = update | ImGui::ColorEdit3("BaseColor", (float*)&data.BaseColor); 
		update = update | ImGui::SliderFloat("Roughness", &data.Roughness, 0.0, 1.0); 
		update = update | ImGui::SliderFloat("Metallic", &data.Metallic, 0.0, 1.0); 
		update = update | ImGui::SliderFloat("Specular", &data.Specular, 0.0, 1.0); 

		if (update) { mat->UploadCBV(); }
         

		ImGui::End();
}
