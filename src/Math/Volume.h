#pragma once
#include "stdafx.h"
#include "MathLib.h"
#include "Ray.h"

namespace Math
{	// Store the isect info for two object
	// ideally p1 should literarlly locates on obj 1 and p2 on obj2,
	// n1 n2 are normals on obj1 obj2
	template<int dim>
	struct CollisionIntersection {
		using VectorD = typename Vector<dim>::Type;

		bool intersect = false;
		VectorD p1 = VectorD(Math::kZero);
		VectorD p2 = VectorD(Math::kZero);
		VectorD n1 = VectorD(Math::kYUnitVec);
		VectorD n2 = VectorD(Math::kYUnitVec);
		
		inline void SwapOrder() { std::swap(n1, n2); }
	};

	template<int dim>
	struct  RayIntersection
	{
		using VectorD = typename Vector<dim>::Type;

		bool intersect = false;
		Scalar t = Scalar(Math::kZero);
		VectorD p = VectorD(Math::kZero);
	};

	template <int Dim> struct SphereVolume;
	template <int Dim> struct AABoxVolume;
	template <int Dim> struct BoxVolume;
	struct PlaneVolume;

	// class Special;	// For Special effect

	// class VolumeTree;

	template <int Dim>
	struct Volume {
		using Collision = typename CollisionIntersection<Dim>;

		// Calling noisect if we only want to know if volume touch each other or not and exact intersection is not needed
		virtual Collision CollideWith(const Volume<Dim>& bv, bool noisect=false) const = 0;
		
		virtual void SetTransform(const Math::Transform& T) = 0;

		virtual RayIntersection<Dim> IntersectRay(Ray& ray) const { return RayIntersection<Dim>(); }

	
		virtual Collision _CollideSphere(const SphereVolume<Dim>& sphere, bool noisect) const { return Collision(); }
		virtual Collision _CollideAABox(const AABoxVolume<Dim>& box, bool noisect) const { return Collision(); }
		virtual Collision _CollideBox(const BoxVolume<Dim>& box, bool noisect) const  { return Collision(); }
		virtual Collision _CollidePlane(const PlaneVolume& plane, bool noisect) const  { return Collision(); }
		
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
	//template<int Dim>
	//class AABoxVolume : public Volume<Dim> {
	//public:
	//	AABoxVolume<Dim>(Vector3 c, Vector3 r);

	//	CollisionIntersection<Dim> CollideWith(const Volume& bv, bool noisect = false) const override
	//	{
	//		return bv._CollideAABox(*this, noisect);
	//	}

	//	void SetTransform(const Math::Transform& T) override;

	//	CollisionIntersection<Dim> _CollideSphere(const SphereVolume& sphere, bool noisect) const override;
	//	CollisionIntersection<Dim> _CollideAABox(const AABoxVolume& box, bool noisect) const override;
	//	CollisionIntersection<Dim> _CollideBox(const BoxVolume& box, bool noisect) const override  { assert(FALSE && "not implemented"); return CollisionIntersection(); }
	//	CollisionIntersection<Dim> _CollidePlane(const PlaneVolume& plane, bool noisect) const override { assert(FALSE && "not implemented"); return CollisionIntersection(); }

	//	Vector3 center;
	//	Vector3 radius;
	//};


}

// using ptrVolume = std::shared_ptr<Math::Volume>;