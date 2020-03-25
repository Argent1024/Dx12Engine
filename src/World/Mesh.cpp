#include "Mesh.h"

namespace Game {

	void TriangleMesh::UseMesh(Graphic::CommandList& commandList) {
		assert(m_VertexBuffer != nullptr && m_IndexBuffer != nullptr);
		commandList.SetPrimitiveTopology(TopologyType);
		commandList.SetVertexBuffer(*m_VertexBuffer);
		commandList.SetIndexBuffer(*m_IndexBuffer);
	}

	void TriangleMesh::Draw(Graphic::CommandList& commandList) {
		// TODO modify
		// Index buffer store UINT so, num of index == buffersize / sizeof(UINT)
		UINT IndexCountPerINstance = m_IndexBuffer->GetSize() / sizeof(UINT);
		commandList.DrawIndexedInstanced(IndexCountPerINstance, 1, 0, 0, 0);
	}

	void PointMesh::UseMesh(Graphic::CommandList& commandList) {
		commandList.SetPrimitiveTopology(TopologyType);
		commandList.SetVertexBuffer(*m_VertexBuffer);
	}

	void PointMesh::Draw(Graphic::CommandList& commandList) {
		// Need vertex buffer stride size to get num of vertex
		UINT pointSize = m_VertexBuffer->GetSize() / m_VertexBuffer->GetStrideSize();
		commandList.DrawInstanced(pointSize, 1, 0, 0);
	}
}