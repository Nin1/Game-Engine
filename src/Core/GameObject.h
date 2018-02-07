#pragma once
#include <Components\Transform.h>

namespace snes
{
	class GameObject : public std::enable_shared_from_this<GameObject>
	{
	public:
		GameObject(std::shared_ptr<GameObject> parent);
		~GameObject() {};

		/** Run logic on all children and components at a fixed rate (60 times a second guaranteed, catch-up enabled) */
		void FixedLogic();
		/** Run logic on all children and components every frame */
		void MainLogic();
		/** Run MainDraw on all children and components every frame */
		void MainDraw();
		/** Run OnCollision events on all children and components */
		void OnCollision(GameObject& other);

		/** @return this GameObject's parent GameObject */
		std::shared_ptr<GameObject> GetParent() { return m_parent; }
		/** @return this GameObject's transform */
		Transform& GetTransform() { return m_transform; }

		/** Add a component to this GameObject, calling its Awake() function
		  * @return a weak_ptr to the component */
		template <typename T>
		std::weak_ptr<T> AddComponent()
		{
			std::shared_ptr<T> component = std::make_shared<T>(*this);
			m_components.emplace_back(component);
			component->Awake();
			return component;
		}

		/** @return the first component of the desired type found, or nullptr if none exist */
		template <typename T>
		std::shared_ptr<T> GetComponent()
		{
			for (auto& component : m_components)
			{
				std::shared_ptr<T> t = std::dynamic_pointer_cast<T>(component);

				if (t)
				{
					return t;
				}
			}

			return nullptr;
		}

		/** Create a GameObject as a child of this GameObject
		  * @return the created GameObject */
		std::weak_ptr<GameObject> AddChild();
		/** @return a vector containing this GameObject, all child GameObjects, their child GameObjects, etc. */
		std::vector<std::weak_ptr<GameObject>> GetAllChildren();

	private:
		std::vector<std::shared_ptr<Component>> m_components;
		std::vector<std::shared_ptr<GameObject>> m_children;
		std::shared_ptr<GameObject> m_parent;
		Transform m_transform;
	};
}