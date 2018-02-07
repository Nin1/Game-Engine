#pragma once
#include <Core\Component.h>
#include "glm\glm.hpp"

namespace snes
{
	class PointLight : public Component
	{
	public:
		PointLight(GameObject& gameObject) : Component(gameObject) {};
		~PointLight() {};

		/** Set the colour of the pointlight */
		void SetColour(glm::vec3 colour) { m_colour = colour; }
		/** @return the colour of the pointlight */
		const glm::vec3& GetColour() const { return m_colour; }

		/** Set the linear attenuation of the pointlight */
		void SetLinearAttenuation(float attenuation) { m_linearAttenuation = attenuation; }
		/** @return the linear attenuation of the pointlight */
		float GetLinearAttenuation() { return m_linearAttenuation; }

		/** Set the quadratic attenuation of the pointlight */
		void SetQuadraticAttenuation(float attenuation) { m_quadraticAttenuation = attenuation; }
		/** @return the quadratic attenuation of the pointlight */
		float GetQuadraticAttenuation() { return m_quadraticAttenuation; }

	private:
		glm::vec3 m_colour;
		float m_linearAttenuation = 0.1f;
		float m_quadraticAttenuation = 0.2f;
	};
}
