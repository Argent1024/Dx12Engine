#include "Mesh.h"
#include "Utility/MeshReader.h"

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

	ptrMesh TriangleMesh::GetXYPlane(UINT subdivision) 
	{
		// TODO subdivision
		std::vector<DefaultVertex> triangleVertices =
		{
			{ { 1.0f,  1.0f, 0.0f }, { 0.0f, 0.0f, 1.0f }, { 1.0f, 0.0f  } },
			{ { 1.0f, -1.0f, 0.0f }, { 0.0f, 0.0f, 1.0f }, { 1.0f, 1.0f } },
			{ { -1.0f, 1.0f, 0.0f }, { 0.0f, 0.0f, 1.0f }, { 0.0f, 0.0f } },
			{ { -1.0f, -1.0f, 0.0f }, { 0.0f, 0.0f, 1.0f}, { 0.0f, 1.0f } }
		};
		std::vector<UINT> index_list = { 0, 1, 2, 3, 2, 1 };
		return std::make_shared<TriangleMesh>(triangleVertices, index_list);
	}

	ptrMesh TriangleMesh::GetSphere(UINT numU, UINT numV) 
	{
		float u, v;
		float x, y, z;
		std::vector<DefaultVertex> vertices;
		for (int j = 0; j <= numV; ++j) {
			for (int i = 0; i < numU; ++i) {
				u = 2.0f * i * 3.1415926f / numU;
				v = j / numV;
				z = 2 * v - 1.0f;
				float s = sqrtf(1 - z * z);
				x = s * cosf(u);
				y = s * sinf(u);
				
				DefaultVertex vertex { {x, y, z}, {x, y, z}, {u, v} };
				vertices.push_back(vertex);
			}
		}

		std::vector<UINT> index;
		// connect north south pole
		// connect other
		for (int j = 1; j < numV - 1; ++j) {
			for (int i = 0; i < numU - 1; ++i) {
				UINT a = 1 + (j + 1) * numU + i;
				UINT b = a + 1;
				UINT c = b - numU;
				UINT d = c - 1;
				index.push_back(a);
				index.push_back(b);
				index.push_back(d);
				index.push_back(d);
				index.push_back(c);
				index.push_back(b);
			}
		}
		return std::make_shared<TriangleMesh>(vertices, index);
	}
}