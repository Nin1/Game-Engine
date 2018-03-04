#pragma once
#include "Camera.h"
#include "glm\glm.hpp"

namespace snes
{
	class DirectionalLight : public Component
	{
	public:
		DirectionalLight(GameObject& gameObject) : Component(gameObject) {};
		~DirectionalLight() {};

		/** Set the colour of the pointlight */
		void SetColour(glm::vec3 colour) { m_colour = colour; }
		/** @return the colour of the pointlight */
		const glm::vec3& GetColour() const { return m_colour; }

		void SetCamera(std::shared_ptr<Camera> camera) { m_camera = camera; }

		glm::mat4 GetViewProjectionMatrix() { return m_camera->GetProjMatrix() * m_camera->GetViewMatrix(); }

		void MainLogic() override;

		//@TODO: Add a "follow" object, so the shadow camera follows the main camera

	private:
		glm::vec3 m_colour;
		std::shared_ptr<Camera> m_camera;
	};
}
