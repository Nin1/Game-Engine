#include "stdafx.h"
#include "Rigidbody.h"
#include "AABBCollider.h"
#include <Core\GameObject.h>
#include <Core\FrameTime.h>
#include <algorithm>

namespace snes
{
	void Rigidbody::FixedLogic()
	{
		//m_velocity.y -= 9.81f * FrameTime::SECONDS_PER_FIXED_LOOP;
		ApplyDrag();
		UpdatePosition();
	}

	void Rigidbody::ApplyDrag()
	{
		if (m_velocity.x > 0)
		{
			m_velocity.x = std::max(m_velocity.x - m_drag.x * FrameTime::SECONDS_PER_FIXED_LOOP, 0.0f);
		}
		else if (m_velocity.x < 0)
		{
			m_velocity.x = std::min(m_velocity.x + m_drag.x * FrameTime::SECONDS_PER_FIXED_LOOP, 0.0f);
		}
		if (m_velocity.y > 0)
		{
			m_velocity.y = std::max(m_velocity.y - m_drag.y * FrameTime::SECONDS_PER_FIXED_LOOP, 0.0f);
		}
		else if (m_velocity.y < 0)
		{
			m_velocity.y = std::min(m_velocity.y + m_drag.y * FrameTime::SECONDS_PER_FIXED_LOOP, 0.0f);
		}
		if (m_velocity.z > 0)
		{
			m_velocity.z = std::max(m_velocity.z - m_drag.z * FrameTime::SECONDS_PER_FIXED_LOOP, 0.0f);
		}
		else if (m_velocity.z < 0)
		{
			m_velocity.z = std::min(m_velocity.z + m_drag.z * FrameTime::SECONDS_PER_FIXED_LOOP, 0.0f);
		}
	}

	void Rigidbody::OnCollision(GameObject& other)
	{
		//m_velocity.y = 100.0f; // Make the object bounce for testing
	}

	/** Swept AABB Collision using Minkowski's difference */
	void Rigidbody::HandleCollision(GameObject& other)
	{
		std::shared_ptr<Rigidbody> otherRB = other.GetComponent<Rigidbody>();
		if (!otherRB)
		{
			return;
		}
		std::shared_ptr<AABBCollider> thisCollider = m_gameObject.GetComponent<AABBCollider>();
		std::shared_ptr<AABBCollider> otherCollider = other.GetComponent<AABBCollider>();

		if (!thisCollider || !otherCollider)
		{
			return;
		}

		// Find the Minkowski sum of the two AABB colliders
		glm::vec3 minkowskiMin = thisCollider->GetMin() - otherCollider->GetMax();
		glm::vec3 minkowskiSize = thisCollider->GetSize() + otherCollider->GetSize();
		glm::vec3 minkowskiMax = minkowskiMin + minkowskiSize;

		if (minkowskiMin.x <= 0 &&
			minkowskiMin.y <= 0 &&
			minkowskiMin.z <= 0 &&
			minkowskiMax.x >= 0 &&
			minkowskiMax.y >= 0 &&
			minkowskiMax.z >= 0)
		{
			// If the Minkowski box encompases the origin, these two objects are currently colliding
			// Find penetration vector
			float minDist = abs(minkowskiMin.x);
			glm::vec3 boundsPoint(-minkowskiMin.x, 0, 0);
			if (abs(minkowskiMax.x) < minDist)
			{
				minDist = abs(minkowskiMax.x);
				boundsPoint = glm::vec3(-minkowskiMax.x, 0, 0);
			}
			if (abs(minkowskiMin.y) < minDist)
			{
				minDist = abs(minkowskiMin.y);
				boundsPoint = glm::vec3(0, -minkowskiMin.y, 0);
			}
			if (abs(minkowskiMax.y) < minDist)
			{
				minDist = abs(minkowskiMax.y);
				boundsPoint = glm::vec3(0, -minkowskiMax.y, 0);
			}
			if (abs(minkowskiMin.z) < minDist)
			{
				minDist = abs(minkowskiMin.z);
				boundsPoint = glm::vec3(0, 0, -minkowskiMin.z);
			}
			if (abs(minkowskiMax.z) < minDist)
			{
				minDist = abs(minkowskiMax.z);
				boundsPoint = glm::vec3(0, 0, -minkowskiMax.z);
			}


			if (!m_lockPosition)
			{
				m_transform.Translate(boundsPoint);
			}

			if (boundsPoint.x != 0.0f)
			{
				m_velocity.x = 0.0f;
				otherRB->m_velocity.x = 0.0f;
			}
			if (boundsPoint.y != 0.0f)
			{
				m_velocity.y = 0.0f;
				otherRB->m_velocity.y = 0.0f;
			}
			if (boundsPoint.z != 0.0f)
			{
				m_velocity.z = 0.0f;
				otherRB->m_velocity.z = 0.0f;
			}
		}
	}

	void Rigidbody::UpdatePosition()
	{
		if (!m_lockPosition)
		{
			m_transform.Translate(m_velocity * FrameTime::SECONDS_PER_FIXED_LOOP);
		}
		else
		{
			m_velocity = glm::vec3(0);
		}
	}

	void Rigidbody::AddVelocity(glm::vec3 vel)
	{
		m_velocity += vel;

		glm::vec2 lateralVelocity(m_velocity.x, m_velocity.z);

		if (m_maxLateralSpeed > 0.0f && lateralVelocity.length() > m_maxLateralSpeed)
		{
			lateralVelocity = glm::normalize(lateralVelocity) * m_maxLateralSpeed;
			m_velocity.x = lateralVelocity.x;
			m_velocity.z = lateralVelocity.y;
		}
	}

	void Rigidbody::SetDrag(glm::vec3 vel)
	{
		m_drag = vel;
	}
}
