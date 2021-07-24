#include "stdafx.h"
#include "Material.h"
#include "CommandQueue.h"

using namespace Graphic;

namespace Game {
	ptrTex2D PrincipleMaterial::brdfTex = nullptr;

	void PrincipleMaterial::InitBRDFTexture() {
		if (brdfTex) { return; }

		UINT type = TEXTURE_SRV | TEXTURE_UAV;
		DXGI_FORMAT format = DXGI_FORMAT_R16G16_FLOAT;
		brdfTex = std::make_shared<Graphic::Texture2D>(BRDF_TEX_Width, BRDF_TEX_Height, type, format);


		const std::wstring cs_path =  L"D:\\work\\tEngine\\Shaders\\GenerateCubeMap.hlsl";
		ptrComputePSO pso = std::make_shared<Graphic::ComputePSO>(cs_path);
		ptrRootSignature rootsignature = Texture::GetTextureRootSignature();
		pso->SetRootSigature(rootsignature->GetRootSignature());
		pso->Initialize();


		struct ConstBufferData {
			UINT width;
			UINT height;
			BOOL padding[2];
		};
		constexpr UINT CBSize = CalculateConstantBufferByteSize(sizeof(ConstBufferData));

		ConstBufferData cbData {BRDF_TEX_Width, BRDF_TEX_Height};

		ptrGBuffer buffer = GPU::MemoryManager::CreateGBuffer();
		buffer->Initialize(CBSize);

		ConstantBuffer cb;
		cb.Initialze(buffer, CBSize);
		cb.copyData(&cbData);


		// ***TODO*** Move to another heap
		Graphic::DescriptorHeap* heap = Engine::GetInUseHeap();
		DescriptorTable DTable(16, heap); 
		// 0 CbV, 1 Empty, 2 UAV
		cb.CreateView(&DTable, 0);
		brdfTex->CreateUAV(&DTable, 2);

		// Record commands
		// Since this descriptor table is created at the heap where shader can access, we don't need to bind it again
		CD3DX12_GPU_DESCRIPTOR_HANDLE tableHandle = DTable.GetSlotGPU(0);

		ptrTBuffer texbuffer = brdfTex->GetBuffer();

		Graphic::CommandList ctx(D3D12_COMMAND_LIST_TYPE_COMPUTE);
		ComputeCommandManager.InitCommandList(&ctx);
		ctx.SetDescriptorHeap(*heap);
		ctx.SetPipelineState(pso);
		ctx.SetComputeRootSignature(rootsignature);
		ctx.ResourceBarrier(*texbuffer, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

		ctx.SetComputeRootDescriptorTable(0, tableHandle);
		
		// We are using group 8 x 8 x 1 
		ctx.Dispatch(BRDF_TEX_Width / 8, BRDF_TEX_Height / 8, 1);

		ctx.ResourceBarrier(*texbuffer, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COPY_DEST);

		ComputeCommandManager.ExecuteCommandList(&ctx);
		ComputeCommandManager.End();
		ComputeCommandManager.Start();
	}


	PrincipleMaterial::PrincipleMaterial() 
	{
		assert(sizeof(PrincipleMaterial::MatData) % 16 == 0);

		// Create the BRDF Texture
		InitBRDFTexture();

		ptrGBuffer buffer = GPU::MemoryManager::CreateGBuffer();
		buffer->Initialize(MatCBVSize);
		m_MatCBV.Initialze(buffer, MatCBVSize);

	}

	void PrincipleMaterial::BindMaterialAt(Graphic::DescriptorTable& table) 
	{
		m_MatCBV.CreateView(&table, MatCBV);

		if (m_MatData.CTexture) { BindTexture(DiffuseTex, table); }
		if (m_MatData.NTexture) { BindTexture(NormalTex, table); }
		
		// TODO other textures
	}

	void PrincipleMaterial::BindTexture(SlotMaps slot, Graphic::DescriptorTable& table) 
	{
		switch (slot)
		{
		case Game::PrincipleMaterial::ObjectCBV:
			break;
		case Game::PrincipleMaterial::MatCBV:
			break;
		case Game::PrincipleMaterial::DiffuseTex:
			assert(m_DiffuseTex != nullptr && "Haven't set texture for diffuse color");
			m_DiffuseTex->CreateSRV(&table, DiffuseTex);
			break;
		case Game::PrincipleMaterial::NormalTex:
			break;
		case Game::PrincipleMaterial::RoughnessTex:
			break;
		default:
			break;
		}
	}

	void PrincipleMaterial::SetTexture(SlotMaps texType, ptrTex2D tex)
	{
		switch (texType)
		{
		case Game::PrincipleMaterial::ObjectCBV:
			assert(FALSE && "Why ObjectCBV in set texture?");
			break;
		case Game::PrincipleMaterial::MatCBV:
			assert(FALSE && "Why MatCBV in set texture?");
			break;
		case Game::PrincipleMaterial::DiffuseTex:
			m_MatData.CTexture = tex != nullptr;
			m_DiffuseTex = tex;
			break;
		case Game::PrincipleMaterial::NormalTex:
			m_MatData.NTexture = tex != nullptr;
			m_NormalTex = tex;
			break;
		case Game::PrincipleMaterial::RoughnessTex:
			break;
		default:
			break;
		}
	}
}


void UI::PrincipleMaterialUIWindow(std::shared_ptr<Game::PrincipleMaterial> mat, const std::string window_name) {
		Game::PrincipleMaterial::MatData& data = mat->GetData();
		ImGui::Begin(window_name.c_str());
		bool update = false;

		update = update | ImGui::ColorEdit3("BaseColor", (float*)&data.BaseColor); 
		update = update | ImGui::SliderFloat("Roughness", &data.Roughness, 0.0, 1.0); 
		update = update | ImGui::SliderFloat("Metallic", &data.Metallic, 0.0, 1.0); 
		update = update | ImGui::SliderFloat("Specular", &data.Specular, 0.0, 1.0); 

		if (update) { mat->UploadCBV(); }
         

		ImGui::End();
}
