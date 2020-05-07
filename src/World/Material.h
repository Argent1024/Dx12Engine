#pragma once
#include "DXHelper.h"
#include "GPUHeap.h"
#include "CommandList.h"
#include "PipelineState.h"
#include "RootSignature.h"
#include "Texture.h"

namespace Game {
	
	class Material {
	public:
		
		// Do nothing
		virtual void UseMaterial(Graphic::CommandList& commandList) { }

	};


	class SimpleMaterial : public Material {
	public:
		SimpleMaterial(UINT numTexture) : m_table(numTexture, Engine::GetInitHeap()) {}

		inline Graphic::DescriptorTable* GetDescriptorTable() { return &m_table; }

		inline UINT size() { return m_table.size(); }

		void UseMaterial(Graphic::CommandList& commandList) override;

	private:
		Graphic::DescriptorTable m_table;
	};

}