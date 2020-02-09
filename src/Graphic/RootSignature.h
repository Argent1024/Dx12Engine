#pragma once

#include "GraphicCore.h"


namespace Graphic {
	// TODO better way to creat root signature layout
	class RootSignatureBinder {
	public:
		RootSignatureBinder() {}

	private:
		CD3DX12_ROOT_PARAMETER1 m_rootParameters;
		
	};


	// Default root signature
	class RootSignature {
	public:
		ID3D12RootSignature* GetRootSignature() const { return m_RootSignature.Get(); }

		// Create a default layout of root signature
		virtual void Initialize();

	protected:
		ComPtr<ID3D12RootSignature> m_RootSignature;
	};


}