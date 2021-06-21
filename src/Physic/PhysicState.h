#pragma once

#include "stdafx.h"
#include "Math/Transform.h"


namespace Physic {


	// Store the transform (object -> world) of an object, one game object must contain one and only one physic state 
	class PState {
	public:
		// Default transform T = identity
		PState() : T() {}

		PState(Math::Transform trans) : T(trans) {}

		inline const Math::Transform& GetTransform() const { return T; }

		inline void SetTransform(const Math::Transform& newT) { T = newT; }

	private:
		Math::Transform T;
	};
	
	

}
