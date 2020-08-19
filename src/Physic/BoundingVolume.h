#pragma once
#include "stdafx.h"
#include "MathLib.h"


namespace Physic
{	// Store the isect info for two object
	// ideally p1 should literarlly locates on obj 1 and p2 on obj2,
	// n1 n2 are normals on obj1 obj2
	struct Intersection {

		bool intersect = false;
		Math::Vector3 p1;
		Math::Vector3 p2;
		Math::Vector3 n1 = Math::Vector3(Math::kZero);
		Math::Vector3 n2 = Math::Vector3(Math::kZero);
		
		inline void SwapOrder() { std::swap(n1, n2); }
	};

	class BoundingSphere;
	class BoundingBox;
	class BoundingPlane;
	// class BoundingSpecial;	// For Special effect

	class BoundingVolumeTree;


	class BoundingVolume {
		friend BoundingSphere;
		friend BoundingBox;
		friend BoundingPlane;
		friend BoundingVolumeTree;
	public:
		virtual Intersection CollideWith(const BoundingVolume& bv, bool noisect=false) const = 0;
		
		virtual void SetTransform(const Math::Transform& T) = 0;

	protected:
		virtual Intersection _CollideSphere(const BoundingSphere& sphere, bool noisect) const = 0;
		virtual Intersection _CollideBox(const BoundingBox& box, bool noisect) const = 0;
		virtual Intersection _CollidePlane(const BoundingPlane& plane, bool noisect) const = 0;
		
		// BV tree should implement this, 
		// while other simple bv should call tree.CollideSelf(*this)
		// TODO maybe dynamic cast here 
		virtual Intersection _CollideTree(const BoundingVolumeTree& tree, bool noisect) const = 0;
	};


	class BoundingVolumeTree : public BoundingVolume {
	public:
		Intersection CollideWith(const BoundingVolume& bv, bool noisect=false) const override 
		{ return bv._CollideTree(*this, noisect); }
	};


	/*************************************************************************************************
	************************************* IMPLEMENTATION BELOW ***************************************
	**************************************************************************************************/

	class BoundingSphere : public BoundingVolume {
		
		friend BoundingBox;
		friend BoundingPlane;
		friend BoundingVolumeTree;

	public:
		BoundingSphere(Math::Vector3 c, Math::Scalar r); 

		Intersection CollideWith(const BoundingVolume& bv, bool noisect=false) const override 
		{ return bv._CollideSphere(*this, noisect); }

		void  SetTransform(const Math::Transform& T) override;

	protected:
		Intersection _CollideSphere(const BoundingSphere& sphere, bool noisect) const override;
		Intersection _CollideBox(const BoundingBox& box, bool noisect) const override  { return Intersection(); }
		Intersection _CollidePlane(const BoundingPlane& plane, bool noisect) const override;

		inline Intersection _CollideTree(const BoundingVolumeTree& tree, bool noisect) const override {
			return tree._CollideSphere(*this, noisect);
		}

		Math::Vector3 center = Math::Vector3(Math::kZero);
		Math::Scalar radius = Math::Scalar(1.0f);
	};


	class BoundingPlane : public BoundingVolume {

		friend BoundingSphere;
		friend BoundingBox;
		friend BoundingVolumeTree;

	public:
		static const Math::Scalar DefaultLength;

		BoundingPlane(Math::Vector3& c, Math::Vector3& x, Math::Vector3& y, Math::Scalar& lx, Math::Scalar& ly);

		Intersection CollideWith(const BoundingVolume& bv, bool noisect=false) const override 
		{ return bv._CollidePlane(*this, noisect); }

		void SetTransform(const Math::Transform& T) override;

	protected:
		Intersection _CollideSphere(const BoundingSphere& sphere, bool noisect) const override  
		{
			Intersection isect = sphere._CollidePlane(*this, noisect); 
			isect.SwapOrder();
			return isect;
		}

		Intersection _CollideBox(const BoundingBox& box, bool noisect) const override  { return Intersection(); }
		
		Intersection _CollidePlane(const BoundingPlane& plane, bool noisect) const override  { return Intersection(); }

		inline Intersection _CollideTree(const BoundingVolumeTree& tree, bool noisect) const override {
			return tree._CollidePlane(*this, noisect);
		}

		Math::Vector3 center = Math::Vector3(Math::kZero);
		Math::Vector3 dirX = Math::Vector3(Math::kXUnitVec); 
		Math::Vector3 dirY = Math::Vector3(Math::kYUnitVec);
		Math::Scalar lenX = 10;
		Math::Scalar lenY = 10;

	private:
		inline void AssertVerticle() { 
			assert( Math::Abs(Math::Dot(dirX, dirY)) < Math::TOLERENCE && "The X, Y directions are not verticle"); 
		}

	};
}

