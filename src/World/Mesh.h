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
	// TODO Doesn't consider some weird usage yet
	class TriangleMesh : public Mesh {
	public:
		
		TriangleMesh(ptrVertexBuffer v, ptrIndexBuffer i)
			: Mesh(), m_VertexBuffer(v), m_IndexBuffer(i) {}

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
		PointMesh(ptrVertexBuffer v) 
			: m_VertexBuffer(v) {}

		void UseMesh(Graphic::CommandList& commandList) override;
		void Draw(Graphic::CommandList& commandList) override;

		static const D3D_PRIMITIVE_TOPOLOGY TopologyType = D3D_PRIMITIVE_TOPOLOGY_POINTLIST;
	protected:
		ptrVertexBuffer m_VertexBuffer;
	};

}