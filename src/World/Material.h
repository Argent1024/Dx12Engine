#pragma once
#include "DXHelper.h"
#include "GPUHeap.h"
#include "CommandList.h"
#include "PipelineState.h"
#include "RootSignature.h"

#define ptrPSO std::shared_ptr<Graphic::GraphicsPSO>
#define ptrRootSigature std::shared_ptr<Graphic::RootSignature>

namespace Game {

	class Material {
	public:
		Material(ptrPSO pso, ptrRootSigature rootSignature)
			: m_pso(pso), m_RootSignature(rootSignature) {}

		void Use(Graphic::CommandList& commandList) 
		{
			commandList.SetPipelineState(*m_pso);
			commandList.SetGraphicsRootSignature(*m_RootSignature);
			_Use();
		}

	protected:
		// Do some copy maybe and set descriptor table here
		virtual void _Use(Graphic::CommandList& commandList) = 0;

		ptrPSO m_pso;
		ptrRootSigature m_RootSignature;
	};


}