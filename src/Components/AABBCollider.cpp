#include "stdafx.h"
#include "AABBCollider.h"
#include "MeshRenderer.h"
#include "SphereCollider.h"
#include "LODModel.h"
#include "TessModel.h"
#include <Core\GameObject.h>
#include <glm/gtx/euler_angles.hpp>

namespace snes
{
	void AABBCollider::FixedLogic()
	{
		std::weak_ptr<Mesh> mesh;

		// Calculate collider bounds from mesh
		const auto mr = m_gameObject.GetComponent<MeshRenderer>();
		if (mr)
		{
			mesh = mr->GetMesh();
		}
		else
		{
			const auto lodMesh = m_gameObject.GetComponent<LODModel>();
			if (lodMesh)
			{
				mesh = lodMesh->GetMesh(0);
			}
			const auto tessMesh = m_gameObject.GetComponent<TessModel>();
			if (tessMesh)
			{
				mesh = tessMesh->GetMesh();
			}
		}

		if (mesh.expired())
		{
			std::cout << "Error: No mesh exists when creating bounding box." << std::endl;
			return;
		}

		glm::vec3 min = mesh.lock()->GetVertices()[0];
		glm::vec3 max = min;

		for (const auto& vertex : mesh.lock()->GetVertices())
		{
			if (vertex.x < min.x)
			{
				min.x = vertex.x;
			}
			if (vertex.y < min.y)
			{
				min.y = vertex.y;
			}
			if (vertex.z < min.z)
			{
				min.z = vertex.z;
			}

			if (vertex.x > max.x)
			{
				max.x = vertex.x;
			}
			if (vertex.y > max.y)
			{
				max.y = vertex.y;
			}
			if (vertex.z > max.z)
			{
				max.z = vertex.z;
			}
		}
		
		// Translate the min/max points into world space
		auto transform = m_gameObject.GetTransform();
		glm::mat4 scale = glm::scale(glm::mat4(1.0f), transform.GetWorldScale());
		glm::mat4 translate = glm::translate(glm::mat4(1.0f), transform.GetWorldPosition());
		glm::mat4 modelMat = translate * scale;

		min = modelMat * glm::vec4(min, 1);
		max = modelMat * glm::vec4(max, 1);

		// Create the bounding box
		SetMinMax(min, max);
		
	}

	void AABBCollider::SetBounds(glm::vec3 center, glm::vec3 size)
	{
		m_center = center;
		m_size = size;
	}

	void AABBCollider::SetMinMax(glm::vec3 min, glm::vec3 max)
	{
		m_size = max - min;
		m_center = min + (m_size / 2.0f);
	}

	bool AABBCollider::Intersects(const AABBCollider& target) const
	{
		return (abs(m_center.x - target.m_center.x) * 2 < m_size.x + target.m_size.x)
			&& (abs(m_center.y - target.m_center.y) * 2 < m_size.y + target.m_size.y)
			&& (abs(m_center.z - target.m_center.z) * 2 < m_size.z + target.m_size.z);
	}

	bool AABBCollider::Intersects(const SphereCollider& target) const
	{
		// SphereCollider implements sphere-AABB collision
		return target.Intersects(*this);
	}
}
