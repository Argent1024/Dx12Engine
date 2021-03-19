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
		GObject();

		inline void SetMaterial(ptrMaterial mat)
		{ 
			m_Material = mat; 
			m_Material->BindMaterialAt(m_table);
		}

		inline void SetMesh(ptrMesh mesh) { m_Mesh = mesh; }

		inline ptrMaterial Material() { return m_Material; }

		inline void SetPhyState(Physic::PState* state) { m_state = state; }

		inline void SetTransform(const Math::Transform& T) { m_state->SetTransform(T); }
		inline Math::Transform GetTransform() const { return m_state->GetTransform(); }

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
		static const UINT cbSize = CalculateConstantBufferByteSize(sizeof(DirectX::XMMATRIX));
		Graphic::ConstantBuffer m_CBV;

		Graphic::DescriptorTable m_table;
		
		ptrMaterial m_Material;
		ptrMesh m_Mesh;
	};

	
}