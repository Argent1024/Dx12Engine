#pragma once
#include "Material.h"
#include "Mesh.h"

#define ptrMesh std::shared_ptr<Game::Mesh>
#define ptrMaterial std::shared_ptr<Game::Material>
#define ptrGobject std::shared_ptr<Game::GObject>

namespace Game {

	class GObject {
	public:
		GObject() {}
		
		GObject(ptrMaterial mat, ptrMesh mesh, const Math::Transform& T= Math::Transform()) 
			: m_Material(mat), m_Mesh(mesh), m_ModelTransform(T) {}

		inline void SetMaterial(ptrMaterial mat) { m_Material = mat; }
		inline void SetMesh(ptrMesh mesh) { m_Mesh = mesh; }

		// Prepare for drawing
		virtual void RecordCommand(Graphic::CommandList& commandList) {
			// TODO Refactor this


			m_Material->UseMaterial(commandList);
			m_Mesh->UseMesh(commandList);
		}
		
		virtual void Draw(Graphic::CommandList& commandList) {
			m_Mesh->Draw(commandList);
		}

		virtual void Update() {
			// Do nothing
			return;
		}

	protected:
		Math::Transform m_ModelTransform;
		ptrMaterial m_Material;
		ptrMesh m_Mesh;
	};

	
}