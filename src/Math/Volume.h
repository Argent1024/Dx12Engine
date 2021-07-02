#pragma once
#include "stdafx.h"
#include "MathLib.h"
#include "Ray.h"

namespace Math
{	// Store the isect info for two object
	// ideally p1 should literarlly locates on obj 1 and p2 on obj2,
	// n1 n2 are normals on obj1 obj2
	struct CollisionIntersection {
		bool intersect = false;
		Vector3 p1;
		Vector3 p2;
		Vector3 n1 = Vector3(Math::kZero);
		Vector3 n2 = Vector3(Math::kZero);
		
		inline void SwapOrder() { std::swap(n1, n2); }
	};

	class SphereVolume;
	class AABoxVolume;
	class BoxVolume;
	class PlaneVolume;
	// class Special;	// For Special effect

	// class VolumeTree;


	class Volume {
	public:
		// Calling noisect if we only want to know if volume touch each other or not and exact intersection is not needed
		virtual CollisionIntersection CollideWith(const Volume& bv, bool noisect=false) const = 0;
		
		virtual void SetTransform(const Math::Transform& T) = 0;

		virtual bool IntersectRay(Ray& ray) = 0;

	
		virtual CollisionIntersection _CollideSphere(const SphereVolume& sphere, bool noisect) const = 0;
		virtual CollisionIntersection _CollideAABox(const AABoxVolume& box, bool noisect) const = 0;
		virtual CollisionIntersection _CollideBox(const BoxVolume& box, bool noisect) const = 0;
		virtual CollisionIntersection _CollidePlane(const PlaneVolume& plane, bool noisect) const = 0;
		
		// virtual CollisionIntersection _CollideTree(const VolumeTree& tree, bool noisect) const = 0;
	};

	/*class VolumeTree : protected Volume {
	public:
		CollisionIntersection CollideWith(const Volume& bv, bool noisect=false) const override 
		{ return bv._CollideTree(*this, noisect); }
	};*/


	/*************************************************************************************************
	************************************* IMPLEMENTATION BELOW ***************************************
	**************************************************************************************************/

	class SphereVolume : public Volume {
	public:
		SphereVolume(Math::Vector3 c, Math::Scalar r);

		CollisionIntersection CollideWith(const Volume& bv, bool noisect = false) const override
		{
			return bv._CollideSphere(*this, noisect);
		}

		// The transform should have constant scale
		void SetTransform(const Math::Transform& T) override;


		CollisionIntersection _CollideSphere(const SphereVolume& sphere, bool noisect) const override;
		CollisionIntersection _CollideAABox(const AABoxVolume& box, bool noisect) const override { return box._CollideSphere(*this, noisect); }
		CollisionIntersection _CollideBox(const BoxVolume& box, bool noisect) const override  { assert(FALSE && "not implemented"); return CollisionIntersection(); }
		CollisionIntersection _CollidePlane(const PlaneVolume& plane, bool noisect) const override;

		Math::Vector3 center = Math::Vector3(Math::kZero);
		Math::Scalar radius = Math::Scalar(1.0f);
	};

	class AABoxVolume : public Volume {
	public:
		AABoxVolume(Vector3 c, Vector3 r);

		CollisionIntersection CollideWith(const Volume& bv, bool noisect = false) const override
		{
			return bv._CollideAABox(*this, noisect);
		}

		void SetTransform(const Math::Transform& T) override;

		CollisionIntersection _CollideSphere(const SphereVolume& sphere, bool noisect) const override;
		CollisionIntersection _CollideAABox(const AABoxVolume& box, bool noisect) const override;
		CollisionIntersection _CollideBox(const BoxVolume& box, bool noisect) const override  { assert(FALSE && "not implemented"); return CollisionIntersection(); }
		CollisionIntersection _CollidePlane(const PlaneVolume& plane, bool noisect) const override { assert(FALSE && "not implemented"); return CollisionIntersection(); }

		Vector3 center;
		Vector3 radius;
	};

	class PlaneVolume : public Volume {
	public:
		static const Math::Scalar DefaultLength;

		PlaneVolume(Math::Vector3& c, Math::Vector3& x, Math::Vector3& y, Math::Scalar& lx, Math::Scalar& ly);

		CollisionIntersection CollideWith(const Volume& bv, bool noisect=false) const override 
		{ return bv._CollidePlane(*this, noisect); }

		void SetTransform(const Math::Transform& T) override;

		CollisionIntersection _CollideSphere(const SphereVolume& sphere, bool noisect) const override  
		{
			CollisionIntersection isect = sphere._CollidePlane(*this, noisect); 
			isect.SwapOrder();
			return isect;
		}

		CollisionIntersection _CollideAABox(const AABoxVolume& box, bool noisect) const override { assert(FALSE && "not implemented"); return CollisionIntersection(); }
		CollisionIntersection _CollideBox(const BoxVolume& box, bool noisect) const override  { assert(FALSE && "not implemented"); return CollisionIntersection(); }
		
		CollisionIntersection _CollidePlane(const PlaneVolume& plane, bool noisect) const override  { return CollisionIntersection(); }

		Math::Vector3 center = Math::Vector3(Math::kZero);
		Math::Vector3 dirX = Math::Vector3(Math::kXUnitVec); 
		Math::Vector3 dirY = Math::Vector3(Math::kYUnitVec);
		Math::Scalar lenX = DefaultLength;
		Math::Scalar lenY = DefaultLength;

	private:
		void AssertOrth() { assert( Math::Abs(Math::Dot(dirX, dirY)) < Math::TOLERENCE && "The X, Y directions are not orthogonal"); }
	};
}

using ptrVolume = std::shared_ptr<Math::Volume>;