#pragma once

#include "Texture.h"
#include "DepthBuffer.h"
#include "Camera.h"

#define ptrLight std::shared_ptr<Game::Light>

namespace Game {

	enum LightType {
		DIRECTION,
		POINT
	};

	using namespace Math;

	class Light {

	public:
		// Use light when rendering object
		// (Bind Shadow texture & Light Texture into root signature)
		virtual void UseLight(Graphic::CommandList& commandList, UINT slot) = 0;

		// Shadow Pass
		virtual void UseDepth(Graphic::CommandList& commandList) = 0;
	};


	class DirectionLight : public Light
	{
	public:
		// TextureNum  2 + n, 2 : CBV for constant, SRV for depth. n : other texture needed 
		DirectionLight(Vector3 dir, Vector3 radiance, UINT shadowMapW=256, UINT shadowMapH=256, UINT textureNum=2);
		
		// Create Depth buffer, bind stuff to descriptor tbl
		void Initialize();

		void UseLight(Graphic::CommandList& commandList, UINT slot) override;

		void UseDepth(Graphic::CommandList& commandList) override;

	private:

		OrthonormalCamera m_Camera;
		Graphic::ConstantBuffer* m_CBV; // store constant
		Graphic::DepthBuffer m_DepthBuffer; // For Shadow Pass

		Graphic::DescriptorTable m_Table;

		Vector3 m_dir;
		Vector3 m_Radiance;
	};

}