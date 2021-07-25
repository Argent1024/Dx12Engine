#include "stdafx.h"
#include "EnvironmentMapping.h"
#include "CommandQueue.h"

using namespace Graphic;

namespace Game {
	ptrComputePSO EnvironmentMap::RoughnessMipmapPSO = nullptr;


	EnvironmentMap::EnvironmentMap(UINT resolution, UINT texType, DXGI_FORMAT format, UINT16 miplevels) 
		: TextureCube(resolution, texType, format, miplevels)
	{

	}
	
	void EnvironmentMap::CalculatePrefilterEnvMap() {
		GetTextureRootSignature();

		if (RoughnessMipmapPSO == nullptr) {
		
			const std::wstring CsPath = L"D:\\work\\tEngine\\Shaders\\PBR\\PbrPrefilterEnvMapCS.hlsl";
			RoughnessMipmapPSO = std::make_shared<Graphic::ComputePSO>(CsPath);
			RoughnessMipmapPSO->SetRootSigature(TextureRootSignature->GetRootSignature());

			RoughnessMipmapPSO->Initialize();
		}

		UINT resolution =this->GetResolution();

		struct ConstBufferData {
			UINT NumRoughness;
			FLOAT InvResolution;
			BOOL padding[2];
		};	
		ConstBufferData cbData;
		cbData.NumRoughness = this->MaxMipLevels() - 1;
		cbData.InvResolution = 1.0 / resolution;

		
		ConstantBuffer<ConstBufferData> cb(cbData);
		cb.Initialize();
		cb.UpdateData();

		Graphic::DescriptorHeap* heap = Engine::GetInUseHeap();


		DescriptorTable DTable(16, heap);
		// 0: CBV, 1: SRV, 2-MipLevels+2:UAV
		cb.CreateView(&DTable, 0);
		this->CreateSRV(&DTable, 1);
		for (UINT16 i = 0; i < cbData.NumRoughness; ++i) {
			this->CreateUAV(&DTable, 2 + i, 1 + i);
		}

		// Since this descriptor table is created at the heap where shader can access, we don't need to bind it again
		CD3DX12_GPU_DESCRIPTOR_HANDLE tableHandle = DTable.GetSlotGPU(0);


		Graphic::CommandList ctx(D3D12_COMMAND_LIST_TYPE_COMPUTE);
		ComputeCommandManager.InitCommandList(&ctx);
		ctx.SetDescriptorHeap(*heap);
		ctx.SetPipelineState(RoughnessMipmapPSO);
		ctx.SetComputeRootSignature(TextureRootSignature);
		ctx.ResourceBarrier(*m_buffer, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

		ctx.SetComputeRootDescriptorTable(0, tableHandle);
		
		// We are using group 8 x 8x 1, dividing 16 will give number of threads = pixels at miplevel1
		ctx.Dispatch(resolution / 16, resolution / 16 , 1);

		ctx.ResourceBarrier(*m_buffer, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COPY_DEST);

		ComputeCommandManager.ExecuteCommandList(&ctx);
		ComputeCommandManager.End();
		ComputeCommandManager.Start();
	}


	SkyBoxAndEnvMap CreateSkyBoxFromFile(std::string path, UINT resolution, UINT16 miplevels) {
		assert(resolution % 8 == 0);

		UINT texType = TEXTURE_SRV | TEXTURE_UAV;
		DXGI_FORMAT format=DXGI_FORMAT_R8G8B8A8_UNORM;
		auto texCube = std::make_shared<Game::EnvironmentMap>(resolution, texType, format, miplevels);
		texCube->LoadFromImage(path);
		texCube->CalculatePrefilterEnvMap();
		// TODO roughness mipmap
		

		auto Mat = std::make_shared<PureColorCube>();
		Mat->SetTexture(0, texCube);
		Mat->UploadCBV();

		auto skybox = std::make_shared<SkyBox>();
		const float defaultScale = 10.f;
		skybox->SetTransform(
			Math::Transform({ defaultScale, 0, 0 }, { 0, defaultScale, 0 }, { 0, 0, defaultScale }, Math::Vector3(Math::kZero))
		);

		skybox->SetMaterial(Mat);
		return { skybox, texCube };
	}
}