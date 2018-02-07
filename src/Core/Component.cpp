#include "stdafx.h"
#include "Component.h"
#include "GameObject.h"

namespace snes
{
	Component::Component(GameObject& gameObject)
		: m_gameObject(gameObject)
		, m_transform(gameObject.GetTransform())
	{

	}
}
