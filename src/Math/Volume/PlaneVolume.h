#pragma once
#include "../Volume.h"

namespace Math {

	struct PlaneVolume : public Volume<3> {
	public:
		static constexpr FLOAT DefaultLength = 10.0f;
		using VectorD = Vector3;
		using Collision = typename CollisionIntersection<3>;

		PlaneVolume(VectorD& c, VectorD& x, VectorD& y, Scalar& lx, Scalar& ly) 
			: center(c), dirX(x), dirY(y), lenX(lx), lenY(ly)
		{ 
			AssertOrth();
		}

		Collision CollideWith(const Volume& bv, bool noisect = false) const override
		{
			return bv._CollidePlane(*this, noisect);
		}

		void SetTransform(const Math::Transform& T) override 
		{
			// TODO slow
			/*Vector3 x = T * Vector3(kXUnitVec);
			Vector3 y = T * Vector3(kYUnitVec);
		
			lenX = Length(x);
			lenY = Length(y);
			dirX = x / lenX;
			dirY = y / lenY;

			AssertOrth();*/
		}

		Collision _CollideSphere(const SphereVolume<3>& sphere, bool noisect) const override
		{
			Collision isect = sphere._CollidePlane(*this, noisect);
			isect.SwapOrder();
			return isect;
		}

		//Collision _CollideAABox(const AABoxVolume& box, bool noisect) const override { assert(FALSE && "not implemented"); return CollisionIntersection(); }
		//Collision _CollideBox(const BoxVolume& box, bool noisect) const override { assert(FALSE && "not implemented"); return CollisionIntersection(); }

		Collision _CollidePlane(const PlaneVolume& plane, bool noisect) const override { return Collision(); }

		VectorD center = VectorD(Math::kZero);
		VectorD dirX = VectorD(Math::kXUnitVec);
		VectorD dirY = VectorD(Math::kYUnitVec);
		Scalar lenX = DefaultLength;
		Scalar lenY = DefaultLength;

	private:
		void AssertOrth() { assert(Math::Abs(Math::Dot(dirX, dirY)) < Math::TOLERENCE && "The X, Y directions are not orthogonal"); }
	};
}