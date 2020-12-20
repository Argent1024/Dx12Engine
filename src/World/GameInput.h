#pragma once

#include "Utility/input.h"
#include "Scene.h"

namespace Game {

	// How the Scene will behave under different input
	class GameControl {
	public:
		virtual void UpdateScene(Scene& scene) = 0;
	};

	class VoidGameControl : public GameControl {
	public:
		void UpdateScene(Scene& scene) { };

	};
}