#include "stdafx.h"
#include "Camera.h"
#include <Core\FrameTime.h>
#include <Core\GameObject.h>
#include <Core\Input.h>

namespace snes
{
	void Camera::CalculateCurrentProjMatrix()
	{
		if (!m_orthographic)
		{
			auto fov = glm::radians(m_fieldOfView);
			m_projMatrix = glm::perspective(fov, 800.0f / 600.0f, m_nearClipPlane, m_farClipPlane);
		}
		else
		{
			m_projMatrix = glm::ortho(-100.0f, 100.0f, -75.0f, 75.0f, -100.0f, 100.0f);
		}
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
		/** Calculate the view/proj matrices this frame */
		CalculateCurrentProjMatrix();
		CalculateCurrentViewMatrix();
	}

	float Camera::GetVerticalFoV()
	{
		return (m_fieldOfView / 4) * 3; // 4:3
	}
}