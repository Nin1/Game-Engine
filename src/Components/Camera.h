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
		glm::mat4 GetProjMatrix() { return m_projMatrix; }
		/** @return the view matrix for this camera */
		glm::mat4 GetViewMatrix() { return m_viewMatrix; }

		float GetVerticalFoV();
		float GetNearClipPlane() { return m_nearClipPlane; }

	private:
		/** @return the direction the camera is facing in euler angles */
		glm::vec3 GetCameraDirection();
		/** Calculate and store the current projection matrix in m_projMatrix */
		void CalculateCurrentProjMatrix();
		/** Calculate and store the current view matrix in m_viewMatrix */
		void CalculateCurrentViewMatrix();

		bool m_cameraControl = false;

		float m_fieldOfView = 85.0f;
		float m_nearClipPlane = 0.01f;
		float m_farClipPlane = 1000.0f;

		float m_xSensitivity = 0.5f;
		float m_ySensitivity = 0.3f;
		float m_moveSpeed = 20.0f;

		glm::mat4 m_projMatrix;
		glm::mat4 m_viewMatrix;
	};
}
