#pragma once

#include "DXHelper.h"


namespace Graphic {

	class RootSignature {
	public:
		ID3D12RootSignature* GetRootSignature() const { return m_RootSignature.Get(); }

		virtual void Initialize(ComPtr<ID3D12Device> device,
								const CD3DX12_DESCRIPTOR_RANGE1* ranges,
								const CD3DX12_ROOT_PARAMETER1* rootParameters ) = 0;


	protected:
	
		ComPtr<ID3D12RootSignature> m_RootSignature;
	};
}