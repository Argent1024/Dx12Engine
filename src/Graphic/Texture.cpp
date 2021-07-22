#include "stdafx.h"
#include "Texture.h"
#include "Utility/Logger.h"
#include "CommandQueue.h"

#include <WICTextureLoader.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb/stb_image_write.h"    

namespace 
{
	// Copy from Dartmouth CS 73/273 Computational Aspects of Digital Photography C++ basecode.
	std::string getExtension(const std::string &filename)
	{
		if (filename.find_last_of(".") != std::string::npos)
			return filename.substr(filename.find_last_of(".") + 1);
		return "";
	}


}

namespace Graphic {
	void LoadChessBoard(const UINT width, const UINT height, const UINT pixelSize, std::vector<UINT8>& data)
	{
		const UINT rowPitch = width * pixelSize;
		const UINT textureSize = rowPitch * height;
		data.resize(textureSize);
		const UINT cellPitch = rowPitch >> 3;        // The width of a cell in the checkboard texture.
		const UINT cellHeight = width >> 3;    // The height of a cell in the checkerboard texture.
		// Load Chess Board
		for (UINT n = 0; n < textureSize; n += pixelSize)
		{
			UINT x = n % rowPitch;
			UINT y = n / rowPitch;
			UINT i = x / cellPitch;
			UINT j = y / cellHeight;

			if (i % 2 == j % 2)
			{
				data[n] = 0x00;        // R
				data[n + 1] = 0x00;    // G
				data[n + 2] = 0x00;    // B
				data[n + 3] = 0xff;    // A
			}
			else
			{
				data[n] = 0xff;        // R
				data[n + 1] = 0xff;    // G
				data[n + 2] = 0xff;    // B
				data[n + 3] = 0xff;    // A
			}
			
		}
	}


	ptrRootSignature Texture::TextureRootSignature = nullptr;
	ptrComputePSO Texture::MipMapPSO = nullptr;
	ptrComputePSO TextureCube::CreateFromTex2DPSO = nullptr;


	void Texture::TRootSignature::_Initialize()	{
		ID3D12Device* device = Engine::GetDevice();
		D3D12_FEATURE_DATA_ROOT_SIGNATURE featureData = {};
		// This is the highest version the sample supports. If CheckFeatureSupport succeeds, the HighestVersion returned will not be greater than this.
		featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_1;
		if (FAILED(device->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE, &featureData, sizeof(featureData))))
		{
			featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_0;
		}

		CD3DX12_DESCRIPTOR_RANGE1 ranges[3];
		CD3DX12_ROOT_PARAMETER1 rootParameters[1];
		

