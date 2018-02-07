#pragma once
#include <Core\Component.h>
#include <glm\gtc\matrix_transform.hpp>

namespace snes
{
	class Camera : public Component
	{
	public:
		Camera(GameObject& gameObject) : Component(gameObject) {}
		~Camera() {};

		void MainLogic() override;

		/** Set whether this camera is being controlled by the keyboard/mouse */
		void SetCameraControl(bool on) { m_cameraControl = on; }

		/** @return the projection matrix for this camera */
		glm::mat4 GetProjMatrix();
		/** @return the view matrix for this camera */
		glm::mat4 GetViewMatrix();

	private:
		/** @return the direction the camera is facing in euler angles */
		glm::vec3 GetCameraDirection();

		bool m_cameraControl = false;

		float m_fieldOfView = 85.0f;
		float m_nearClipPlane = 0.01f;
		float m_farClipPlane = 1000.0f;

		float m_xSensitivity = 0.5f;
		float m_ySensitivity = 0.3f;
		float m_moveSpeed = 20.0f;
	};
}
