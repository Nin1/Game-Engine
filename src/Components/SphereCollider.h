#pragma once
#include "Collider.h"
#include <glm\glm.hpp>

namespace snes
{
	class SphereCollider : public Collider
	{
	public:
		SphereCollider(GameObject& gameObject) : Collider(gameObject) {};
		~SphereCollider() {};

		/** Set the center and radius of this bounding sphere */
		void SetBounds(glm::vec3 center, float radius);

		/** Return whether this bounding box intersects with target collider */
		bool Intersects(const AABBCollider& target) const override;
		bool Intersects(const SphereCollider& target) const override;

	private:
		glm::vec3 m_center;
		float m_radius;
	};
}
