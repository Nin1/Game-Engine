#include "stdafx.h"
#include "ControllableCamera.h"
#include <Core\FrameTime.h>
#include <Core\GameObject.h>
#include <Core\Input.h>

namespace snes
{
	void ControllableCamera::MainLogic()
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