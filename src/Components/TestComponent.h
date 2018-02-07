#pragma once
#include <Core\Component.h>

namespace snes
{
	class TestComponent :
		public Component
	{
	public:
		TestComponent(GameObject& gameObject)
			: Component(gameObject) {}
		~TestComponent() {};

		void MainLogic() override;
	};
}
