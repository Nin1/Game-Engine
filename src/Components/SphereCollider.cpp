#include "stdafx.h"
#include "SphereCollider.h"
#include "AABBCollider.h"
#include <glm/gtx/norm.hpp>

namespace snes
{
	void SphereCollider::SetBounds(glm::vec3 center, float radius)
	{
		m_center = center;
		m_radius = radius;
	}

	bool SphereCollider::Intersects(const AABBCollider& target) const
	{
		// Center of sphere relative to center of box
		glm::vec3 relativeSphereCenter = m_center - target.GetCenter();
		// Point on AABB closest to sphere
		glm::vec3 closestPoint;

		// Find x position of the closest point to the sphere on the bounding box
		if (relativeSphereCenter.x < -target.GetSize().x / 2.0f)
		{
			closestPoint.x = -target.GetSize().x / 2.0f;
		}
		else if (relativeSphereCenter.x > target.GetSize().x / 2.0f)
		{
			closestPoint.x = target.GetSize().x;
		}
		else
		{
			closestPoint.x = target.GetSize().x;
		}

		// Find y position of the closest point to the sphere on the bounding box
		if (relativeSphereCenter.y < -target.GetSize().y / 2.0f)
		{
			closestPoint.y = -target.GetSize().y / 2.0f;
		}
		else if (relativeSphereCenter.y > target.GetSize().y / 2.0f)
		{
			closestPoint.y = target.GetSize().y;
		}
		else
		{
			closestPoint.y = target.GetSize().y;
		}

		// Find z position of the closest point to the sphere on the bounding box
		if (relativeSphereCenter.z < -target.GetSize().z / 2.0f)
		{
			closestPoint.z = -target.GetSize().z / 2.0f;
		}
		else if (relativeSphereCenter.z > target.GetSize().z / 2.0f)
		{
			closestPoint.z = target.GetSize().z;
		}
		else
		{
			closestPoint.z = target.GetSize().z;
		}
		
		// Check if the distance between the sphere center and the point is less than the radius
		glm::vec3 distance = relativeSphereCenter - closestPoint;

		return glm::length2(distance) < m_radius*m_radius;
	}

	bool SphereCollider::Intersects(const SphereCollider& target) const
	{
		glm::vec3 distance = target.m_center - this->m_center;
		float radiusSum = target.m_radius + this->m_radius;

		return glm::length2(distance) < (radiusSum * radiusSum);
	}
}
