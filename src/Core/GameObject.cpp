#include "stdafx.h"
#include "GameObject.h"
#include <Components\Collider.h>

namespace snes
{
	GameObject::GameObject(std::shared_ptr<GameObject> parent)
		: m_parent(parent)
		, m_transform(*this)
	{
	}
	
	std::weak_ptr<GameObject> GameObject::AddChild()
	{
		std::shared_ptr<GameObject> child = std::make_shared<GameObject>(shared_from_this());
		m_children.push_back(child);
		return child;
	}

	std::vector<std::weak_ptr<GameObject>> GameObject::GetAllChildren()
	{
		std::vector<std::weak_ptr<GameObject>> allChildren;
		for (auto& child : m_children)
		{
			std::vector<std::weak_ptr<GameObject>> grandChildren = child->GetAllChildren();
			for (auto& grandChild : grandChildren)
			{
				allChildren.push_back(grandChild);
			}
			allChildren.push_back(child);
		}
		return allChildren;
	}

	void GameObject::FixedLogic()
	{
		for (auto& child : m_children)
		{
			child->FixedLogic();
		}

		for (auto& component : m_components)
		{
			if (component->IsEnabled())
			{
				component->FixedLogic();
			}
		}
	}

	void GameObject::MainLogic()
	{
		for (auto& child : m_children)
		{
			child->MainLogic();
		}

		for (auto& component : m_components)
		{
			if (component->IsEnabled())
			{
				component->MainLogic();
			}
		}
	}

	void GameObject::MainDraw(RenderPass renderPass, Camera& camera)
	{
		for (auto& child : m_children)
		{
			child->MainDraw(renderPass, camera);
		}

		for (auto& component : m_components)
		{
			if (component->IsEnabled())
			{
				component->MainDraw(renderPass, camera);
			}
		}
	}

	void GameObject::OnCollision(GameObject& other)
	{
		for (auto& child : m_children)
		{
			child->OnCollision(other);
		}

		for (auto& component : m_components)
		{
			if (component->IsEnabled())
			{
				component->OnCollision(other);
			}
		}
	}
}