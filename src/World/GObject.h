#pragma once
#include "Material.h"
#include "Mesh.h"
#include "Transform.h"
#include "Texture.h"

#define ptrMesh std::shared_ptr<Game::Mesh>
#define ptrMaterial std::shared_ptr<Game::Material>
#define ptrGobject std::shared_ptr<Game::GObject>

namespace Game {

	class GObject {
	public:
		GObject(UINT textureNum=1) : m_table(textureNum) {}
		
		GObject(ptrMesh mesh, const Math::Transform& T= Math::Transform(), UINT textureNum=1) 
			: m_Mesh(mesh), m_Transform(T), m_table(textureNum) {}


		inline Graphic::DescriptorTable& GetDescriptorTable() { return m_table; }

		//inline void SetMaterial(ptrMaterial mat) { m_Material = mat; }
		inline void SetMesh(ptrMesh mesh) { m_Mesh = mesh; }
		inline void SetTransform(const Math::Transform& T) { m_Transform = T; }

		inline Math::Transform GetTransform() const { return m_Transform; }

		virtual void Initialize();

		// Prepare for drawing
		virtual void RecordCommand(Graphic::CommandList& commandList); 
		
		virtual void Draw(Graphic::CommandList& commandList) {
			m_Mesh->Draw(commandList);
		}

		virtual void Update() {
			// Do nothing
			return;
		}

	protected:
		Math::Transform m_Transform;

		// TODO make a struct later. only model Transformation for now
		// Store const data used in rendering ( Transformation, other settings )
		//Graphic::TextureBuffer* m_Texture;
		Graphic::ConstantBuffer* m_CBV;

		Graphic::DescriptorTable m_table;
		// ptrMaterial m_Material;
		ptrMesh m_Mesh;
	};

	
}