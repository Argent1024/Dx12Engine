#pragma once
#include "Material.h"
#include "Mesh.h"
#include "Utility/Logger.h"
#include "Physic/PhysicState.h"

#define ptrMaterial std::shared_ptr<Game::Material>
#define ptrGobject std::shared_ptr<Game::GObject>

namespace Game {

	class GObject {
	public:
		// TODO create phy state & fix textureNum,
		// Ask material for how large the descriptor table should be
		GObject(UINT textureNum=2) : m_table(textureNum) 
		{
			m_state = new Physic::PState();
		}
		
		GObject(ptrMesh mesh, const Math::Transform& T= Math::Transform(), UINT textureNum=2) 
			: m_Mesh(mesh), m_table(textureNum) 
		{
			m_state = new Physic::PState();
		}


		inline Graphic::DescriptorTable& GetDescriptorTable() { return m_table; }

		inline void SetMaterial(ptrMaterial mat) { m_Material = mat; }
		inline void SetMesh(ptrMesh mesh) { m_Mesh = mesh; }

		inline void SetPhyState(Physic::PState* state) { m_state = state; }

		inline void SetTransform(const Math::Transform& T) { m_state->SetTransform(T); }
		inline Math::Transform GetTransform() const { return m_state->GetTransform(); }

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
		// Math::Transform m_Transform;
		Physic::PState* m_state;

		// TODO make a struct later. only model Transformation for now
		// Store const data used in rendering ( Transformation, other settings )
		Graphic::ConstantBuffer* m_CBV;

		Graphic::DescriptorTable m_table;
		
		ptrMaterial m_Material;
		ptrMesh m_Mesh;
	};

	
}