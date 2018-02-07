#include "stdafx.h"
#include "Collider.h"
#include "AABBCollider.h"
#include "SphereCollider.h"

namespace snes
{
	bool Collider::CheckIntersection(Collider& target)
	{
		// Cast Collider to find type
		AABBCollider* aabb = dynamic_cast<AABBCollider*>(&target);

		if (aabb)
		{
			return Intersects(*aabb);
		}

		SphereCollider* sphere = dynamic_cast<SphereCollider*>(&target);
		if (sphere)
		{
			return Intersects(*sphere);
		}

		return false;
	}
}
