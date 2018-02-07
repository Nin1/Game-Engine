#pragma once
#include <Core\Component.h>
#include <glm\vec3.hpp>
#include <glm\mat4x4.hpp>

namespace snes
{
	class GameObject;

	class Transform
	{
	public:
		Transform(GameObject& gameObject);
		~Transform();

		/** Set the element of this transform in local space */
		void SetLocalPosition(glm::vec3 position) { m_localPosition = position; }
		void SetLocalRotation(glm::vec3 rotation) { m_localRotation = rotation; }
		void SetLocalScale(glm::vec3 scale) { m_localScale = scale; }

		/** @return the element of this transform in local space */
		const glm::vec3& GetLocalPosition() const { return m_localPosition; }
		const glm::vec3& GetLocalRotation() const { return m_localRotation; }
		const glm::vec3& GetLocalScale() const { return m_localScale; }

		/** @return the element of this transform in world space */
		glm::vec3 GetWorldPosition() const;
		glm::vec3 GetWorldRotation() const;
		glm::vec3 GetWorldRotationRadians() const;
		glm::vec3 GetWorldScale() const;

		/** Apply a transformation to this transform in local space */
		void Translate(glm::vec3 translation) { m_localPosition += translation; }
		void Rotate(glm::vec3 rotation);
		void Scale(glm::vec3 scale) { m_localScale *= scale; }

		/** @return the transform-rotate-scale matrix of the transform */
		glm::mat4 GetTRS() const;

	private:
		glm::vec3 m_localPosition;
		glm::vec3 m_localRotation;
		glm::vec3 m_localScale;
		GameObject& m_gameObject;
	};
}
