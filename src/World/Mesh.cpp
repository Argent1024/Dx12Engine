#include "Mesh.h"

namespace Game {
	void TriangleMesh::SetMesh(Graphic::CommandList& commandList) {
		commandList.SetPrimitiveTopology(TopologyType);
		commandList.SetVertexBuffer(*m_VertexBuffer);
		commandList.SetIndexBuffer(*m_IndexBuffer);
	}
	
	void TriangleMesh::Draw(Graphic::CommandList& commandList) {
		UINT IndexCountPerINstance = m_IndexBuffer->GetSize();
		commandList.DrawIndexedInstanced(IndexCountPerINstance, 1, 0, 0, 0);
	}

	void PointMesh::SetMesh(Graphic::CommandList& commandList) {
		commandList.SetPrimitiveTopology(TopologyType);
		commandList.SetVertexBuffer(*m_VertexBuffer);
	}

	void PointMesh::Draw(Graphic::CommandList& commandList) {
		UINT pointSize = m_VertexBuffer->GetSize();
		commandList.DrawInstanced(pointSize, 1, 0, 0);
	}
}