#pragma once

#include "stdafx.h"
#include "Transform.h"


namespace Physic {

	using namespace Math;

	// Store the transform (object -> world) of an object, one game object must contain one and only one physic state 
	class State {
	public:
		// Default transform T = identity
		State() : T() {}

		State(Transform trans) : T(trans) {}

	private:
		Transform T;
	};
	
	

}
