#pragma once
#include "../Volume.h"

template<int Dim>
class AABoxVolume : public Volume<Dim> {
public:
	using VectorD = typename Vector<dim>::Type;

	AABoxVolume<Dim>(VectorD c, VectorD r) : center(c), radius(r) { }

	CollisionIntersection<Dim> CollideWith(const Volume& bv, bool noisect = false) const override
	{
		return bv._CollideAABox(*this, noisect);
	}

	// TODO
	void SetTransform(const Math::Transform& T) override { 
		center = T.GetTranslation();
	}

	CollisionIntersection<Dim> _CollideSphere(const SphereVolume& sphere, bool noisect) const override;
	CollisionIntersection<Dim> _CollideAABox(const AABoxVolume& box, bool noisect) const override;
	CollisionIntersection<Dim> _CollideBox(const BoxVolume& box, bool noisect) const override  { assert(FALSE && "not implemented"); return CollisionIntersection(); }
	CollisionIntersection<Dim> _CollidePlane(const PlaneVolume& plane, bool noisect) const override { assert(FALSE && "not implemented"); return CollisionIntersection(); }

	VectorD center;
	VectorD radius;
};