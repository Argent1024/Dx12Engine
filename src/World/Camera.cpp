#include "Camera.h"

namespace Game {
	void ProjectiveCamera::CreateCBV() 
	{
		const UINT cbSize = CalculateConstantBufferByteSize(sizeof(DirectX::XMMATRIX));

		ptrGPUMem gpumem = Engine::MemoryAllocator.CreateCommittedBuffer(cbSize, D3D12_HEAP_TYPE_UPLOAD);
		m_RootCBV = std::make_shared<Graphic::RootConstantBuffer>(gpumem, cbSize);
	}

	void ProjectiveCamera::UseCamera(Graphic::CommandList& commandList, const Transform& ModelTransformation) {
		commandList.SetViewPorts(&m_Viewport);
		commandList.SetScissorRects(&m_ScissorRect);
		//TODO Camera space transform
		// Root Signature is used for projective camera should use b0 as the view matrix
		//commandList.SetGraphicsRootConstants(0, sizeof(DirectX::XMMATRIX) / 4,  &(DirectX::XMMATRIX)m_ViewProjective);

		// Calculate & set model_view_projective matrix
		Transform mvp = ModelTransformation * m_ViewProjective;
		m_RootCBV->copyData(&(DirectX::XMMATRIX)mvp);
		// TODO set to b0, however this knowledge should only be known by the RootSignature
		commandList.SetGraphicRootCBV(*m_RootCBV, 0);
	}
}