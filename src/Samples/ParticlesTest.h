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
        XMFLOAT4 position;
    };

	class ParticleTest : public GraphicCore {
	public:
		const size_t NumParticles = 1000;
		ParticleTest(UINT t_width, UINT t_height,  LPCTSTR t_title=L"playground") 
			: GraphicCore(t_width, t_height, t_title), m_Camera(t_width, t_height) {}

		void Init(const HWND m_appHwnd) override;
		void Render() override;

	private:
		void CreateGameObject();
		const std::wstring ShaderPath=L"D:\\work\\tEngine\\Shaders\\particle.hlsl";

		std::shared_ptr<VertexBuffer> m_vertexBuffer;
		std::shared_ptr<IndexBuffer> m_indexBuffer;

		SwapChain* m_swapChain;
		ptrRootSigature m_rootSignature;
		ptrPSO m_GraphicPSO;

		ptrGPUMem m_GPUmem;
		ptrMesh m_Mesh;
		ptrTexture m_texture;
		ptrMaterial m_Material;
		GObject m_ParticleObject;

		Camera m_Camera;
	};
}

