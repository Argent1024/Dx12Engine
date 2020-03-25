#pragma once
#include "CommandList.h"
#include "Descriptor.h"

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
		// Simple init
		// Create the mesh with all the data, create vertex buffer & index buffer
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

		// Use this constructor when we already created the vertex and index buffer
		// We only need to suballocate a index buffer from the large index buffer
		TriangleMesh(ptrVertexBuffer vertex, ptrIndexBuffer index, 
			         const UINT start, const UINT end) 
		{
			m_VertexBuffer = vertex;
			m_IndexBuffer = std::make_shared<Graphic::IndexBuffer>(*index, start, end);
		}

		// Constructor simliar to the upper one
		// Create a sub-mesh from a bigger one, I think the bigger one should be released after
		// created all the sub-mesh.
		TriangleMesh(TriangleMesh& mesh, const UINT start, const UINT end)
		{
			m_VertexBuffer = mesh.m_VertexBuffer;
			m_IndexBuffer = std::make_shared<Graphic::IndexBuffer>(*mesh.m_IndexBuffer, start, end);
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