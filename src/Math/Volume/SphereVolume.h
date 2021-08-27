#pragma once
#include "../Volume.h"

namespace Math {

	template <int Dim>
	struct SphereVolume : Volume<Dim> {
		using VectorD = typename Vector<Dim>::Type;
		using Collision = CollisionIntersection<Dim>;

		VectorD center = VectorD(Math::kZero);
		Scalar radius = Scalar(1.0f);
		
		SphereVolume(VectorD c, Math::Scalar r) : center(c), radius(r) { }

		Collision CollideWith(const Volume<Dim>& bv, bool noisect = false) const override
		{
			return bv._CollideSphere(*this, noisect);
		}

		// The transform should have constant scale
		void SetTransform(const Math::Transform& T) override 
		{
			center = T.GetTranslation();
		}


		Collision _CollideSphere(const SphereVolume<Dim>& sphere, bool noisect) const override 
		{
			Collision ans;
			const VectorD& a = center;
			const VectorD& b = sphere.center;

			const Scalar& r1 = radius;
			const Scalar& r2 = sphere.radius;
			VectorD d = a - b;
			Scalar dis2 = Dot(d, d);
			Scalar sumRadius = r1 + r2;

			ans.intersect = dis2 <= sumRadius * sumRadius;
			if (!ans.intersect || noisect) {
				return ans;
			}
		
			// Have an intersection here, calculate normal and position
			d = Normalize(d);
			ans.p1 = a - r1 * d; 
			ans.p2 = b + r2 * d;
			assert(Length2(ans.p1 - ans.p2) < TOLERENCE && "Two isect points too far away");
			ans.n1 = -d;
			ans.n2 = d;
			return ans;
		}

		//Collision _CollideAABox(const AABoxVolume<Dim>& box, bool noisect) const override { return box._CollideSphere(*this, noisect); }
		//Collision _CollideBox(const BoxVolume<Dim>& box, bool noisect) const override { assert(FALSE && "not implemented"); return CollisionIntersection(); }
		/*Collision _CollidePlane(const PlaneVolume& plane, bool noisect) const override {
			
		}*/
	};
}