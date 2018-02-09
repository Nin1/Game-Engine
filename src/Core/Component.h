#pragma once

namespace snes
{
	class GameObject;
	class Transform;

	/** Component base class
	  * Inherit from this to create new component types */
	class Component : public std::enable_shared_from_this<Component>
	{
	public:
		Component(GameObject& gameObject);

		~Component() {}

		/** Component function called once as soon as the component is created */
		virtual void Awake() {};

		/** Component function called once at the start of the first frame that the component exists for. */
		virtual void Start() {};

		/** Component function called at a guarunteed fixed interval */
		virtual void FixedLogic() {};

		/** Component function called at a variable interval equal to the time taken for last frame to process */
		virtual void MainLogic() {};

		/** Component function called at the end of each frame */
		virtual void MainDraw() {};

		/** Component function called whenever the component or parent GameObject is destroyed */
		virtual void OnDestroy() {};

		/** Component function called whenever a collider component attached to this game object is collided with */
		virtual void OnCollision(GameObject& other) {};

		/** @return this component's GameObject's transform */
		Transform& GetTransform() { return m_transform; }

	protected:
		GameObject& m_gameObject;
		Transform& m_transform;
	};
}
