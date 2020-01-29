#pragma once
#include "Descriptor.h"

#define ptrVertexBuffer std::shared_ptr<Graphic::VertexBuffer>
#define ptrIndexBuffer std::shared_ptr<Graphic::IndexBuffer>

namespace Game {

	class Mesh {
	public:
		virtual void SetMesh(Graphic::CommandList& commandList) = 0;
		virtual void Draw(Graphic::CommandList& commandList) = 0;
	};

	// Class for triangle mesh.
	// TODO Doesn't consider some weird usage yet
	class TriangleMesh : public Mesh {
	public:
		TriangleMesh(ptrVertexBuffer v, ptrIndexBuffer i)
			: Mesh(), m_VertexBuffer(v), m_IndexBuffer(i) {}

		void SetMesh(Graphic::CommandList& commandList) override {
			commandList.SetPrimitiveTopology(TopologyType);
			commandList.SetVertexBuffer(*m_VertexBuffer);
			commandList.SetIndexBuffer(*m_IndexBuffer);
		}

		// navie draw one instance
		void Draw(Graphic::CommandList& commandList) override {
			UINT IndexCountPerINstance = m_IndexBuffer->GetSize();
			commandList.DrawIndexedInstanced(IndexCountPerINstance, 1, 0, 0, 0);
		}

		static const D3D_PRIMITIVE_TOPOLOGY TopologyType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	protected:
		ptrVertexBuffer m_VertexBuffer;
		ptrIndexBuffer m_IndexBuffer;

	};

}