#pragma once

#include "stdafx.h"
#include "Math/Transform.h"
#include "Math/Volume.h"

namespace Physics {


	// Store the transform (object -> world) of an object, one game object must contain one and only one physic state 
	class PhysicsState {
	public:
		// Default transform T = identity
		PhysicsState() : T() {}

		PhysicsState(Math::Transform trans) : T(trans) {}

		inline const Math::Transform& GetTransform() const { return T; }

		virtual void SetTransform(const Math::Transform& newT) { T = newT; }
		virtual void Update() { }
	protected:
		Math::Transform T;
	};

	//class CollidiablePhysicsState : public PhysicsState {
	//public:
	//	CollidiablePhysicsState(ptrVolume vol) : m_CollisionVolume(vol) {}

	//	void SetTransform(const Math::Transform& newT) override { T = newT; m_CollisionVolume->SetTransform(newT); }

	//	ptrVolume CollisionVolume() { return m_CollisionVolume; }
	//	const ptrVolume CollisionVolume() const { return m_CollisionVolume; }
	//
	//	// This is called from the collision detection code, store some info for calling the update later
	//	virtual void CollisionCallBack(const CollidiablePhysicsState&, const Math::CollisionIntersection&) { return; };

	//protected:
	//	ptrVolume m_CollisionVolume;
	//};

	//class RigidBodyPhysicState : public CollidiablePhysicsState {
	//public:
	//	RigidBodyPhysicState(ptrVolume vol) : CollidiablePhysicsState(vol) { }


	//	void CollisionCallBack(const CollidiablePhysicsState&, const Math::CollisionIntersection&) override;
	//	void CollisionCallBack(const RigidBodyPhysicState&, const Math::CollisionIntersection&);

	//	void Update() override;
	//};
}

//using ptrPhysicState = std::shared_ptr<Physics::PhysicsState>;
//using ptrCpstate = std::shared_ptr<Physics::CollidiablePhysicsState>;