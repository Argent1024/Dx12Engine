#pragma once

#include "Utility/input.h"
#include "Scene.h"


namespace Game {

	// How the Scene will behave under different input
	class GameControl {
	public:
		virtual void UpdateScene(Scene& scene) = 0;
	};

	
	class SimpleCameraControl : public GameControl {
	public:
		void UpdateScene(Scene& scene) override;

	private:
		const float m_MouseSpeed = 0.1f;

		const float m_RotationSpeed = 0.1f;

		const float m_MoveSpeed = 0.1f;

		const float m_ZoomSpeed = 0.001f;

	//	static const char m_RotationKey = 0xa4; //Left Alt
	};
}