#include "stdafx.h"
#include "Camera.h"
#include <Core\FrameTime.h>
#include <Core\GameObject.h>
#include <Core\Input.h>

namespace snes
{
	void Camera::CalculateCurrentProjMatrix()
	{
		auto fov = glm::radians(m_fieldOfView);
		m_projMatrix = glm::perspective(fov, 800.0f / 600.0f, m_nearClipPlane, m_farClipPlane);
	}

	void Camera::CalculateCurrentViewMatrix()
	{
		const glm::vec3& cameraPosition = m_transform.GetWorldPosition();

		m_viewMatrix = glm::lookAt(
			cameraPosition,						// Camera position
			cameraPosition + GetCameraDirection(),	// Camera "look-at" point
			glm::vec3(0, 1, 0)					// Up vector
		);
	}

	glm::vec3 Camera::GetCameraDirection()
	{
		glm::vec3 cameraDirection;
		const glm::vec3& rot = m_transform.GetWorldRotationRadians();
		cameraDirection.x = cos(rot.x) * cos(rot.y);
		cameraDirection.y = sin(rot.x);
		cameraDirection.z = cos(rot.x) * sin(rot.y);
		return cameraDirection;
	}

	void Camera::MainLogic()
	{
		if (Input::GetKeyDown('c'))
		{
			m_cameraControl = !m_cameraControl;
		}

		if (m_cameraControl)
		{
			// Movement keys
			if (Input::GetKeyHeld('w'))
			{
				glm::vec3 translate = GetCameraDirection();
				m_transform.Translate(translate * m_moveSpeed * FrameTime::GetLastFrameDuration());
			}
			if (Input::GetKeyHeld('s'))
			{
				glm::vec3 translate = -GetCameraDirection();
				m_transform.Translate(translate * m_moveSpeed * FrameTime::GetLastFrameDuration());
			}
			if (Input::GetKeyHeld('d'))
			{
				glm::vec3 translate = glm::normalize(glm::cross(GetCameraDirection(), glm::vec3(0, 1, 0)));
				m_transform.Translate(translate * m_moveSpeed * FrameTime::GetLastFrameDuration());
			}
			if (Input::GetKeyHeld('a'))
			{
				glm::vec3 translate = -glm::normalize(glm::cross(GetCameraDirection(), glm::vec3(0, 1, 0)));
				m_transform.Translate(translate * m_moveSpeed * FrameTime::GetLastFrameDuration());
			}

			// Mouse input
			float yaw = Input::GetLastMouseOffset().x * m_xSensitivity;
			float pitch = -Input::GetLastMouseOffset().y * m_ySensitivity;
			glm::vec3 newRotation = m_transform.GetLocalRotation() + glm::vec3(pitch, yaw, 0);

			// Clamp pitch
			if (newRotation.x > 89.0f)
			{
				newRotation.x = 89.0f;
			}
			else if (newRotation.x < -89.0f)
			{
				newRotation.x = -89.0f;
			}

			m_transform.SetLocalRotation(newRotation);

			if (Input::GetMousePos().x < 300 ||
				Input::GetMousePos().x > 500 ||
				Input::GetMousePos().y < 200 ||
				Input::GetMousePos().y > 400)
			{
				Input::WarpMousePos(400, 300);
			}
		}

		/** Calculate the view/proj matrices this frame */
		CalculateCurrentProjMatrix();
		CalculateCurrentViewMatrix();
	}
}