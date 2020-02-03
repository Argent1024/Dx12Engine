#pragma once

#include "DXHelper.h"


namespace Graphic {

	class RootSignature {
	public:
		ID3D12RootSignature* GetRootSignature() const { return m_RootSignature.Get(); }

		virtual void Initialize(ComPtr<ID3D12Device> device) = 0;


	protected:
		ComPtr<ID3D12RootSignature> m_RootSignature;
	};

	

	// Simple root signature
	// Store View Mat
	class NaiveRootSignature : public RootSignature
	{
	public:
		void Initialize(ComPtr<ID3D12Device> device) 
		{
			D3D12_FEATURE_DATA_ROOT_SIGNATURE featureData = {};
			// This is the highest version the sample supports. If CheckFeatureSupport succeeds, the HighestVersion returned will not be greater than this.
			featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_1;
			if (FAILED(device->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE, &featureData, sizeof(featureData))))
			{
				featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_0;
			}

			CD3DX12_ROOT_PARAMETER1 rootParameters[1];
			{
				// Create Root CBV at regisiter 0, space 0
				rootParameters[0].InitAsConstants(sizeof(DirectX::XMMATRIX) / 4,  0, 0, D3D12_SHADER_VISIBILITY_VERTEX);
			}
			D3D12_ROOT_SIGNATURE_FLAGS rootSignatureFlags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
			
			 CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
			rootSignatureDesc.Init_1_1(_countof(rootParameters), rootParameters, 1, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

			ComPtr<ID3DBlob> signature;
			ComPtr<ID3DBlob> error;
			ThrowIfFailed(D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, featureData.HighestVersion, &signature, &error));
			ThrowIfFailed(device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&m_RootSignature)));
			NAME_D3D12_OBJECT(m_RootSignature);
		}


	};
}