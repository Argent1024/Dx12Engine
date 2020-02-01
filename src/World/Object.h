#pragma once
#include "Material.h"
#include "Mesh.h"

#define pMesh std::shared_ptr<Game::Mesh>
#define pMaterial std::shared_ptr<Game::Material>

namespace Game {
	class Object {
	public:
		virtual void RecordCommand(Graphic::CommandList& commandList) = 0;

	protected:
		pMaterial m_Material;
		pMesh m_Mesh;
	};


}