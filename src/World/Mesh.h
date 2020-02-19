#pragma once
#include "CommandList.h"
#include "Descriptor.h"


#define ptrVertexBuffer std::shared_ptr<Graphic::VertexBuffer>
#define ptrIndexBuffer std::shared_ptr<Graphic::IndexBuffer>
#define ptrMesh std::shared_ptr<Game::Mesh>


namespace Game {
	class Mesh {
	public:
		virtual void UseMesh(Graphic::CommandList& commandList) = 0;
		virtual void Draw(Graphic::CommandList& commandList) = 0;
	};

	// Class for triangle mesh.
	class TriangleMesh : public Mesh {
	public:
		// Create the mesh with data, create vertex buffer & index buffer
		template<class Vertex>
		TriangleMesh(const std::vector<Vertex>& vertex, const std::vector<UINT>& index)
		{
			const UINT vertexBufferSize = vertex.size() * sizeof(Vertex);
			const UINT indexBufferSize = index.size() * sizeof(UINT);

			// Allocate memory from gpu to store vertex buffer and index buffer
			ptrGPUMem gpumem = Engine::MemoryAllocator.CreateCommittedBuffer(vertexBufferSize + indexBufferSize);
			m_VertexBuffer = std::make_shared<Graphic::VertexBuffer>(gpumem, vertexBufferSize, sizeof(Vertex));
			m_VertexBuffer->copyData(&vertex[0]);
		
			m_IndexBuffer = std::make_shared<Graphic::IndexBuffer>(gpumem, indexBufferSize);
			m_IndexBuffer->copyData(&index[0]);
		}

		void UseMesh(Graphic::CommandList& commandList) override;

		// navie draw one instance
		void Draw(Graphic::CommandList& commandList) override;
		
		static const D3D_PRIMITIVE_TOPOLOGY TopologyType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	protected:
		ptrVertexBuffer m_VertexBuffer;
		ptrIndexBuffer m_IndexBuffer;

	};

	// Class that store point, used for particle system
	class PointMesh : public Mesh {
	public:
		template <class Vertex>
		PointMesh(const std::vector<Vertex>& vertex) 
		{
			const UINT vertexBufferSize = vertex.size() * sizeof(Vertex);
			ptrGPUMem gpumem = Engine::MemoryAllocator.CreateCommittedBuffer(vertexBufferSize);
			m_VertexBuffer = std::make_shared<Graphic::VertexBuffer>(gpumem, vertexBufferSize, sizeof(Vertex));
			m_VertexBuffer->copyData(&vertex[0]);
		}

		void UseMesh(Graphic::CommandList& commandList) override;
		void Draw(Graphic::CommandList& commandList) override;
		
		static const D3D_PRIMITIVE_TOPOLOGY TopologyType = D3D_PRIMITIVE_TOPOLOGY_POINTLIST;
	protected:
		ptrVertexBuffer m_VertexBuffer;
	};

}