#pragma once

#include "GraphicCore.h"
#include "Descriptor.h"
#include "DescriptorHeap.h"

#define ptrRootSignature std::shared_ptr<Graphic::RootSignature>

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
		// TODO move to the binder later
		enum ResourceBinding {
			CameraCBV,	
			ObjectCBV,
			SceneDTable,
			MaterialDTable,
			RenderpassDTable
		};

		RootSignature() { }
		
		ID3D12RootSignature* GetRootSignature() const { return m_RootSignature.Get(); }

		void Initialize() {
			if (m_initialized) { return; }
			m_initialized = true;
			_Initialize();
		}

		// Create a default layout of root signature
		virtual void _Initialize();

	protected:
		ComPtr<ID3D12RootSignature> m_RootSignature;
		bool m_initialized = false;
	};


	// Root Signature just containing a list of textures
	class MixRootSignature : public RootSignature  {
	public:
		MixRootSignature(const UINT num) : RootSignature(), m_textureCount(num) {}

		void _Initialize() override;

	private:
		UINT m_textureCount;
	};


}