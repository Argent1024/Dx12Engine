#pragma once
#include "Material.h"
#include "Mesh.h"
#include "Transform.h"

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

		virtual void Initialize() {
			m_table.Initialize(Engine::GetInitHeap());
		}

		// Prepare for drawing
		virtual void RecordCommand(Graphic::CommandList& commandList) {
			// TODO Refactor this
			// TODO! Put transformation
			
			assert(m_Mesh && "Not initialized GameObj");
			
			commandList.SetDescriptorHeap(*Engine::GetInUseHeap());
			CD3DX12_GPU_DESCRIPTOR_HANDLE tableHandle = m_table.BindDescriptorTable();
			commandList.SetGraphicsRootDescriptorTable(1, tableHandle);  //  slot 1 is reversed for object
			
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
		Math::Transform m_Transform;
		Graphic::DescriptorTable m_table;
		// ptrMaterial m_Material;
		ptrMesh m_Mesh;
	};

	
}