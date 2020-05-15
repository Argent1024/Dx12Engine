#include "Camera.h"

namespace Game {
	void ProjectiveCamera::Initialize() 
	{
		const UINT cbSize = CalculateConstantBufferByteSize(sizeof(DirectX::XMMATRIX));

		ptrGPUMem gpumem = Engine::MemoryAllocator.CreateCommittedBuffer(cbSize, D3D12_HEAP_TYPE_UPLOAD);
		m_RootCBV = std::make_shared<Graphic::ConstantBuffer>(gpumem, cbSize, true); // We are creating a root descriptor
	}

	void ProjectiveCamera::UseCamera(Graphic::CommandList& commandList) 
	{
		assert(m_RootCBV != nullptr && "Should create cbv before use it");
		
		Graphic::DescriptorHeap* descriptorHeap = Engine::GetInUseHeap();
		UINT index = descriptorHeap->MallocHeap(1); // only one descriptor
		m_RootCBV->BindDescriptor(index);

		commandList.SetViewPorts(&m_Viewport);
		commandList.SetScissorRects(&m_ScissorRect);

		// Calculate & set model_view_projective matrix
		// Transform mvp( (Matrix4)model * (Matrix4)m_ViewProjective);
		m_RootCBV->copyData(&(DirectX::XMMATRIX)m_ViewProjective);

		// TODO set to b0, however this knowledge should only be known by the RootSignature
		commandList.SetGraphicRootCBV(m_RootCBV, 0);
	}

	void OrthonormalCamera::Initialize() 
	{
		const UINT cbSize = CalculateConstantBufferByteSize(sizeof(DirectX::XMMATRIX));

		ptrGPUMem gpumem = Engine::MemoryAllocator.CreateCommittedBuffer(cbSize, D3D12_HEAP_TYPE_UPLOAD);
		m_RootCBV = std::make_shared<Graphic::ConstantBuffer>(gpumem, cbSize, true); // We are creating a root descriptor
	}

	void OrthonormalCamera::UseCamera(Graphic::CommandList& commandList) 
	{
		assert(m_RootCBV != nullptr && "Should Create cbv before use it");
		
		Graphic::DescriptorHeap* descriptorHeap = Engine::GetInUseHeap();
		UINT index = descriptorHeap->MallocHeap(1); // only one descriptor
		m_RootCBV->BindDescriptor(index);

		commandList.SetViewPorts(&m_Viewport);
		commandList.SetScissorRects(&m_ScissorRect);

		// Calculate & set model_view_projective matrix
		//Transform mvp( (Matrix4)model * (Matrix4)m_ViewOrthormal);
		m_RootCBV->copyData(&(DirectX::XMMATRIX)m_ViewOrthormal);

		// TODO set to slot 0, however this knowledge should only be known by the RootSignature
		commandList.SetGraphicRootCBV(m_RootCBV, 0);
	}
}