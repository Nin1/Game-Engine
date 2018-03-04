#pragma once
#include "Camera.h"
#include <glm\gtc\matrix_transform.hpp>

namespace snes
{
	class ControllableCamera : public Camera
	{
	public:
		ControllableCamera(GameObject& gameObject) : Camera(gameObject) {}
		~ControllableCamera() {};

		/** Set whether this camera is being controlled by the keyboard/mouse */
		void SetCameraControl(bool on) { m_cameraControl = on; }

		void MainLogic() override;

	private:

		bool m_cameraControl = false;

		float m_xSensitivity = 0.5f;
		float m_ySensitivity = 0.3f;
		float m_moveSpeed = 20.0f;
	};
}
