#pragma once
#include "PipelineState.h"
#include "RootSignature.h"
#include "CommandList.h"
#include "Camera.h"
#include "GObject.h"

namespace Game {
	
	class RenderPass {
	public:
		virtual void Initialize() = 0;

		// Record Commands
		// Who implement this class need to implement at least one of this
		virtual void Render(Graphic::CommandList& commandList) {
			throw std::runtime_error("Using wrong render function");
		}

		virtual void Render(Graphic::CommandList& commandList, GObject* obj) {
			throw std::runtime_error("Using wrong render function");
		}

		virtual void Render(Graphic::CommandList& commandList, std::vector<GObject*>& objList) {
			throw std::runtime_error("Using wrong render function");
		}
		
	protected:
		ptrPSO m_PSO;
		ptrRootSignature m_rootSignature;
	};
	
	class DefaultRenderPass : public RenderPass {
	public:
		void Initialize() override;

		void Render(Graphic::CommandList& commandList, std::vector<GObject*>& objList) override;
		
		inline void SetCamera(Camera* camera) { m_Camera = camera; }

	private:
		Camera* m_Camera;

	};

	class MixtureRenderPass : public RenderPass {

	public:
		MixtureRenderPass(UINT num_texture);

		inline UINT size() { return m_MixtureTextures->size(); }

		inline Graphic::DescriptorTable* GetDescriptorTable() 
		{ return m_MixtureTextures->GetDescriptorTable(); }

		void Initialize() override;
	
		void Render(Graphic::CommandList& commandList) override;

	private:
		std::shared_ptr<SimpleMaterial> m_MixtureTextures;
		GObject* m_RenderScreen;
	};

}