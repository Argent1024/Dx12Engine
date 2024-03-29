#pragma once
#include "Graphic/PipelineState.h"
#include "Graphic/RootSignature.h"
#include "Graphic/CommandList.h"

#include "Scene.h"

namespace Game {

	// Implement one explict render pass 
	// e.x. default render pass, screen space method, transpancy ... 
	class RenderPass {
	public:
		virtual void Initialize(ptrGraphicsPSO pso) = 0;

		// Prepare data for rendering(like put in the camera's data), 
		// RenderEngine will call this function in main thread TODO ?
		virtual void PrepareData(const Scene& scene) = 0;

		// Record Commands for rendering
		// RenderEngine will call this function in worker thread
		virtual void Render(Graphic::CommandList& commandList, const Scene& scene) = 0;

		// TODO maybe more than one table
		Graphic::DescriptorTable* GetTable() { return m_DescriptorTable; }

		ptrRootSignature GetRootSignature() { return m_rootSignature; }

		RenderType m_ObjRenderType; // Record the render needed game objects' position in the scene class

	protected:
		ptrRootSignature m_rootSignature;
		ptrGraphicsPSO m_PSO;

		Graphic::DescriptorTable* m_DescriptorTable;

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
			BOOL debugnormal = FALSE;
			BOOL debugpos = FALSE;
			BOOL padding0[2]; // Align

		};

		void Initialize(ptrGraphicsPSO pso) override;

		void PrepareData(const Scene& scene) override;

		void Render(Graphic::CommandList& commandList,const Scene& scene) override;

		// Settings of the renderpass
		ConstBufferData& GetCBVData() { return m_ConstBuffer.GetData(); }

	private:
		Graphic::ConstantBuffer<ConstBufferData> m_ConstBuffer;
	};

	class MixtureRenderPass : public RenderPass {

	public:
		MixtureRenderPass(UINT num_texture, const UINT width, const UINT height);

		inline UINT size() { return m_DescriptorTable->size(); }

		void Initialize(ptrGraphicsPSO) override;
		
		void PrepareData(const Scene& scene) override { } 

		void Render(Graphic::CommandList& commandList, const Scene& scene) override;

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