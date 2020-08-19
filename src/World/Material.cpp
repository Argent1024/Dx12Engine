#include "Material.h"


namespace Game {

	PrincipleMaterial::PrincipleMaterial() 
		:	m_MatCBV(Engine::MemoryAllocator.CreateCommittedBuffer(MatCBVSize), MatCBVSize)
	{
		assert(sizeof(PrincipleMaterial::Data) % 16 == 0);

	}

	void PrincipleMaterial::BindMaterialAt(Graphic::DescriptorTable& table) 
	{
		m_MatCBV.CreateView(table, MatCBV);

		/* if (m_MatData.CTexture) {
			BindTexture(DiffuseColor, table);
		}

		if (m_MatData.NTexture) {
			BindTexture(Normal, table);
		} */
		
		// TODO other textures
	}

}