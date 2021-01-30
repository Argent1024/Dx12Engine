#include "GraphicCore.h"
#include "GPUMemory.h"
#include "DescriptorHeap.h"
#include "CommandQueue.h"
#include "Utility/Input.h"
#include "Utility/Logger.h"

// Helper function for acquiring the first available hardware adapter that supports Direct3D 12.
// If no such adapter can be found, *ppAdapter will be set to nullptr.
void Engine::CreateHardwareAdapter(IDXGIFactory2* pFactory, IDXGIAdapter1** ppAdapter)
{
	ComPtr<IDXGIAdapter1> adapter;
	*ppAdapter = nullptr;
	for (UINT adapterIndex = 0; DXGI_ERROR_NOT_FOUND != pFactory->EnumAdapters1(adapterIndex, &adapter); ++adapterIndex)
	{
		DXGI_ADAPTER_DESC1 desc;
		adapter->GetDesc1(&desc);

		if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
		{
			// Don't select the Basic Render Driver adapter.
			// If you want a software adapter, pass in "/warp" on the command line.
			continue;
		}
			// Check to see if the adapter supports Direct3D 12, but don't create the
		// actual device yet.
		if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), nullptr)))
		{
			break;
		}
	}

	*ppAdapter = adapter.Detach();
}

void Engine::CreateDevice() {
	bool useWarpDevice = FALSE;

	ThrowIfFailed(CreateDXGIFactory1(IID_PPV_ARGS(&dxFactory)));
	ThrowIfFailed(CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&dxFactory)));
			
	if (useWarpDevice) {
		ComPtr<IDXGIAdapter> warpAdapter;
		ThrowIfFailed(dxFactory->EnumWarpAdapter(IID_PPV_ARGS(&warpAdapter)));

		ThrowIfFailed(D3D12CreateDevice(
			warpAdapter.Get(),
			D3D_FEATURE_LEVEL_11_0,
			IID_PPV_ARGS(&dxDevice)
		));
	} else {
		ComPtr<IDXGIAdapter1> hardwareAdapter;
		CreateHardwareAdapter(dxFactory.Get(), &hardwareAdapter);

		ThrowIfFailed(D3D12CreateDevice(
			hardwareAdapter.Get(),
			D3D_FEATURE_LEVEL_11_0,
			IID_PPV_ARGS(&dxDevice)
		));
	}
}

void Engine::EnableDebug()
{
	ComPtr<ID3D12Debug> debugController;
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
	{
		debugController->EnableDebugLayer();
		// Enable additional debug layers.
		dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
	}		
}	


// Graphic::GPU::MemoryAllocator Engine::MemoryAllocator;
Graphic::CommandManager CopyHelper(1);
Graphic::CommandManager GraphicsCommandManager(2);

void Engine::InitializeEngine() {
	// **** Graphic Stuff ******//
	EnableDebug();
	CreateDevice();

	CopyHelper.Initialize();
	GraphicsCommandManager.Initialize();

	Graphic::DescriptorHeap* initHeap = Engine::GetInitHeap();
	initHeap->Initialize();
	Graphic::DescriptorHeap* useHeap = Engine::GetInUseHeap();
	useHeap->Initialize();

	Graphic::DescriptorHeap* dsvHeap = Engine::GetDSVHeap();
	dsvHeap->Initialize();

	Graphic::DescriptorHeap* rtvHeap = Engine::GetRTVHeap();
	rtvHeap->Initialize();
	// **** Graphic Stuff End ******//

	// ***** Utility stuff start *******//
	Logger::Initialize();
}

namespace Engine 
{

	ComPtr<ID3D12Device> dxDevice;
	ComPtr<IDXGIFactory4> dxFactory;
	UINT dxgiFactoryFlags;
	// TODO read config file
	UINT NumDescriptors = 64;
	UINT NumDSV = 10;
	UINT NumRTV = 10;

	Graphic::DescriptorHeap InitHeap(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, NumDescriptors,  D3D12_DESCRIPTOR_HEAP_FLAG_NONE);
	Graphic::DescriptorHeap InUseHeap(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, NumDescriptors, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE);
	
	Graphic::DescriptorHeap DSVHeap(D3D12_DESCRIPTOR_HEAP_TYPE_DSV, NumDSV, D3D12_DESCRIPTOR_HEAP_FLAG_NONE);
	Graphic::DescriptorHeap RTVHeap(D3D12_DESCRIPTOR_HEAP_TYPE_RTV, NumRTV, D3D12_DESCRIPTOR_HEAP_FLAG_NONE);
}