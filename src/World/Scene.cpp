#include "Scene.h"

namespace Game {

	Scene::Scene(const HWND AppHwnd, const UINT width, const UINT height)
		: m_SwapChain(AppHwnd, width, height), m_depthBuffer(width, height),
		  m_Camera(width, height, Vector3(-3.0f, -1.0f, -12.f), Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f, 1.0f, 0.0f)),
		  m_MixturePass(1, width, height) // one Texture to mix in the mixture stage
	{

		m_SwapChain.Initialize(GraphicsCommandManager.GetCommadnQueue());
		m_Renderpass.Initialize();
		m_MixturePass.Initialize();

		// Create depth buffer & srv
		m_depthBuffer.Initialize(Graphic::TEXTURE_DSV | Graphic::TEXTURE_SRV);
		Graphic::Texture* depthTexture = m_depthBuffer.GetTexture();
		Graphic::DescriptorTable* table = m_MixturePass.GetDescriptorTable();
		depthTexture->CreateView(Graphic::TEXTURE_SRV, table, 0);
	
	}

	Scene::Scene(const HWND m_appHwnd, const UINT width, const UINT height, ProjectiveCamera& camera)
		: m_SwapChain(m_appHwnd, width, height), m_depthBuffer(width, height), m_Camera(camera), 
		  m_MixturePass(1, width, height) // one Texture to mix in the mixture stage
	{ }

	void Scene::SetCameraTransformation(Camera& camera) 
	{
		const Transform& view = camera.GetView();
		const Transform& proj = camera.GetToScreen();
		
		// Need to transpose
		XMStoreFloat4x4(&m_SceneData.projection, XMMatrixTranspose((XMMATRIX)proj));
		XMStoreFloat4x4(&m_SceneData.view, XMMatrixTranspose((XMMATRIX)view));
	}
	
	void Scene::AddGameObj(GObject* obj) 
	{
		m_ObjList.push_back(obj);
	}
}