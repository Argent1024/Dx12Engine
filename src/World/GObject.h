#pragma once
#include "Material.h"
#include "Mesh.h"

#define ptrMesh std::shared_ptr<Game::Mesh>
#define ptrMaterial std::shared_ptr<Game::Material>

namespace Game {

	class GObject {
	public:

		inline void SetMaterial(ptrMaterial mat) { m_Material = mat; }
		inline void SetMesh(ptrMesh mesh) { m_Mesh = mesh; }

		virtual void RecordCommand(Graphic::CommandList& commandList) {
			m_Material->UseMaterial(commandList);
			m_Mesh->UseMesh(commandList);
		}
		
		virtual void Draw(Graphic::CommandList& commandList) {
			m_Mesh->Draw(commandList);
		}

	protected:
		ptrMaterial m_Material;
		ptrMesh m_Mesh;
	};

	
}