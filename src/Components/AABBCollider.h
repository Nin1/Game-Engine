#pragma once
#include "Collider.h"
#include <glm\glm.hpp>

namespace snes
{
	/** Axis-Aligned Bounding Box */
	class AABBCollider : public Collider
	{
	public:
		AABBCollider(GameObject& gameObject) : Collider(gameObject) {};
		~AABBCollider() {};

		void FixedLogic() override;

		/** Set the position and size of this bounding box */
		void SetBounds(glm::vec3 center, glm::vec3 size);
		/** Set the min/max points of the bounding box */
		void SetMinMax(glm::vec3 min, glm::vec3 max);

		/** Return the size of the bounding box */
		const glm::vec3& GetSize() const { return m_size; }

		/** Return the center of the bounding box */
		const glm::vec3& GetCenter() const { return m_center; }

		/** Return the minimum corner of the bounding box */
		glm::vec3 GetMin() const { return m_center - (m_size / 2.0f); }

		/** Return the maximum corner of the bounding box */
		glm::vec3 GetMax() const { return m_center + (m_size / 2.0f); }

		/** Return whether this bounding box intersects with target collider */
		bool Intersects(const AABBCollider& target) const override;
		bool Intersects(const SphereCollider& target) const override;

	private:
		glm::vec3 m_center;
		glm::vec3 m_size;
	};
}
