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

		virtual void MainLogic() override;

		void SetOrthographic(bool set) { m_orthographic = set; }

		/** @return the projection matrix for this camera */
		glm::mat4 GetProjMatrix() { return m_projMatrix; }
		/** @return the view matrix for this camera */
		glm::mat4 GetViewMatrix() { return m_viewMatrix; }

		float GetVerticalFoV();
		float GetNearClipPlane() { return m_nearClipPlane; }

	protected:
		/** @return the direction the camera is facing in euler angles */
		glm::vec3 GetCameraDirection();
		/** Calculate and store the current projection matrix in m_projMatrix */
		void CalculateCurrentProjMatrix();
		/** Calculate and store the current view matrix in m_viewMatrix */
		void CalculateCurrentViewMatrix();

		glm::mat4 m_projMatrix;
		glm::mat4 m_viewMatrix;

	private:
		bool m_orthographic = false;

		float m_fieldOfView = 85.0f;
		float m_nearClipPlane = 0.01f;
		float m_farClipPlane = 1000.0f;
	};
}
