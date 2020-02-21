#pragma once
#include "GraphicCore.h"
#include "CommandQueue.h"
#include "../World/Camera.h"
#include "../World/GObject.h"


namespace Samples {

	using namespace Graphic;
	using namespace Game;

	struct PointVertex
    {
        XMFLOAT4 color;
    };

	struct ParticleData
	{
		XMFLOAT4 position;
	};

	class ParticleTest : public GraphicCore {
	public:
		const size_t NumRow = 15;//Particle in one row
		const size_t NumParticles = NumRow * NumRow * NumRow;
		const float RowLength = 0.5;
		const float Delta = 2.0f * RowLength / NumRow;
		const XMFLOAT4 LeftCorner = XMFLOAT4(-RowLength, -RowLength, -RowLength, 0.0);

		ParticleTest(UINT t_width, UINT t_height,  LPCTSTR t_title=L"playground") 
			: GraphicCore(t_width, t_height, t_title), 
			  m_Camera(t_width, t_height, Vector3(0.0, 0.0, 1.0), Vector3(0.0, 0.0, 0.0), Vector3(0.0, 1.0, 0.0)) {}

		void Init(const HWND m_appHwnd) override;
		void Render() override;

		inline XMFLOAT4 GetPosition(UINT index) 
		{
			UINT x = index / (NumRow * NumRow);
			UINT y = index / NumRow - x * NumRow;
			UINT z = index % NumRow;
			XMFLOAT4 Pos(Delta * x - RowLength, Delta * y - RowLength, Delta * z - RowLength, 0.0);
			return Pos;
		}

		void GenerateParticlesTexture(std::vector<ParticleData>& data) 
		{
			data.resize(NumParticles);
			for (UINT i = 0; i < NumParticles; i++) 
			{
				XMFLOAT4 pos = GetPosition(i);
				data[i] = { pos };
			}
		}

	private:
		void CreateGameObject();

		std::shared_ptr<VertexBuffer> m_vertexBuffer;
		std::shared_ptr<IndexBuffer> m_indexBuffer;

		DepthBuffer* m_depthBuffer;
		SwapChain* m_swapChain;

		ptrRootSigature m_GraphicRootSignature;
		ptrPSO m_GraphicPSO;

		ptrMesh m_Mesh;
		ptrTexture m_texture;
		ptrMaterial m_Material;
		GObject m_ParticleObject;

		ProjectiveCamera m_Camera;
	};
}

