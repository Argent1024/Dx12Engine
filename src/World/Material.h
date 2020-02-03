#pragma once
#include "DXHelper.h"
#include "GPUHeap.h"
#include "CommandList.h"
#include "PipelineState.h"
#include "RootSignature.h"

#define ptrPSO std::shared_ptr<Graphic::GraphicsPSO>
#define ptrRootSigature std::shared_ptr<Graphic::RootSignature>

namespace Game {
	// Class store rendering data
	// Store pipeline state obj &
	//       root signature &
	//       texture (maybe)&
	//		 other View or parameter that are needed when rendering
	class Material {
	public:
		Material() {}

		Material(ptrPSO pso, ptrRootSigature rootSignature)
			: m_pso(pso), m_RootSignature(rootSignature) {}

		inline void SetGPUStuff(ptrPSO pso, ptrRootSigature rootSignature) 
		{
			m_pso = pso;
			m_RootSignature = rootSignature;
		}

		void UseMaterial(Graphic::CommandList& commandList) const
		{
			commandList.SetPipelineState(*m_pso);
			commandList.SetGraphicsRootSignature(*m_RootSignature);
			_UseMaterial(commandList);
		}

	protected:
		// Do some copy maybe and set descriptor table here
		virtual void _UseMaterial(Graphic::CommandList& commandList) const = 0;

		ptrPSO m_pso;
		ptrRootSigature m_RootSignature;
	};


	class NoMaterial : public Material 
	{	
	public:
		NoMaterial(ptrPSO pso, ptrRootSigature rootSignature)
			: Material(pso, rootSignature) {}

		void _UseMaterial(Graphic::CommandList& commandList) const override {}
	};

}