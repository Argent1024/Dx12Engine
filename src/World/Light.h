#pragma once

#include "Texture.h"
#include "Camera.h"

#define ptrLight std::shared_ptr<Game::Light>
	
namespace Game {
	using namespace Math;

	class Light {

	public:
		// Record Commands for creating shadow map
		virtual void RecordCommands() = 0;
		
		// Call commandlist->Draw() to create the shadow map
		virtual void DrawShadowMap() = 0;

		// Use light when rendering object
		// (Bind Shadow texture & Light Texture into root signature)
		virtual void UseLight() = 0;
	};


	class DirectionLight : public Light
	{
	public:
		DirectionLight(Vector3 dir, Vector3 radiance);
		

	private:
		OrthonormalCamera m_Camera;

		Vector3 m_dir;
		Vector3 m_Radiance;
	};

}