		// Descriptor table 0 for Camera / Renderpass
		ranges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, nCBV, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DESCRIPTORS_VOLATILE); // b0
		ranges[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, nSRV, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DESCRIPTORS_VOLATILE); // t0 
		ranges[2].Init(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, nUAV, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DESCRIPTORS_VOLATILE); // u0-ux

		rootParameters[0].InitAsDescriptorTable(3, &ranges[0], D3D12_SHADER_VISIBILITY_ALL);

		D3D12_STATIC_SAMPLER_DESC sampler = {};
        // sampler.Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;
        sampler.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
		sampler.AddressU = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
        sampler.AddressV = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
        sampler.AddressW = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
        sampler.MipLODBias = 0;
        sampler.MaxAnisotropy = 0;
        sampler.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
        sampler.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
        sampler.MinLOD = 0.0f;
        sampler.MaxLOD = D3D12_FLOAT32_MAX;
        sampler.ShaderRegister = 0;
        sampler.RegisterSpace = 0;
        sampler.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
			
		D3D12_ROOT_SIGNATURE_FLAGS rootSignatureFlags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
			
		CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
		rootSignatureDesc.Init_1_1(_countof(rootParameters), rootParameters, 1, &sampler, rootSignatureFlags);

		ComPtr<ID3DBlob> signature;
		ComPtr<ID3DBlob> error;
		ThrowIfFailed(D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, featureData.HighestVersion, &signature, &error));
		ThrowIfFailed(device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&m_RootSignature)));
		NAME_D3D12_OBJECT(m_RootSignature);
	}

	void Texture2D::CreateMipMap(ptrTex2D texture, UINT srcMip, UINT numMip) 
	{	
		assert((texture->m_Type & TEXTURE_SRV) && (texture->m_Type & TEXTURE_UAV) && 
			"Createing texture without srv/uav enabled");

		ptrGMem textureResouce = texture->m_buffer;

		// Create root signatue & PSO
		// TODO reuse root signature from else where
		struct MipmapCB {
			UINT SrcMipLevel;	// Texture level of source mip
			UINT NumMipLevels;	// Number of OutMips to write: [1, 4]
			DirectX::XMFLOAT2 TexelSize;	// 1.0 / OutMip1.Dimensions
		};
		UINT Width = texture->m_textureDesc.Width;
		UINT Height = texture->m_textureDesc.Height;
		UINT16 maxMiplevels = texture->MaxMipLevels();
		assert(numMip <= 4 && numMip + srcMip < maxMiplevels &&
			"Creating mipmaps larger than texture's size");

		MipmapCB cbData;
		cbData.SrcMipLevel = srcMip;
		cbData.NumMipLevels = numMip;
		cbData.TexelSize = {2.0f / Width, 2.0f / Height};


		// Create Constant buffer
		const UINT MatmapCBSize = CalculateConstantBufferByteSize(sizeof(MipmapCB));
		ptrGBuffer buffer = GPU::MemoryManager::CreateGBuffer();
		buffer->Initialize(MatmapCBSize);
		ConstantBuffer cb;
		cb.Initialze(buffer, MatmapCBSize);
		cb.copyData(&cbData);

		// ***TODO*** Since this function will only be called in load assert now,
		// all will happen before the first frame now, so it's fine for now
		// but need to create a GPU heap for this later
		Graphic::DescriptorHeap* heap = Engine::GetInUseHeap();

		InitTextureRootSignature();

		if (MipMapPSO == nullptr) {

			const std::wstring MipCsPath =  L"D:\\work\\tEngine\\Shaders\\MipmapCS.hlsl";
			
			MipMapPSO = std::make_shared<Graphic::ComputePSO>(MipCsPath);
			MipMapPSO->SetRootSigature(TextureRootSignature->GetRootSignature());

			MipMapPSO->Initialize();
		}	

		DescriptorTable MipDTable(16, heap);
	
		// 0: CBV, 1: SRV, 2-Miplevels+2: UAV
		cb.CreateView(&MipDTable, 0);
		texture->CreateSRV(&MipDTable, 1);
		for (UINT16 i = 0; i < numMip; ++i) {
			texture->CreateUAV(&MipDTable, 2 + i, 1 + srcMip + i);
		}


		// Since this descriptor table is created at the heap where shader can access, we don't need to bind it again
		CD3DX12_GPU_DESCRIPTOR_HANDLE tableHandle = MipDTable.GetSlotGPU(0);


		Graphic::CommandList ctx(D3D12_COMMAND_LIST_TYPE_COMPUTE);
		ComputeCommandManager.InitCommandList(&ctx);
		ctx.SetDescriptorHeap(*heap);
		ctx.SetPipelineState(MipMapPSO);
		ctx.SetComputeRootSignature(TextureRootSignature);
		ctx.ResourceBarrier(*textureResouce, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

		ctx.SetComputeRootDescriptorTable(0, tableHandle);
		
		// We are using group 8 x 8x 1, dividing 16 will give number of threads = pixels at miplevel1
		ctx.Dispatch(Width /16, Height / 16 , 1);

		ctx.ResourceBarrier(*textureResouce, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COPY_DEST);

		ComputeCommandManager.ExecuteCommandList(&ctx);
		ComputeCommandManager.End();
		ComputeCommandManager.Start();
	}

	/*Texture1D::Texture1D(UINT elementNum, UINT stride, UINT type) 
		:TextureSingle(type), m_elementNum(elementNum), m_stride(stride), m_totalSize(m_elementNum * m_stride)
	{
		// TODO consider flags
		D3D12_RESOURCE_FLAGS flag;
		switch (type)
		{
		case Graphic::TEXTURE_UAV:
			flag = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
			break;
		default:
			flag = D3D12_RESOURCE_FLAG_NONE;
			break;
		}

		m_buffer = GPU::MemoryManager::CreateGBuffer();
		m_buffer->Initialize(m_totalSize);
	}

	Texture1D::Texture1D(UINT totalSize) : 
		TextureSingle(TEXTURE_CBV), m_totalSize(totalSize)
	{
		D3D12_RESOURCE_FLAGS flag = D3D12_RESOURCE_FLAG_NONE;

		m_buffer = GPU::MemoryManager::CreateGBuffer();
		m_buffer->Initialize(m_totalSize);
	}

	void Texture1D::CreateCBV(DescriptorTable* table, UINT tableIndex) 
	{
		/*if (!m_CBV) {
			m_CBV = new ConstantBuffer(m_buffer, m_totalSize);
		}
		if (table) {
			m_CBV->CreateView(*table, tableIndex);
		} else {
			m_CBV->CreateRootView();
		}
	}

	void Texture1D::CreateSRV(DescriptorTable* table, UINT tableIndex) 
	{
	
		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.Format = DXGI_FORMAT_UNKNOWN;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
		srvDesc.Buffer.FirstElement = 0;
		srvDesc.Buffer.NumElements = m_elementNum;
		srvDesc.Buffer.StructureByteStride = m_stride;
		srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
		
		m_SRV.CreateView(table, tableIndex, m_buffer->GetResource(), &srvDesc);
	}

	void Texture1D::CreateUAV(DescriptorTable* table, UINT tableIndex) 
	{
		D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
		uavDesc.Format = DXGI_FORMAT_UNKNOWN;
		uavDesc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
		uavDesc.Buffer.FirstElement = 0;
		uavDesc.Buffer.NumElements = m_elementNum;
		uavDesc.Buffer.StructureByteStride = m_stride;
		uavDesc.Buffer.CounterOffsetInBytes = 0;
		uavDesc.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_NONE;

		m_UAV.CreateView(table, tableIndex, m_buffer->GetResource(), &uavDesc);
	}
	*/


	Texture2D::Texture2D(UINT width, UINT height, UINT type, DXGI_FORMAT format, UINT miplevels, bool loadChessBoard)
		: Texture(type)
	{
		m_textureDesc = CD3DX12_RESOURCE_DESC::Tex2D(format, width, height, ArraySize, miplevels);
		Initialize(m_textureDesc);

		if (loadChessBoard) {
			assert(format == DXGI_FORMAT_R8G8B8A8_UNORM && "Can only load chessboard for r8g8b8a8");
			std::vector<UINT8> data;
			LoadChessBoard(width, height, 4, data);
			D3D12_SUBRESOURCE_DATA textureData = 
			CreateTextureData({ (UINT)width, (UINT)height, 4, 1}, &data[0]);
			UploadTexture(&textureData);
		}
	}

	Texture2D::Texture2D(std::string& filename, UINT type, UINT miplevels) 
		: Texture(type)
	{	
		// Load Chess Board

		unsigned char* data = nullptr;
		
		ImageMetadata metadata = LoadFromImage(filename, data);
		metadata.channel = 4;
		metadata.channelSize = 1;

		D3D12_SUBRESOURCE_DATA texData = CreateTextureData(metadata, data);
		m_textureDesc =
			CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_R8G8B8A8_UNORM, 
				metadata.width, metadata.height, 
				ArraySize, miplevels);

		Initialize(m_textureDesc);

		UploadTexture(&texData);

		// Using stb_image so need to free data here
		// TODO Thinking sometime we need store the data in memory = =
		stbi_image_free(data);
		
	}
	
	void Texture2D::Initialize(CD3DX12_RESOURCE_DESC& texDesc) 
	{
		//TODO flags not tested
		m_buffer = GPU::MemoryManager::CreateTBuffer();
		if (m_Type & TEXTURE_DSV) {
			// Change Texture format to D32
			texDesc.Format = DXGI_FORMAT_D32_FLOAT;
			texDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

			D3D12_RESOURCE_STATES initState = D3D12_RESOURCE_STATE_DEPTH_WRITE;

			// provide a clear value if we are using texture as a depth buffer
			D3D12_CLEAR_VALUE clearValue = {};
			clearValue.Format = DXGI_FORMAT_D32_FLOAT;
			clearValue.DepthStencil.Depth = 1.0f;
			clearValue.DepthStencil.Stencil = 0;

			m_buffer->Initialize(&texDesc, &clearValue);
			// Change back to R32 from D32
			texDesc.Format = DXGI_FORMAT_R32_FLOAT;
		} else if(m_Type & TEXTURE_UAV) {
			texDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
			m_buffer->Initialize(&texDesc);
		} else {
			m_buffer->Initialize(&texDesc);
		}
	}	

	ImageMetadata Texture2D::LoadFromImage(std::string& filename, unsigned char*& data) 
	{
		// stb image only support 8 bit
		ImageMetadata metadata = {};
		data = stbi_load(filename.c_str(), (int*)&metadata.width, (int*)&metadata.height,(int*) &metadata.channel, 4);
		if (data == nullptr) {
			std::string errormsg = "ERROR when reading image " + filename;
			Logger::Log(errormsg);
		}
		metadata.channel = metadata.channel; // Store pixel size in Byte
		return metadata;
		// LoadWICTextureFromFile()
	}

	void Texture2D::CreateSRV(DescriptorTable* table, UINT tableIndex, UINT MostDetailedMip, UINT MipLevels) 
	{
		assert(MostDetailedMip < MaxMipLevels());
		assert(MipLevels == -1 || MostDetailedMip + MipLevels < MaxMipLevels());

		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.Format = m_textureDesc.Format;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		
		srvDesc.Texture2D.MipLevels = MipLevels;
		srvDesc.Texture2D.MostDetailedMip = MostDetailedMip;

		this->_CreateSRV(&srvDesc, table, tableIndex);
	}
	
	void Texture2D::CreateUAV(DescriptorTable* table, UINT tableIndex, UINT MipLevels) 
	{
		assert(MipLevels < MaxMipLevels());

		D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
		uavDesc.Format = m_textureDesc.Format;
		uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
		uavDesc.Texture2D.MipSlice = MipLevels;

		this->_CreateUAV(&uavDesc, table, tableIndex);
	}


	void Texture2D::CreateDSV() 
	{
		// Describe SRV
		D3D12_DEPTH_STENCIL_VIEW_DESC  dsvDesc = {};
		dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
		dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
		dsvDesc.Flags = D3D12_DSV_FLAG_NONE;

		m_DSV.CreateView(m_buffer->GetResource(), &dsvDesc);
	}

	TextureCube::TextureCube(UINT resolution, UINT type, DXGI_FORMAT format, UINT16 miplevels) 
		: Texture(type)
	{
		TextureDescHelper(resolution, miplevels);
		Initialize();
	}
	
	void TextureCube::TextureDescHelper(UINT resolution, UINT16 miplevels) 
	{
		m_textureDesc = {};
		m_textureDesc.MipLevels = miplevels;
		m_textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		m_textureDesc.Width = resolution;
		m_textureDesc.Height = resolution;
		m_textureDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
		m_textureDesc.DepthOrArraySize = 6; 
		m_textureDesc.SampleDesc.Count = 1;
		m_textureDesc.SampleDesc.Quality = 0;
		m_textureDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;

		if (m_Type & TEXTURE_UAV) m_textureDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
	}

	void TextureCube::Initialize()
	{
		m_buffer = GPU::MemoryManager::CreateTBuffer();
		m_buffer->Initialize(&m_textureDesc);
	}


	void TextureCube::CreateFromTex2D(ptrTex2D tex2d)
	{
		assert((tex2d->GetType() & TEXTURE_SRV) && (GetType() & TEXTURE_UAV));
		InitTextureRootSignature();

		// Create root signatue & PSO
		if (CreateFromTex2DPSO == nullptr) {
			const std::wstring Tex2d_Cube_CsPath =  L"D:\\work\\tEngine\\Shaders\\GenerateCubeMap.hlsl";
			CreateFromTex2DPSO = std::make_shared<Graphic::ComputePSO>(Tex2d_Cube_CsPath);
			CreateFromTex2DPSO->SetRootSigature(TextureRootSignature->GetRootSignature());
			CreateFromTex2DPSO->Initialize();
		}

		struct ConstBufferData {
			FLOAT InvResolution;
			BOOL Padding[3];
		};

		UINT resolution = GetResolution();
		ptrGMem textureResouce = this->m_buffer;

		ConstBufferData cbData;
		cbData.InvResolution = 1.0f / resolution;

		const UINT CBSize = CalculateConstantBufferByteSize(sizeof(ConstBufferData));
		ptrGBuffer buffer = GPU::MemoryManager::CreateGBuffer();
		buffer->Initialize(CBSize);

		ConstantBuffer cb;
		cb.Initialze(buffer, CBSize);
		cb.copyData(&cbData);

		// ***TODO*** Move to another heap
		Graphic::DescriptorHeap* heap = Engine::GetInUseHeap();

		DescriptorTable DTable(16, heap); 
		// 0 CbV, 1 SRV, 2 UAV
		cb.CreateView(&DTable, 0);
		tex2d->CreateSRV(&DTable, 1);
		this->CreateUAV(&DTable, 2);


		// Record commands
		// Since this descriptor table is created at the heap where shader can access, we don't need to bind it again
		CD3DX12_GPU_DESCRIPTOR_HANDLE tableHandle = DTable.GetSlotGPU(0);


		Graphic::CommandList ctx(D3D12_COMMAND_LIST_TYPE_COMPUTE);
		ComputeCommandManager.InitCommandList(&ctx);
		ctx.SetDescriptorHeap(*heap);
		ctx.SetPipelineState(CreateFromTex2DPSO);
		ctx.SetComputeRootSignature(TextureRootSignature);
		ctx.ResourceBarrier(*textureResouce, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

		ctx.SetComputeRootDescriptorTable(0, tableHandle);
		
		// We are using group 8 x 8 x 1 
		ctx.Dispatch(resolution / 8, resolution / 8, 1);

		ctx.ResourceBarrier(*textureResouce, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COPY_DEST);

		ComputeCommandManager.ExecuteCommandList(&ctx);
		ComputeCommandManager.End();
		ComputeCommandManager.Start();

	}

	void TextureCube::CreateSRV(DescriptorTable* table, UINT tableIndex,  UINT MostDetailedMip, UINT MipLevels) 
	{
		assert(MostDetailedMip < MaxMipLevels());
		assert(MipLevels == -1 || MostDetailedMip + MipLevels < MaxMipLevels());

		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.Format = m_textureDesc.Format;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
	
		srvDesc.TextureCube.MipLevels = MipLevels;
		srvDesc.TextureCube.MostDetailedMip = MostDetailedMip;
		
		this->_CreateSRV(&srvDesc, table, tableIndex);

	}

	void TextureCube::CreateUAV(DescriptorTable* table, UINT tableIndex, UINT MipLevels) 
	{
		assert(MipLevels < MaxMipLevels());

		D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
		uavDesc.Format = m_textureDesc.Format;
		uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2DARRAY;
		uavDesc.Texture2DArray.MipSlice = MipLevels;
		uavDesc.Texture2DArray.FirstArraySlice = 0;
		uavDesc.Texture2DArray.ArraySize = 6;
		uavDesc.Texture2DArray.PlaneSlice = 0; // TODO?

	
		this->_CreateUAV(&uavDesc, table, tableIndex);
	}

}