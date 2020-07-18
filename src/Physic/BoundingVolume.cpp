#include "BoundingVolume.h"
#include "MathCommon.h"

namespace Physic 
{
	using namespace Math;

	//****************** Sphere *********************//
	BoundingSphere::BoundingSphere(Vector3 c,Scalar r) :
		center(c), radius(r) { }
	
	void BoundingSphere::ApplyTransform(const Transform& T) {
		center = T.GetTranslation();
	}

	Intersection BoundingSphere::_CollideSphere(const BoundingSphere& sphere, bool noisect) const 
	{
		static const Math::Scalar tolerence(0.1f);
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
		assert(Length2(ans.p1 - ans.p2) < tolerence && "Two isect points too far away");
		ans.n1 = -d;
		ans.n2 = d;
		return ans;
	}

}