#include "stdafx.h"
#include "TestComponent.h"
#include "Transform.h"
#include <Core\FrameTime.h>

namespace snes
{
	void TestComponent::MainLogic()
	{
		m_transform.Rotate(glm::vec3(0, 50, 0) * FrameTime::GetLastFrameDuration());
	}
}
