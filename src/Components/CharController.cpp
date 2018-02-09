#include "stdafx.h"
#include "CharController.h"
#include "Rigidbody.h"
#include <Core\FrameTime.h>
#include <Core\GameObject.h>
#include <Core\Input.h>
#include <GL/freeglut.h>

namespace snes
{
	CharController::CharController(GameObject& gameObject) : Component(gameObject)
	{
	}


	CharController::~CharController()
	{
	}

	void CharController::Awake()
	{
		auto rb = m_gameObject.GetComponent<Rigidbody>();
		if (rb)
		{
			rb->SetDrag(glm::vec3(200.0f, 0.0f, 200.0f));
			rb->SetMaxLateralSpeed(1.0f);
		}
	}

	void CharController::MainLogic()
	{
		auto rb = m_gameObject.GetComponent<Rigidbody>();

		if (Input::GetKeyDown('c'))
		{
			m_charControl = !m_charControl;
		}

		if (rb && m_charControl)
		{
			if (Input::GetKeyHeld('w'))
			{
				rb->AddVelocity(glm::vec3(0, 0, -100.0f) * FrameTime::GetLastFrameDuration());
			}
			if (Input::GetKeyHeld('s'))
			{
				rb->AddVelocity(glm::vec3(0, 0, 100.0f) * FrameTime::GetLastFrameDuration());
			}
			if (Input::GetKeyHeld('d'))
			{
				rb->AddVelocity(glm::vec3(100.0f, 0, 0) * FrameTime::GetLastFrameDuration());
			}
			if (Input::GetKeyHeld('a'))
			{
				rb->AddVelocity(glm::vec3(-100.0f, 0, 0) * FrameTime::GetLastFrameDuration());
			}
			if (Input::GetKeyDown(' '))
			{
				rb->AddVelocity(glm::vec3(0, 9.81f, 0));
			}
		}
	}
}
