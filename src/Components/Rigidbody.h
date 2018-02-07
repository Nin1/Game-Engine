#pragma once
#include <Core\Component.h>
#include <glm/glm.hpp>

namespace snes
{
	/** Rigidbody
	  * Handles physics and collision between objects */
	class Rigidbody : public Component
	{
	public:
		Rigidbody(GameObject& gameObject)
			: Component(gameObject)
			, m_velocity(0)
			, m_drag(0)
		{};
		~Rigidbody() {};

		void FixedLogic() override;
		void OnCollision(GameObject& other) override;

		/** Check if this object has intersected with the other object,
		  * and push them apart if the have, triggering OnCollision() for each object. */
		void HandleCollision(GameObject& other);
		/** Update the position of the GameObject's transform using the current velocity */
		void UpdatePosition();

		/** Lock the position of this GameObject */
		void LockPosition() { m_lockPosition = true; }
		/** Unlock the position of this GameObject */
		void UnlockPosition() { m_lockPosition = false; }

		/** Set the maximum speed of this object on the x/z axis */
		void SetMaxLateralSpeed(float speed) { m_maxLateralSpeed = speed; }
		/** Add the input velocity to the current velocity, clamped to the max speed */
		void AddVelocity(glm::vec3 vel);
		/** Set the drag amount to the current velocity every second */
		void SetDrag(glm::vec3 vel);

	private:
		void ApplyDrag();

		glm::vec3 m_velocity;
		glm::vec3 m_drag;
		float m_mass;
		float m_maxLateralSpeed = 1.0f;
		bool m_lockPosition = false;
	};
}
