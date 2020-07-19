#pragma once

#include "stdafx.h"
#include "Transform.h"


namespace Physic {


	// Store the transform (object -> world) of an object, one game object must contain one and only one physic state 
	class State {
	public:
		// Default transform T = identity
		State() : T() {}

		State(Math::Transform trans) : T(trans) {}

		Math::Transform GetTransform() const { return  T; }

	private:
		Math::Transform T;
	};
	
	

}
