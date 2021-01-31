#include "Mesh.h"
#include "MathLib.h"
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
		UINT IndexCountPerINstance = m_IndexBuffer->Size() / sizeof(UINT);
		commandList.DrawIndexedInstanced(IndexCountPerINstance, 1, 0, 0, 0);
	}

	//void PointMesh::UseMesh(Graphic::CommandList& commandList) {
	//	commandList.SetPrimitiveTopology(TopologyType);
	//	commandList.SetVertexBuffer(*m_VertexBuffer);
	//}

	//void PointMesh::Draw(Graphic::CommandList& commandList) {
	//	// Need vertex buffer stride size to get num of vertex
	//	UINT pointSize = m_VertexBuffer->GetSize() / m_VertexBuffer->GetStrideSize();
	//	commandList.DrawInstanced(pointSize, 1, 0, 0);
	//}

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

		for (UINT j = 0; j <= numV; ++j) {
			for (UINT i = 0; i < numU; ++i) {
			
				/* // Make the triganle same area
				u = 2.0f * i * Math::PI / numU;
				v = float(j) / (numV);
				
				z = 2 * v - 1.0f;
				float s = sqrtf(1 - z * z);
				x = s * cosf(u);
				y = s * sinf(u);
				*/

				u = 2.0f * i * Math::PI / numU;
				v = j * Math::PI / numV;
				z = cosf(v);
				float s = sqrtf(1 - z * z);
				x = s * cosf(u);
				y = s * sinf(u);

				DefaultVertex vertex { {x, z, y}, {x, z, y}, {u, v} };
				vertices.push_back(vertex);

				// Pole
				if (j == 0 || j == numV + 1) { break; }
			}
		}

		std::vector<UINT> index;
		// connect north south pole

		for (UINT i = 0; i < numU; ++i) {
			const UINT n = 0;
			const UINT s = vertices.size() - 1;
			UINT a = 1 + i;
			UINT b = 2 + i;
			
			UINT c = s - numU + i;
			UINT d = c + 1;

			if (i == numU - 1) {
				b -= numU;
				d -= numU;
			} 
			index.push_back(n);
			index.push_back(a);
			index.push_back(b);

			index.push_back(s);
			index.push_back(c);
			index.push_back(d);
		}

		// connect other
		for (UINT j = 1; j < numV ; ++j) {
			for (UINT i = 0; i < numU; ++i) {
				UINT a = 1 + j * numU + i;
				UINT b = a + 1;
				if (i == numU - 1) { b -= numU; }
				UINT c = b - numU;
				UINT d = a - numU;
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