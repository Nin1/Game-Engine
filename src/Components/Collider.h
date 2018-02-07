#pragma once
#include <Core\Component.h>

namespace snes
{
	class AABBCollider;
	class SphereCollider;

	class Collider : public Component
	{
	public:
		Collider(GameObject& gameObject) : Component(gameObject) {};
		~Collider() {};

		virtual void Awake() {};
		virtual void Start() {};
		virtual void FixedLogic() {};
		virtual void MainLogic() {};
		virtual void MainDraw() {};
		virtual void OnDestroy() {};
		virtual void OnCollision(GameObject& other) {};

		/** @return whether the target collider is intersecting with this collider */
		bool CheckIntersection(Collider& target);
		virtual bool Intersects(const AABBCollider& target) const = 0;
		virtual bool Intersects(const SphereCollider& target) const = 0;
	};
}
