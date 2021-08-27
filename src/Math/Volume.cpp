
/*
namespace Math 
{


	CollisionIntersection<Dim> SphereVolume::_CollidePlane(const PlaneVolume& plane, bool noisect) const
	{
		CollisionIntersection ans;

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


	
	AABoxVolume::AABoxVolume(Vector3 c, Vector3 r) : center(c), radius(r) 
	{ }

	void AABoxVolume::SetTransform(const Math::Transform& T) 
	{
		
	}

	CollisionIntersection<Dim> AABoxVolume::_CollideSphere(const SphereVolume& sphere, bool noisect) const 
	{
		return CollisionIntersection<Dim>();
	}

	CollisionIntersection<Dim> AABoxVolume::_CollideAABox(const AABoxVolume& box, bool noisect) const {
		CollisionIntersection<Dim> ans;
		const Vector3& a = center;
		const Vector3& b = box.center;

		const Vector3& r1 = radius;
		const Vector3& r2 = box.radius;
		
		Vector3 ab = a - b;
		Vector3 r = r1 + r2;
		Vector3 T = ab > r;
		if (T.GetX() || T.GetY() || T.GetZ()) { ans.intersect = TRUE; }
		
		if (ans.intersect && !noisect) {
			// Calculate the intersection of line segment ab and two boxes
			// TODO just simply assign the mid point now
			ans.p1 = (a + b) / 2;
			ans.p2 = ans.p1;

			ab = Normalize(ab);
			ans.n1 = -ab;
			ans.n2 = ab;
		}

		return ans;
	}

	
	const Math::Scalar PlaneVolume::DefaultLength = Scalar(10);

	PlaneVolume::PlaneVolume(Vector3& c, Vector3& x, Vector3& y, Scalar& lx, Scalar& ly)
		: center(c), dirX(x), dirY(y), lenX(lx), lenY(ly)
	{ 
		AssertOrth();
	}

	void PlaneVolume::SetTransform(const Transform& T) 
	{
		// TODO slow
		Vector3 x = T * Vector3(kXUnitVec);
		Vector3 y = T * Vector3(kYUnitVec);
		
		lenX = Length(x);
		lenY = Length(y);
		dirX = x / lenX;
		dirY = y / lenY;

		AssertOrth();
	}

	
}
*/