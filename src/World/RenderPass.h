#pragma once
#include "PipelineState.h"
#include "RootSignature.h"
#include "CommandList.h"

#include "Scene.h"

namespace Game {

	// Implement one explict render pass 
	// e.x. default render pass, screen space method, transpancy ... 
	class RenderPass {
	public:
		virtual void Initialize() = 0;

		//Prepare data for rendering(like put in the camera's data), 
		// should be called in main thread
		virtual void PrepareData(Scene& scene) = 0;

		// Record Commands for rendering
		// Should in worker thread
		virtual void Render(Graphic::CommandList& commandList, Scene& scene) = 0;

		Graphic::DescriptorTable* GetTable() { return m_DescriptorTable; }

	protected:
		ptrRootSignature m_rootSignature;
		ptrPSO m_PSO;

		Graphic::DescriptorTable* m_DescriptorTable;

		UINT m_ObjRenderType; // Record the render needed game objects' position in the scene class
	};
	

	/*************************************************************************************
			             Render pass implementation below
	*************************************************************************************/


	// Default render pass:
	//		Render simple material (no special effect, no reflection)
	//		Use Scene's camera & lighting.
	//		Required a shadow pass for light before this
	class DefaultRenderPass : public RenderPass {
	public:
		
		struct ConstBufferData 
		{
			XMFLOAT4X4 projection;
			XMFLOAT4X4 view;

			BOOL debugnormal = FALSE;
			BOOL debugpos = FALSE;
			BOOL padding0[2]; // Align

		};

		void Initialize() override;

		void PrepareData(Scene& scene) override;

		void Render(Graphic::CommandList& commandList, Scene& scene) override;

		ConstBufferData& GetCBVData() { return m_CBVData; }

	private:
		ConstBufferData m_CBVData;
		Graphic::ConstantBuffer* m_CBV;
	};


	class MixtureRenderPass : public RenderPass {

	public:
		MixtureRenderPass(UINT num_texture, const UINT width, const UINT height);

		inline UINT size() { return m_DescriptorTable->size(); }

		void Initialize() override;
		
		void PrepareData(Scene& scene) override { } 

		void Render(Graphic::CommandList& commandList, Scene& scene) override;

	private:
		CD3DX12_VIEWPORT m_Viewport;
		CD3DX12_RECT m_ScissorRect;
		// std::shared_ptr<SimpleMaterial> m_MixtureTextures;
	
		ptrMesh m_RenderScreen;
	};

	/*************************************************************************************
			             Render pass implementation End
	*************************************************************************************/
}