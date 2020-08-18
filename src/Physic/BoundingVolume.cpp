#include "BoundingVolume.h"
#include "MathCommon.h"

namespace Physic 
{
	using namespace Math;

	//****************** Sphere *********************//
	BoundingSphere::BoundingSphere(Vector3 c,Scalar r) :
		center(c), radius(r) { }
	
	void BoundingSphere::SetTransform(const Transform& T) {
		center = T.GetTranslation();
	}

	Intersection BoundingSphere::_CollideSphere(const BoundingSphere& sphere, bool noisect) const 
	{
		Intersection ans;

		const Vector3& a = center;
		const Vector3& b = sphere.center;

		const Scalar& r1 = radius;
		const Scalar& r2 = sphere.radius;

		Vector3 d = a - b;
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


	Intersection BoundingSphere::_CollidePlane(const BoundingPlane& plane, bool noisect) const
	{
		Intersection ans;

		const Vector3& x = plane.dirX;
		const Vector3& y = plane.dirY;
		
		Vector3 n = CrossProduct(x, y);
		Vector3 v = plane.center - center;

		Scalar L = Dot(n, v);
		// Sphere below the plane
		if (L < 0) { L = -L; n = -n; }

		if (L - radius > TOLERENCE) {
			return ans;
		}

		// p should be the interection point on plane
		Vector3 p = center - L * n;

		// check whether p locates on the plane
		Vector3 dir = p - plane.center;
		Scalar projX = Abs(Dot(dir, x));
		Scalar projY = Abs(Dot(dir, y));
		if (!projX < plane.lenX || !projY < plane.lenY) {
			return ans;
		}

		ans.intersect = TRUE;
		if (!noisect) {
			// TODO Will this be faster?
			ans.p1 = center - radius * n;
			ans.n1 = n;

			ans.p2 = p;
			ans.n2 = -n;
		}
		return ans;
	}



	//********************** Plane *********************//
	const Math::Scalar BoundingPlane::DefaultLength = Scalar(10);

	BoundingPlane::BoundingPlane(Vector3& c, Vector3& x, Vector3& y, Scalar& lx, Scalar& ly)
		: center(c), dirX(x), dirY(y), lenX(lx), lenY(ly)
	{ 
		AssertVerticle();
	}

	void BoundingPlane::SetTransform(const Transform& T) 
	{
		// TODO slow
		Vector3 x = T * Vector3(kXUnitVec);
		Vector3 y = T * Vector3(kYUnitVec);
		
		lenX = Length(x);
		lenY = Length(y);
		dirX = x / lenX;
		dirY = y / lenY;

		AssertVerticle();
	}

	
}
