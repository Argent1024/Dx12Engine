#pragma once
#include "Descriptor.h"

#define ptrVertexBuffer std::shared_ptr<Graphic::VertexBuffer>
#define ptrIndexBuffer std::shared_ptr<Graphic::IndexBuffer>

namespace Game {

	class Mesh {
	public:
		virtual void SetMesh(Graphic::CommandList& commandList) = 0;

	};

	// Class for triangle mesh.
	// TODO Doesn't consider some weird usage yet
	class TriangleMesh : public Mesh {
	public:
		TriangleMesh(ptrVertexBuffer v, ptrIndexBuffer i)
			: Mesh(), m_VertexBuffer(v), m_IndexBuffer(i) {}

		void SetMesh(Graphic::CommandList& commandList) override {
			commandList.SetVertexBuffer(*m_VertexBuffer);
			commandList.SetIndexBuffer(*m_IndexBuffer);
		}

	protected:
		ptrVertexBuffer m_VertexBuffer;
		ptrIndexBuffer m_IndexBuffer;

	};

}